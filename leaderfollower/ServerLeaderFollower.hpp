#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "../structures/MST.hpp"


class Server {
public:
    Server(int port, int numThreads);
    ~Server();
    void run();

private:
    void workerThread();
    void acceptNewConnection();
    void handleClientData(int);
    std::string processClientMessage(const std::string&);

    int server_fd;
    int port;
    int numThreads;

    bool newGraphFlag;
    int counter;
    Graph* graph;
    MST* mst;

    std::string parse(std::string);
    void addedge(std::vector<std::string> &);

    // Thread pool
    std::vector<std::thread> threads;

    // Synchronization
    std::mutex mutex_;
    std::condition_variable condVar;
    bool isLeaderAvailable;
    std::thread::id leaderThreadId;

    // Client management
    std::vector<int> client_fds;
    std::map<int, std::string> client_buffers;

    // File descriptors for polling
    std::vector<struct pollfd> poll_fds;

    // Other private members and methods
};
