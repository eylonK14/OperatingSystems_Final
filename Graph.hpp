#include <iostream>

/**
 * @class Graph
 * @brief A class to represent a graph using an adjacency matrix.
 *
 * This class provides methods to add and remove edges, and to print the graph.
 */
class Graph
{
public:
    /**
     * @brief Constructs a graph with a specified number of vertices.
     *
     * @param V The number of vertices in the graph.
     */
    Graph(int V);

    /**
     * @brief Adds an edge to the graph.
     *
     * @param src The source vertex of the edge.
     * @param dest The destination vertex of the edge.
     * @param weight The weight of the edge.
     */
    void addEdge(int src, int dest, int weight);

    /**
     * @brief Removes an edge from the graph.
     *
     * @param src The source vertex of the edge.
     * @param dest The destination vertex of the edge.
     */
    void removeEdge(int src, int dest);

    /**
     * @brief Prints the adjacency matrix of the graph.
     */
    void printGraph();

private:
    int V;
    int **matrix;
};
