#include <iostream>
#include "MSTAlgorithmFactory.hpp"

class MST : public Graph
{
public:
    MST(Graph &g, std::string algorithm) : Graph(MSTAlgorithmFactory::MSTFactory(g, algorithm)) {};
    ~MST();

    int getTotalWeight() const;
    int getLongestDistance() const;
    double getAverageDistance() const;
    int getShortestDistance() const;

    std::pair<int, int> bfs(int start) const;
};


/*
 0    3
 v    ^
 1 -> 2

 3 / 3 = 1

 0 -> 1 = 1
 0 -> 2 = 2
 0 -> 3 = 3
 1 -> 2 = 1
 1 -> 3 = 2
 2 -> 3 = 1
 ==========
         10 / 6 = 1.6666666666666667
*/
