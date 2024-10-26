#pragma once

#include <iostream>
#include "Graph.hpp"
#include "UnionFind.hpp"

/**
 * @class MSTAlgorithmFactory
 * @brief Factory class for generating Minimum Spanning Tree (MST) using different algorithms.
 *
 * This class provides static methods to generate MST using various algorithms such as Prim's, Kruskal's, and Boruvka's.
 */
class MSTAlgorithmFactory
{
public:
    /**
     * @brief Generates the Minimum Spanning Tree (MST) for the given graph using the specified algorithm.
     *
     * @param graph The graph for which the MST is to be generated.
     * @param algorithm The algorithm to be used for generating the MST. Supported values are "prim", "kruskal", and "boruvka".
     * @return The graph representing the MST.
     */
    static Graph MSTFactory(Graph &graph, std::string algorithm);

private:
    /**
     * @brief Generates the Minimum Spanning Tree (MST) for the given graph using Prim's algorithm.
     *
     * @param graph The graph for which the MST is to be generated.
     * @return The graph representing the MST.
     */
    static Graph prim(Graph &graph);

    /**
     * @brief Generates the Minimum Spanning Tree (MST) for the given graph using Kruskal's algorithm.
     *
     * @param graph The graph for which the MST is to be generated.
     * @return The graph representing the MST.
     */
    static Graph kruskal(Graph &graph);

    /**
     * @brief Generates the Minimum Spanning Tree (MST) for the given graph using Boruvka's algorithm.
     *
     * @param graph The graph for which the MST is to be generated.
     * @return The graph representing the MST.
     */
    static Graph boruvka(Graph &graph);

    /**
     * @brief Finds the vertex with the minimum key value that is not yet included in the MST.
     *
     * @param key Array of key values used to pick the minimum weight edge in the cut.
     * @param mstSet Array to represent the set of vertices included in the MST.
     * @param V Number of vertices in the graph.
     * @return The index of the vertex with the minimum key value.
     */
    static int minKey(int key[], bool mstSet[], int V);
};