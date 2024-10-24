#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Pipeline.hpp"

class Server {
public:
    Server(int port);
    ~Server();
    void run();

private:
    int server_fd;
    int port;
    Pipeline pipeline;
    std::vector<int> client_fds;
    std::map<int, std::string> client_buffers;
    // other private members and methods
};
