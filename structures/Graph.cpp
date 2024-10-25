#include "Graph.hpp"

// Constructor
Graph::Graph(int V)
{
    if (V <= 0)
        throw std::invalid_argument("received negative value");

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
            this->matrix[i][j] = other.getAdjMatrix()[i][j];
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
std::string Graph::printGraph()
{
    std::string graph = "";

    for (int i = 0; i < this->V; i++)
    {
        for (int j = 0; j < this->V; j++)
            if (this->matrix[i][j] != 0)
                graph += "(" + std::to_string(i) + ", " + std::to_string(j) + ") = " + std::to_string(this->matrix[i][j]) + "\n";
    }

    return graph;
}

// // Function to get the transpose of the graph
// Graph *transposeGraph(Graph *graph)
// {
// 	Graph *transposed = createGraph(graph->V);
// 	for (int v = 0; v < graph->V; v++)
// 	{
// #ifdef ADJ_MATRIX
// 		for (int i = 0; i < graph->V; i++)
// 		{
// 			if (graph->G[v][i])
// 				PLaddedge(transposed, i, v);
// 		}
// #endif // ADJ_MATRIX

// #ifdef ADJ_LIST
// 		AdjListNode *adjNode = graph->array[v].head;
// 		while (adjNode != NULL)
// 		{
// 			int neighbor = adjNode->dest;
// 			PLaddedge(transposed, neighbor, v);
// 			adjNode = adjNode->next;
// 		}
// #endif // ADJ_LIST
// 	}
// 	return transposed;
// }
