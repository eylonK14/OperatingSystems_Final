#include <iostream>
#include "Graph.hpp"

class MST : public Graph 
{
public:
    MST(Graph &g);
    ~MST();

    int getTotalWeight();
    int getLongestDistance();
    int getAverageDistance();
    int getShortestDistance();
};


 