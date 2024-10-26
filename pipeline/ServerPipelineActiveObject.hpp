#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include "Pipeline.hpp"

class PLServer
{
public:
    PLServer(int port);
    ~PLServer();
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
    void handleClientData(int, std::vector<struct pollfd> &);
    // std::vector<int> client_fds;
    // std::map<int, std::string> client_buffers;
    // other private members and methods
};
