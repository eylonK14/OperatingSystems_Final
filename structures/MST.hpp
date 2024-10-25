#pragma once

#include <iostream>
#include "MSTAlgorithmFactory.hpp"

/**
 * @class MST
 * @brief A class representing a Minimum Spanning Tree (MST) derived from a given graph.
 *
 * This class provides functionalities to compute and analyze the MST of a graph * @brief A class representing a Minimum Spanning Tree (MST) derived from a Graph.
 *
 * This class provides functionalities to compute and analyze the properties of a Minimum Spanning Tree
 * using different algorithms.
 *
 * @note This class inherits from the Graph class.
 */

class MST : public Graph
{
public:
    /**
     * @brief Empty constructor for the MST class.
     *
     * This constructor initializes an empty MST object.
     */
    MST();

    /**
     * @brief Constructs a Minimum Spanning Tree (MST) using the specified algorithm.
     *
     * @param g The graph from which the MST is to be constructed.
     * @param algorithm The algorithm to be used for constructing the MST.
     */
    MST(Graph &g, std::string algorithm) : Graph(MSTAlgorithmFactory::MSTFactory(g, algorithm)) {};

    /**
     * @brief Destructor for the MST class.
     */
    ~MST();

    /**
     * @brief Gets the total weight of the MST.
     *
     * @return The total weight of the MST.
     */
    int getTotalWeight() const;

    /**
     * @brief Gets the longest distance in the MST.
     *
     * @return The longest distance in the MST.
     */
    int getLongestDistance() const;

    /**
     * @brief Gets the average distance of the edges in the MST.
     *
     * @return The average distance of the edges in the MST.
     */
    int getAverageDistance() const;

    /**
     * @brief Gets the shortest distance in the MST.
     *
     * @return The shortest distance in the MST.
     */
    int getShortestDistance() const;

private:
    /**
     * @brief Performs a breadth-first search (BFS) starting from a given node.
     *
     * @param start The starting node for the BFS.
     * @return A pair containing the farthest node and its distance from the start node.
     */
    std::pair<int, int> bfs(int start) const;

    /**
     * @brief Gets the shortest distance between two nodes in the MST.
     *
     * @param u The starting node.
     * @param v The ending node.
     * @return The shortest distance between the two nodes.
     */
    int shortestDistance(int u, int v) const;

    /**
     * @brief Performs a depth-first search (DFS) to determine if there is a path between two nodes.
     *
     * @param u The starting node.
     * @param v The ending node.
     * @param visited A vector to keep track of visited nodes.
     * @param distance The distance between the nodes if a path exists.
     * @return True if a path exists, false otherwise.
     */
    bool dfs(int u, int v, std::vector<bool> &visited, int &distance) const;
};