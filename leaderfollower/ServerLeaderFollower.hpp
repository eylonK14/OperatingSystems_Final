#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
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
#include "../structures/MST.hpp"

/**
 * @class LFServer
 * @brief A server class implementing the Leader-Follower pattern.
 *
 * This class manages a server that listens on a specified port and handles
 * multiple client connections using a pool of worker threads. It uses the
 * Leader-Follower pattern to efficiently manage client requests.
 */
class LFServer
{
public:
    /**
     * @brief Constructor for LFServer.
     * @param port The port number on which the server listens for incoming connections.
     * @param numThreads The number of worker threads in the thread pool.
     */
    LFServer(int port, int numThreads);

    /**
     * @brief Destructor for LFServer.
     */
    ~LFServer();

    /**
     * @function run
     * @brief Starts the server and begins accepting and processing client connections.
     */
    void run();

private:
    /**
     * @function workerThread
     * @brief Function executed by each worker thread to handle client connections.
     */
    void workerThread();

    /**
     * @function acceptNewConnection
     * @brief Accepts a new client connection and assigns it to a worker thread.
     */
    void acceptNewConnection();

    /**
     * @function handleClientData
     * @brief Handles data received from a client.
     * @param client_fd The file descriptor of the client connection.
     */
    void handleClientData(int client_fd);

    /**
     * @function processClientMessage
     * @brief Processes a message received from a client.
     * @param message The message received from the client.
     * @return The response to be sent back to the client.
     */
    std::string processClientMessage(const std::string &message);

    /**
     * @brief Parses the given string according to the Leader-Follower pattern.
     *
     * This function takes a string as input and processes it based on the
     * Leader-Follower design pattern. The exact parsing logic depends on the
     * implementation details within the function.
     *
     * @param input The string to be parsed.
     * @return A parsed string based on the Leader-Follower pattern.
     */
    std::string LFparse(std::string);

    /**
     * @brief Adds an edge to the graph in the Leader-Follower server.
     *
     * This function adds an edge between two nodes in the graph, with an optional label or weight.
     *
     * @param node1 The identifier of the first node.
     * @param node2 The identifier of the second node.
     * @param label The label or weight of the edge.
     */
    void LFaddedge(std::string, std::string, std::string);

    int LFServer_fd;
    int port;
    int numThreads;

    bool newGraphFlag;
    int counter;
    Graph *graph;
    MST *mst;

    std::vector<std::thread> threads;

    std::mutex mutex_;
    std::condition_variable condVar;
    bool isLeaderAvailable;
    std::thread::id leaderThreadId;

    std::vector<int> client_fds;
    std::map<int, std::string> client_buffers;

    std::vector<struct pollfd> poll_fds;
};
