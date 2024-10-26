#include "Graph.hpp"

Graph::Graph()
{
    this->V = 0;
    this->matrix = nullptr;
}

// Constructor
Graph::Graph(int V)
{
    if (V <= 0)
        throw std::invalid_argument("received negative value");

    this->V = V;
    // vector
    this->matrix = new int*[this->V];
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

// Grapg::Graph(Graph&& other) noexcept : V(other.V), matrix(std::move(other.matrix)) {
//     other.V = 0;
// }

// Grapg::Graph& operator=(Graph&& other) noexcept {
//     if (this != &other) {
//         for (int i = 0; i < this->V; i++)
//             delete[] this->matrix[i];
//         delete[] this->matrix;

//         V = other.V;
//         matrix = std::move(other.matrix);
//         other.V = 0;
//     }
//     return *this;
// }

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
    if (u < 0 || u >= V || v < 0 || v >= V)
        throw std::out_of_range("Vertex index out of range");
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
    std::ostringstream oss;
    for (int i = 0; i < this->V; i++) {
        for (int j = 0; j < this->V; j++) {
            if (this->matrix[i][j] != 0) {
                oss << "(" << i << ", " << j << ") = " << this->matrix[i][j] << "\n";
            }
        }
    }
    return oss.str();
}
