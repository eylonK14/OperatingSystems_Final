#include "MST.hpp"

MST::MST(Graph &graph)
{
    this->V = graph->V;
    this->matrix = new int *[this->V];
    for (int i = 0; i < this->V; i++)
    {
        this->matrix[i] = new int[this->V];
        for (int j = 0; j < this->V; j++)
        {
            this->matrix[i][j] = graph->matrix[i][j];
        }
    }
}

MST::~MST()
{
    for (int i = 0; i < this->V; i++)
    {
        delete[] this->matrix[i];
    }
    delete[] this->matrix;
}

void MST::addEdge(int u, int v, int weight)
{
    this->matrix[u][v] = weight;
    this->matrix[v][u] = weight;
}

void