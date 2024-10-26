#include "ServerLeaderFollower.hpp"

LFServer::LFServer(int port, int numThreads) : port(port), numThreads(numThreads), isLeaderAvailable(true)
{
    // Initialize the LFServer socket (same as before)
    LFServer_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (LFServer_fd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(LFServer_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        close(LFServer_fd);
        exit(EXIT_FAILURE);
    }

    // Bind to the specified port
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Any address
    address.sin_port = htons(port);

    if (bind(LFServer_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(LFServer_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(LFServer_fd, SOMAXCONN) < 0)
    {
        perror("listen failed");
        close(LFServer_fd);
        exit(EXIT_FAILURE);
    }

    // Set LFServer_fd to non-blocking
    int flags = fcntl(LFServer_fd, F_GETFL, 0);
    fcntl(LFServer_fd, F_SETFL, flags | O_NONBLOCK);

    // Initialize poll_fds with LFServer_fd
    struct pollfd LFServer_poll_fd;
    LFServer_poll_fd.fd = LFServer_fd;
    LFServer_poll_fd.events = POLLIN;
    poll_fds.push_back(LFServer_poll_fd);
}

LFServer::~LFServer()
{
    close(LFServer_fd);
    for (int fd : client_fds)
    {
        close(fd);
    }
}

void LFServer::run()
{
    // Create the thread pool
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(&LFServer::workerThread, this);
    }

    // Wait for threads to finish (they won't unless the LFServer is stopped)
    for (auto &thread : threads)
    {
        thread.join();
    }
}

void LFServer::workerThread()
{
    while (true)
    {
        // Leader-Follower pattern implementation
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // Wait until this thread can become the leader
            condVar.wait(lock, [this]()
                         { return isLeaderAvailable; });
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

        if (poll_count < 0)
        {
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
            for (size_t i = 0; i < poll_fds.size(); ++i)
            {
                if (poll_fds[i].revents & POLLIN)
                {
                    if (poll_fds[i].fd == LFServer_fd)
                    {
                        // Accept new connection
                        acceptNewConnection();
                    }
                    else
                    {
                        // Handle data from client
                        int client_fd = poll_fds[i].fd;
                        handleClientData(client_fd);
                    }
                }
            }
        }
    }
}

void LFServer::acceptNewConnection()
{
    int new_socket = accept(LFServer_fd, NULL, NULL);
    if (new_socket < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
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

void LFServer::handleClientData(int client_fd)
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

            client_fds.erase(std::remove(client_fds.begin(), client_fds.end(), client_fd), client_fds.end());
            client_buffers.erase(client_fd);
        }
    }
    else
    {
        // Append to client's buffer
        client_buffers[client_fd].append(buffer, bytes_read);

        // Check for complete message (e.g., newline-terminated)
        size_t pos;
        while ((pos = client_buffers[client_fd].find('\n')) != std::string::npos)
        {
            std::string message = client_buffers[client_fd].substr(0, pos);
            client_buffers[client_fd].erase(0, pos + 1);

            // Process message (synchronously for simplicity)
            // In practice, you might use a task queue or dispatch to another thread
            std::string response = processClientMessage(message);
            write(client_fd, response.c_str(), response.length());
        }
    }
}

std::string LFServer::processClientMessage(const std::string &message)
{
    // Parse the message
    std::string response = LFparse(message);
    return response;
}

std::string LFprintMenu()
{
    std::stringstream ss;
    ss << "Menu:\n";
    ss << " 1. Create New Graph           - `newgraph v e`\n";
    ss << " 2. Add Edge                   - `addedge i j w`\n";
    ss << " 3. Remove Edge                - `removeedge i j`\n";
    ss << " 4. Compute MST                - `boruvka`/`kruskal`/`prim`\n";
    ss << " 5. Get Longest Path           - `longestpath`\n";
    ss << " 6. Get Shortest Path          - `shortestpath\n";
    ss << " 7. Get Average Path           - `avgpath`\n";
    ss << " 8. Get Total Path             - `totalweight`\n";
    ss << " 9. Print                      - `print`\n";
    ss << "10. Exit                       - `exit`\n";
    ss << "11. Help (show this help text) - `help`\n";
    return ss.str();
}

void LFServer::LFaddedge(std::string uStr, std::string vStr, std::string wStr)
{
    int u = 0, v = 0, w = 0;
    std::cout << "Adding edge " << uStr << " " << vStr << " " << wStr << "\n";
    try
    {
        u = std::stoi(uStr) - 1;
        v = std::stoi(vStr) - 1;
        w = std::stoi(wStr);
        std::cout << "Adding edge " << u << " " << v << " " << w << "\n";
        this->graph->addEdge(u, v, w);
    }
    catch (const std::invalid_argument &)
    {
        std::cerr << "Invalid arguments for LFaddedge!\n";
    }
}

std::string LFServer::LFparse(std::string input)
{
    std::string result;
    int n = 0, m = 0, u = 0, v = 0;

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
        return "Invalid Command!\n";
    }

    const std::string &command = tokens[0];

    if (tokens.size() > 1)
    {
        if (command == "newgraph" || this->newGraphFlag)
        {
            if (this->newGraphFlag)
            {
                LFaddedge(tokens[0], tokens[1], tokens[2]);
                this->counter--;
                if (this->counter == 0)
                    this->newGraphFlag = false;
            }
            else
            {
                try
                {
                    n = std::stoi(tokens[1]);
                    m = std::stoi(tokens[2]);
                    this->counter = m;
                    this->graph = new Graph(n);

                    newGraphFlag = true;
                }
                catch (const std::invalid_argument &)
                {
                    std::cerr << "Invalid arguments for newgraph!\n";
                }
            }
        }
        else if (command == "addedge")
        {
            LFaddedge(tokens[1], tokens[2], tokens[3]);

            result = "Edge added successfully.\n";
        }
        else if (command == "removeedge")
        {
            try
            {
                u = std::stoi(tokens[1]) - 1;
                v = std::stoi(tokens[2]) - 1;
                this->graph->removeEdge(u, v);
            }
            catch (const std::invalid_argument &)
            {
                std::cerr << "Invalid arguments for removeedge!\n";
            }

            result = "Edge removed successfully.\n";
        }
    }
    else if (command == "kruskal" || command == "prim" || command == "boruvka")
    {
        try
        {
            if (this->graph != nullptr)
            {
                this->mst = new MST(*this->graph, command);
                result = "Created MST using " + command + " algorithm.\n";
            }
            else
            {
                result = "Must create Graph first!";
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    else if (command == "longestpath")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Longest path of the MST: " + std::to_string(mst->getLongestDistance()) + ".\n";
    }
    else if (command == "shortestpath")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Shortest path of the MST: " + std::to_string(mst->getShortestDistance()) + ".\n";
    }
    else if (command == "avgpath")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Average path of the MST: " + std::to_string(mst->getAverageDistance()) + ".\n";
    }
    else if (command == "totalweight")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Total Weight of the MST: " + std::to_string(mst->getTotalWeight()) + ".\n";
    }
    else if (command == "print")
    {   
        if (this->graph != nullptr)
        {
            result = "Graph:\n";
            result += this->graph->printGraph();
        }
        if (this->mst != nullptr)
        {
            result += "MST:\n";
            result += this->mst->printGraph();
        }
    }
    else if (command == "exit")
    {
        exit(EXIT_SUCCESS);
    }
    else if (command == "help")
    {
        result = LFprintMenu();
    }
    else
    {
        // Unrecognized command
        std::cerr << "Unrecognized command.\n";
    }

    return result;
}
