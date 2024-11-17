#include "Pipeline.hpp"

Pipeline::Pipeline()
{
    // init the stages vector with 4 ActiveObjects
    for (int i = 0; i < 4; i++)
    {
        stages.push_back(std::move(std::make_shared<ActiveObject>()));
        stages[i]->previousTaskFinished = false;
    }
    // stages[0]->setNext(stages[1]);
    // stages[1]->setNext(stages[2]);
    // stages[2]->setNext(stages[3]);

    stages[0]->previousTaskFinished = true;

    stages[0]->currentTask = [this](int fd) { this->createGraph(fd); };
    stages[1]->currentTask = [this](int fd) { this->modifyGraph(fd); };
    stages[2]->currentTask = [this](int fd) { this->createMST(fd); };
    stages[3]->currentTask = [this](int fd) { this->statisticsMST(fd); };
}

Pipeline::~Pipeline()
{
    // delete the stages vector
    stages.clear();
}

std::string Pipeline::handleRequest(int fd, std::string request)
{
    if (request == "newgraph")
    {
        stages[0]->enqueue(fd);
    }
    else if (request == "modifygraph")
    {
        stages[1]->enqueue(fd);
    }
    else if (request == "newmst")
    {
        stages[2]->enqueue(fd);
    }
    else if (request == "statistics")
    {
        stages[3]->enqueue(fd);
    }
}

// Create a new graph conversation
void Pipeline::createGraph(int fd){
    std::string message = "Enter the number of vertices: ";
    send(fd, message.c_str(), message.size(), 0);

    int vertices = 0;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    read(fd, buffer, sizeof(buffer)); // Read number of vertices from client
    try{
        vertices = std::stoi(buffer);
        memset(buffer, 0, sizeof(buffer));
        if(vertices <= 0){  
            throw std::invalid_argument("Invalid number of vertices");
        }

        if(graph) graph.reset();  // Reset the graph
        graph = std::make_unique<Graph>(vertices);  // Create a new graph
        
        // Send acknowledgment to client
        message = "Graph created successfully\n";
        send(fd, message.c_str(), message.size(), 0);
    }
    catch(std::invalid_argument& e)
    {
        message = "Invalid number of vertices, Exiting Graph Creation.\n";
        send(fd, message.c_str(), message.size(), 0);
        memset(buffer, 0, sizeof(buffer));
    }
    stages[1]->previousTaskFinished = true;
}

void Pipeline::modifyGraph(int fd)
{
    std::string message;
    if (!stages[1]->previousTaskFinished) 
    {
        message = "Please complete the previous task first\n";
        send(fd, message.c_str(), message.size(), 0);
        return;
    }
    std::string message = "Do you want to add or remove edges? (addedge/removeedge): ";
    send(fd, message.c_str(), message.size(), 0);
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    read(fd, buffer, sizeof(buffer));
    if (strcmp(buffer, "addedge") == 0)
    {
        int u = 0, v = 0, w = 0;
        message = "Enter the edges in the format u v w: ";
        send(fd, message.c_str(), message.size(), 0);
        sscanf(buffer, "%d %d %d", &u, &v, &w);
        graph->addEdge(u, v, w);
        message = "Edge added successfully\n";
        send(fd, message.c_str(), message.size(), 0);
    }
    else if (strcmp(buffer, "removeedge") == 0)
    {
        int u = 0, v = 0;
        message = "Enter the edges in the format u v: ";
        send(fd, message.c_str(), message.size(), 0);
        sscanf(buffer, "%d %d", &u, &v);
        graph->removeEdge(u, v);
        message = "Edge removed successfully\n";
        send(fd, message.c_str(), message.size(), 0);
    }
    stages[2]->previousTaskFinished = true;
}

void Pipeline::createMST(int fd)
{
    std::string message;
    if (!stages[2]->previousTaskFinished)
    {
        message = "Please complete the previous task first\n";
        send(fd, message.c_str(), message.size(), 0);
        return;
    }
    char buffer[1024];
    message = "Choose the algorithm to create MST (prim, kruskal, boruvka): "; 
    send(fd, message.c_str(), message.size(), 0);
    read(fd, buffer, sizeof(buffer));
    mst = std::make_unique<MST>(this->graph, buffer);
    message = "MST created successfully\n";
    send(fd, message.c_str(), message.size(), 0);
    stages[3]->previousTaskFinished = true;
}

void Pipeline::statisticsMST(int fd)
{
    std::string message;
    if (!stages[3]->previousTaskFinished)
    {
        message = "Please complete the previous task first\n";
        send(fd, message.c_str(), message.size(), 0);
        return;
    }
    message = "The MST total weight is: " + std::to_string(mst->getTotalWeight()) + "\n";
    send(fd, message.c_str(), message.size(), 0);
    message = "The MST average distance is: " + std::to_string(mst->getAverageDistance()) + "\n";
    send(fd, message.c_str(), message.size(), 0);
    message = "The MST longest distance is: " + std::to_string(mst->getLongestDistance()) + "\n";
    send(fd, message.c_str(), message.size(), 0);
    message = "The MST shortest distance is: " + std::to_string(mst->getShortestDistance()) + "\n";
    send(fd, message.c_str(), message.size(), 0);
    message = "The MST is: " + mst->printGraph() + "\n";
    send(fd, message.c_str(), message.size(), 0);
}