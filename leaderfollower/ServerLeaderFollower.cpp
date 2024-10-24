#include "ServerLeaderFollower.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <errno.h>
#include <sstream>
#include <algorithm>

Server::Server(int port, int numThreads) : port(port), numThreads(numThreads), isLeaderAvailable(true) {
    // Initialize the server socket (same as before)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
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

    // Initialize poll_fds with server_fd
    struct pollfd server_poll_fd;
    server_poll_fd.fd = server_fd;
    server_poll_fd.events = POLLIN;
    poll_fds.push_back(server_poll_fd);
}

Server::~Server() {
    close(server_fd);
    for (int fd : client_fds) {
        close(fd);
    }
}

void Server::run() {
    // Create the thread pool
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(&Server::workerThread, this);
    }

    // Wait for threads to finish (they won't unless the server is stopped)
    for (auto& thread : threads) {
        thread.join();
    }
}

void Server::workerThread() {
    while (true) {
        // Leader-Follower pattern implementation
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // Wait until this thread can become the leader
            condVar.wait(lock, [this]() { return isLeaderAvailable; });
            // Become the leader
            isLeaderAvailable = false;
            leaderThreadId = std::this_thread::get_id();
        }

        // Leader thread waits for events
        int timeout = -1; // Wait indefinitely
        int poll_count;

        {
            // Lock the mutex while accessing poll_fds
            std::unique_lock<std::mutex> lock(mutex_);
            poll_count = poll(poll_fds.data(), poll_fds.size(), timeout);
        }

        if (poll_count < 0) {
            perror("poll failed");
            break;
        }

        // Before handling events, promote a follower to leader
        {
            std::unique_lock<std::mutex> lock(mutex_);
            isLeaderAvailable = true;
            condVar.notify_one();
        }

        // Now handle events (as the previous leader)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            for (size_t i = 0; i < poll_fds.size(); ++i) {
                if (poll_fds[i].revents & POLLIN) {
                    if (poll_fds[i].fd == server_fd) {
                        // Accept new connection
                        acceptNewConnection();
                    } else {
                        // Handle data from client
                        int client_fd = poll_fds[i].fd;
                        handleClientData(client_fd);
                    }
                }
            }
        }
    }
}

void Server::acceptNewConnection() {
    int new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            perror("accept failed");
        }
        return;
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
}

void Server::handleClientData(int client_fd) {
    char buffer[1024];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));

    if (bytes_read <= 0) {
        if (bytes_read == 0 || (bytes_read < 0 && errno != EWOULDBLOCK && errno != EAGAIN)) {
            // Client disconnected or error
            std::cout << "Client disconnected: FD " << client_fd << std::endl;
            close(client_fd);

            // Remove from poll_fds
            auto it = std::find_if(poll_fds.begin(), poll_fds.end(), [client_fd](const struct pollfd& pfd) {
                return pfd.fd == client_fd;
            });
            if (it != poll_fds.end()) {
                poll_fds.erase(it);
            }

            client_fds.erase(std::remove(client_fds.begin(), client_fds.end(), client_fd), client_fds.end());
            client_buffers.erase(client_fd);
        }
    } else {
        // Append to client's buffer
        client_buffers[client_fd].append(buffer, bytes_read);

        // Check for complete message (e.g., newline-terminated)
        size_t pos;
        while ((pos = client_buffers[client_fd].find('\n')) != std::string::npos) {
            std::string message = client_buffers[client_fd].substr(0, pos);
            client_buffers[client_fd].erase(0, pos + 1);

            // Process message (synchronously for simplicity)
            // In practice, you might use a task queue or dispatch to another thread
            std::string response = processClientMessage(message);
            write(client_fd, response.c_str(), response.length());

        }
    }
}

std::string Server::processClientMessage(const std::string& message) {
    // Parse the message
    std::string response = parse(message);
    return response;
}

