#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sys/socket.h>
#include "ActiveObject.hpp"
#include "../structures/Graph.hpp"
#include "../structures/MST.hpp"


class Pipeline
{
public:
    Pipeline();
    ~Pipeline();
    std::string handleRequest(int, std::string request);
    
private:
    void createGraph(int fd);
    void modifyGraph(int fd);
    void createMST(int fd);
    void statisticsMST(int fd);
    std::unique_ptr<Graph> graph;
    std::unique_ptr<MST> mst;
    std::vector<std::shared_ptr<ActiveObject>> stages;
};
