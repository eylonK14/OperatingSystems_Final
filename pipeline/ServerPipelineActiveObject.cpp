#include "ServerPipelineActiveObject.hpp"
#include "ActiveObject.hpp"
#include "ExamplePipelineStage.hpp" // Custom pipeline stages
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <errno.h>
#include <algorithm>

PLServer::PLServer(int port) : port(port)
{
    // Initialize the PLServer socket
    PLServer_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (PLServer_fd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    int opt = 1;
    if (setsockopt(PLServer_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        close(PLServer_fd);
        exit(EXIT_FAILURE);
    }

    // Bind to the specified port
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Any address
    address.sin_port = htons(port);

    if (bind(PLServer_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(PLServer_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(PLServer_fd, SOMAXCONN) < 0)
    {
        perror("listen failed");
        close(PLServer_fd);
        exit(EXIT_FAILURE);
    }

    // Set PLServer_fd to non-blocking
    int flags = fcntl(PLServer_fd, F_GETFL, 0);
    fcntl(PLServer_fd, F_SETFL, flags | O_NONBLOCK);

    // Initialize pipeline and add stages
    pipeline.addStage(std::make_shared<ExamplePipelineStage>());
    // Add more stages as needed
}

PLServer::~PLServer()
{
    for (auto &client : clients)
    {
        close(client.first);
    }
    clients.clear();
    close(PLServer_fd);
}

void PLServer::handleClientData(int client_fd, std::vector<pollfd> &poll_fds)
{
    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));

    if (bytes_read <= 0)
    {
        if (bytes_read == 0 || (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN))
        {
            // Client disconnected or error
            std::cout << "Client disconnected: FD " << client_fd << std::endl;
            close(client_fd);

            // Remove from poll_fds
            auto it = std::find_if(poll_fds.begin(), poll_fds.end(), [client_fd](const struct pollfd &pfd)
                                   { return pfd.fd == client_fd; });
            if (it != poll_fds.end())
            {
                poll_fds.erase(it);
            }

            // Remove client from clients map
            {
                std::lock_guard<std::mutex> lock(client_mutex);
                clients.erase(client_fd);
            }
        }
    }
    else
    {
        std::shared_ptr<ClientConnection> client;
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            auto it = clients.find(client_fd);
            if (it != clients.end())
            {
                client = it->second;
                client->buffer.append(buffer, bytes_read);
            }
            else // Client not found, maybe disconnected
                return;
        }

        // Check for complete messages (newline-terminated)
        size_t pos;
        while ((pos = client->buffer.find('\n')) != std::string::npos)
        {
            std::string message = client->buffer.substr(0, pos);
            client->buffer.erase(0, pos + 1);

            // Process message through the pipeline
            std::weak_ptr<ClientConnection> client_weak = client;
            pipeline.process(message, [this, client_weak](const std::string &output)
                             {
                // Output callback executed in another thread
                if (auto client = client_weak.lock()) {
                    int fd = client->fd;
                    std::string response = output + "\n";
                    ssize_t bytes_written = write(fd, response.c_str(), response.length());
                    if (bytes_written < 0) {
                        perror("write failed");
                    }
                } else {
                    // Client has disconnected
                    std::cout << "Client disconnected before output could be sent" << std::endl;
                } });
        }
    }
}

void PLServer::run()
{
    // Use poll to handle multiple clients
    std::vector<struct pollfd> poll_fds;

    // Add PLServer_fd to poll_fds
    struct pollfd PLServer_poll_fd;
    PLServer_poll_fd.fd = PLServer_fd;
    PLServer_poll_fd.events = POLLIN;
    poll_fds.push_back(PLServer_poll_fd);

    while (true)
    {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0)
        {
            if (errno == EINTR) // Interrupted by signal, retry poll
                continue;
            perror("poll failed");
            break;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i)
        {
            if (poll_fds[i].revents & POLLIN)
            {
                if (poll_fds[i].fd == PLServer_fd)
                {
                    // Accept new connection
                    int new_socket = accept(PLServer_fd, NULL, NULL);
                    if (new_socket < 0)
                    {
                        // ... Error handling ...
                        if (errno != EWOULDBLOCK && errno != EAGAIN)
                            perror("accept failed");
                        continue;
                    }

                    // Set new_socket to non-blocking
                    int flags = fcntl(new_socket, F_GETFL, 0);
                    fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);

                    // Create a new ClientConnection
                    auto client = std::make_shared<ClientConnection>();
                    client->fd = new_socket;
                    client->buffer = "";

                    // Add client to clients map
                    {
                        std::lock_guard<std::mutex> lock(client_mutex);
                        clients[new_socket] = client;
                    }

                    // Add new_socket to poll_fds
                    struct pollfd client_poll_fd;
                    client_poll_fd.fd = new_socket;
                    client_poll_fd.events = POLLIN;
                    poll_fds.push_back(client_poll_fd);

                    std::cout << "New client connected: FD " << new_socket << std::endl;
                }
                else
                {
                    // Handle data from client
                    int client_fd = poll_fds[i].fd;
                    handleClientData(client_fd, poll_fds);
                }
            }
        }
    }
}
