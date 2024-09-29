#include <iostream>
#include "MST.hpp"

class MSTAlgorithmFactory 
{
public:
    MST MSTFactory(Graph &graph, std::string algorithm);

private:
    MST prim(Graph &graph);
    MST kruskal(Graph &graph);
    MST boruvka(Graph &graph);

};