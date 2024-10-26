#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include "Pipeline.hpp"

/**
 * @class PLServer
 * @brief A server class that handles client connections and processes data through a pipeline.
 *
 * The PLServer class is responsible for managing client connections, receiving data from clients,
 * and processing the data using a pipeline. It uses a map to keep track of client connections and
 * a mutex to ensure thread safety when accessing the client connections.
 *
 * @note This class is part of the ServerPipelineActiveObject module.
 */
class PLServer
{
public:
    /**
     * @brief Constructs a PLServer object with the specified port.
     *
     * This constructor initializes a PLServer instance that listens on the given port.
     *
     * @param port The port number on which the server will listen for incoming connections.
     */
    PLServer(int port);

    /**
     * @brief Destructor for the PLServer class.
     *
     * This destructor is responsible for cleaning up any resources
     * allocated by the PLServer instance. It ensures that all
     * necessary cleanup operations are performed when an object
     * of this class is destroyed.
     */
    ~PLServer();

    /**
     * @brief Executes the main logic of the active object.
     *
     * This method is responsible for running the core functionality
     * of the active object. It typically involves processing tasks,
     * handling events, or performing operations in a loop until a
     * termination condition is met.
     */
    void run();

private:
    struct ClientConnection
    {
        int fd;
        std::string buffer;
    };

    int PLServer_fd;
    int port;
    Pipeline pipeline;
    std::map<int, std::shared_ptr<ClientConnection>> clients;
    std::mutex client_mutex;

    /**
     * @brief Handles the data received from a client.
     *
     * This function processes the data received from a client identified by the given file descriptor.
     * It utilizes a vector of pollfd structures to manage multiple file descriptors for polling.
     *
     * @param clientFd The file descriptor of the client.
     * @param pollFds A reference to a vector of pollfd structures used for polling multiple file descriptors.
     */
    void handleClientData(int, std::vector<struct pollfd> &);
};
