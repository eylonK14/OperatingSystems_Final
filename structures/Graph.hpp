#pragma once

#include <iostream>
#include <sstream>


/**
 * @class Graph
 * @brief A class representing a graph using an adjacency matrix.
 *
 * This class provides functionalities to create a graph, add and remove edges,
 * and print the graph.
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
     * @brief Default constructor for the Graph class.
     */
    Graph();

    /**
     * @brief Copy constructor for the Graph class.
     *
     * @param other The graph to copy.
     */
    Graph(const Graph &other);

    /**
     * @brief Destructor for the Graph class.
     */
    ~Graph();

    /**
     * @brief Gets the adjacency matrix of the graph.
     *
     * @return A pointer to the adjacency matrix.
     */
    int** getAdjMatrix() const;

    /**
     * @brief Gets the number of vertices in the graph.
     *
     * @return The number of vertices.
     */
    int getV() const;

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
    std::string printGraph();

private:
    int V;
    int** matrix;
};
