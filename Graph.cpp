#include "Graph.hpp"

// Constructor
Graph::Graph(int V)
{
    this->V = V;
    this->matrix = new int *[this->V];
    for (int i = 0; i < this->V; i++)
    {
        this->matrix[i] = new int[this->V];
        for (int j = 0; j < this->V; j++)
            this->matrix[i][j] = 0;
    }
}

Graph::Graph(const Graph &other)
{
    this->V = other.V;
    this->matrix = new int *[this->V];
    for (int i = 0; i < this->V; i++)
    {
        this->matrix[i] = new int[this->V];
        for (int j = 0; j < this->V; j++)
            this->matrix[i][j] = other.matrix[i][j];
    }
}

// Destructor
Graph::~Graph()
{
    for (int i = 0; i < this->V; i++)
        delete[] this->matrix[i];
    delete[] this->matrix;
}

int **Graph::getAdjMatrix() const
{
    return this->matrix;
}

int Graph::getV() const
{
    return this->V;
}

// Add an edge to the graph
void Graph::addEdge(int u, int v, int w)
{
    this->matrix[u][v] = w;
    this->matrix[v][u] = w;
}

// remove an edge from the graph
void Graph::removeEdge(int u, int v)
{
    this->matrix[u][v] = 0;
    this->matrix[v][u] = 0;
}

// Print the graph as adjacency list
void Graph::printGraph()
{
    for (int i = 0; i < this->V; i++)
    {
        std::cout << i << " : ";
        for (int j = 0; j < this->V; j++)
            if (this->matrix[i][j] != 0)
                std::cout << j << " ";
        std::cout << std::endl;
    }
}
