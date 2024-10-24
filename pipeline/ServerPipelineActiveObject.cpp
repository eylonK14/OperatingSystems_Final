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

Server::Server(int port) : port(port) {
    // Initialize the server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind to the specified port
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Any address
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Set server_fd to non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // Initialize pipeline and add stages
    pipeline.addStage(std::make_shared<ExamplePipelineStage>());
    // Add more stages as needed
}

Server::~Server() {
    close(server_fd);
    for (int fd : client_fds) {
        close(fd);
    }
}

void Server::run() {
    // Use poll to handle multiple clients
    std::vector<struct pollfd> poll_fds;

    // Add server_fd to poll_fds
    struct pollfd server_poll_fd;
    server_poll_fd.fd = server_fd;
    server_poll_fd.events = POLLIN;
    poll_fds.push_back(server_poll_fd);

    while (true) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            perror("poll failed");
            break;
        }

        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_fd) {
                    // Accept new connection
                    int new_socket = accept(server_fd, NULL, NULL);
                    if (new_socket < 0) {
                        if (errno != EWOULDBLOCK && errno != EAGAIN) {
                            perror("accept failed");
                        }
                        continue;
                    }

                    // Set new_socket to non-blocking
                    int flags = fcntl(new_socket, F_GETFL, 0);
                    fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);

                    // Add new_socket to poll_fds
                    struct pollfd client_poll_fd;
                    client_poll_fd.fd = new_socket;
                    client_poll_fd.events = POLLIN;
                    poll_fds.push_back(client_poll_fd);
                    client_fds.push_back(new_socket);
                    client_buffers[new_socket] = "";

                    std::cout << "New client connected: FD " << new_socket << std::endl;
                } else {
                    // Handle data from client
                    int client_fd = poll_fds[i].fd;
                    char buffer[1024];
                    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));

                    if (bytes_read <= 0) {
                        if (bytes_read == 0 || (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN)) {
                            // Client disconnected or error
                            std::cout << "Client disconnected: FD " << client_fd << std::endl;
                            close(client_fd);
                            poll_fds.erase(poll_fds.begin() + i);
                            client_fds.erase(std::remove(client_fds.begin(), client_fds.end(), client_fd), client_fds.end());
                            client_buffers.erase(client_fd);
                            --i;
                        }
                        continue;
                    } else {
                        // Append to client's buffer
                        client_buffers[client_fd].append(buffer, bytes_read);

                        // Check for complete message (e.g., newline-terminated)
                        size_t pos;
                        while ((pos = client_buffers[client_fd].find('\n')) != std::string::npos) {
                            std::string message = client_buffers[client_fd].substr(0, pos);
                            client_buffers[client_fd].erase(0, pos + 1);

                            // Process message through the pipeline
                            pipeline.process(message, [this, client_fd](const std::string& output) {
                                // Send the output back to the client
                                std::string response = output + "\n";
                                write(client_fd, response.c_str(), response.length());
                            });
                        }
                    }
                }
            }
        }
    }
}