std::string printMenu()
{
    std::stringstream ss;
    ss << "Menu:\n";
    ss << " 1. Create New Graph           - `newgraph v, e`\n";
    ss << " 2. Add Edge                   - `addedge i, j, w`\n";
    ss << " 3. Remove Edge                - `removeedge i, j`\n";
    ss << " 4. Compute MST                - `boruvka`/`kruskal`/`prim`\n";
    ss << " 5. Get Longest Path           - `longestpath`\n";
    ss << " 6. Get Shortest Path          - `shortestpath\n";
    ss << " 7. Get Average Path           - `avgpath`\n";
    ss << " 8. Get Total Path             - `totalweight`\n";
    ss << " 9. Print MST                  - `print`\n";
    ss << "10. Exit                       - `exit`\n";
    ss << "11. Help (show this help text) - `help`\n";
    return ss.str();
}

void Server::addedge(std::vector<std::string> &tokens)
{
    std::istringstream uv_stream(tokens[1]);
    std::string u_str, v_str, w_str;
    if (std::getline(uv_stream, u_str, ',') && std::getline(uv_stream, v_str, ',') && std::getline(uv_stream, w_str))
    {
        try
        {
            int u = std::stoi(u_str) - 1;
            int v = std::stoi(v_str) - 1;
            int w = std::stoi(w_str);
            this->graph->addEdge(u, v, w);
        }
        catch (const std::invalid_argument &)
        {
            std::cerr << "Invalid arguments for addedge.\n";
        }
    }
}

std::string Server::parse(std::string input)
{
    std::string result;

    // Remove trailing newline character if present
    if (!input.empty() && input.back() == '\n')
        input.pop_back();

    // Use a string stream to tokenize the input
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }

    if (tokens.empty())
    {
        // Empty input, do nothing
        return "Invalid Command";
    }

    const std::string& command = tokens[0];

    if(tokens.size() > 1)
    {
        if (command == "newgraph" || this->newGraphFlag)
        {
            if (this->newGraphFlag)
            {
                // insert edges
                addedge(tokens);
                this->counter--;
                if (this->counter == 0)
                {
                    this->newGraphFlag = false;
                }
            }
            else
            {
                std::istringstream nm_stream(tokens[1]);
                std::string n_str, m_str;
                if (std::getline(nm_stream, n_str, ',') && std::getline(nm_stream, m_str))
                {
                    try
                    {
                        int n = std::stoi(n_str);
                        int m = std::stoi(m_str);
                        this->counter = m;
                        Graph my_graph(n);
                        this->graph = new Graph(my_graph);

                        newGraphFlag = true;
                    }
                    catch (const std::invalid_argument&)
                    {
                        std::cerr << "Invalid arguments for newgraph.\n";
                    }
                }
            }
        }
        else if (command == "addedge")
        {
            addedge(tokens);

            result = "Edge added successfully";
        }
        else if (command == "removeedge")
        {
            std::istringstream uv_stream(tokens[1]);
            std::string u_str, v_str;
            if (std::getline(uv_stream, u_str, ',') && std::getline(uv_stream, v_str))
            {
                try
                {
                    int u = std::stoi(u_str) - 1;
                    int v = std::stoi(v_str) - 1;
                    this->graph->removeEdge(u, v);
                }
                catch (const std::invalid_argument&)
                {
                    std::cerr << "Invalid arguments for removeedge.\n";
                }
            }

        result = "Edge removed successfully";
        }
    }
    else if (command == "kruskal" || command == "prim" || command == "boruvka")
    {
        this->mst = new MST(*this->graph, command);
        result = "Created MST using " + command + " algorithm";
    }
    else if (command == "longestpath")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Longest path of the MST: " + mst->getLongestDistance();
    }
    else if (command == "shortestpath")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Shortest path of the MST: " + mst->getShortestDistance();
    }
    else if (command == "avgpath")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Average path of the MST: " + mst->getAverageDistance();
    }
    else if (command == "totalweight")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Total Weight of the MST: " + mst->getTotalWeight();
    }
    else if (command == "exit")
    {
        exit(EXIT_SUCCESS);
    }
    else if (command == "help")
    {
        result = printMenu();
    }
    else
    {
        // Unrecognized command
        std::cerr << "Unrecognized command.\n";
    }

    return result;
}