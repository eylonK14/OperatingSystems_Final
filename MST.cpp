#include "MST.hpp"

int MST::getTotalWeight() const
{
    int MSTweight = 0;
    for (int i = 0; i < getV(); i++)
        for (int j = i; j < getV(); j++)
            if (getAdjMatrix()[i][j])
                MSTweight += getAdjMatrix()[i][j];

    return MSTweight;
}

int MST::getLongestDistance() const
{
    std::pair<int, int> firstBFS = bfs(0);
    std::pair<int, int> secondBFS = bfs(firstBFS.first);
    return secondBFS.second;
}

double MST::getAverageDistance() const
{
    int totalWeight = 0;
    int edgeCount = 0;

    for (int i = 0; i < getV(); i++)
    {
        for (int j = i + 1; j < getV(); j++)
        {
            if (getAdjMatrix()[i][j] != 0)
            {
                totalWeight += getAdjMatrix()[i][j];
                edgeCount++;
            }
        }
    }

    if (edgeCount == 0)
        return 0.0; // Avoid division by zero

    return static_cast<double>(totalWeight) / edgeCount;
}

std::pair<int, int> MST::bfs(int start) const
{
    std::vector<int> dist(getV(), INT_MAX);
    std::queue<int> q;
    q.push(start);
    dist[start] = 0;

    int farthestNode = start;
    while (!q.empty())
    {
        int node = q.front();
        q.pop();

        for (int i = 0; i < getV(); i++)
        {
            if (getAdjMatrix()[node][i] != 0 && dist[i] == INT_MAX)
            {
                dist[i] = dist[node] + getAdjMatrix()[node][i];
                q.push(i);

                if (dist[i] > dist[farthestNode])
                {
                    farthestNode = i;
                }
            }
        }
    }

    return {farthestNode, dist[farthestNode]};
}
