#include <iostream>
#include "Graph.hpp"
#include "UnionFind.hpp"

class MSTAlgorithmFactory
{
public:
    static Graph MSTFactory(Graph &graph, std::string algorithm);

private:
    static Graph prim(Graph &graph);
    static Graph kruskal(Graph &graph);
    static Graph boruvka(Graph &graph);

    static int minKey(int key[], bool mstSet[], int V);
};