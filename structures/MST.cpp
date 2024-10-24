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

int MST::getAverageDistance() const
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

    return static_cast<int>(totalWeight) / edgeCount;
}

int MST::getShortestDistance() const
{
    int dis = INT_MAX;
    for (int i = 0; i < getV(); i++)
    {
        for (int j = i + 1; j < getV(); j++)
        {
            if (getAdjMatrix()[i][j] != 0)
            {
                int distance = shortestDistance(i, j);
                if (distance != -1 && distance < dis)
                {
                    dis = distance;
                }
            }
        }
    }

    return dis;
}

std::pair<int, int> MST::bfs(int start) const
{
    std::vector<int> dist(getV(), INT_MAX);
    std::queue<int> q;
    q.push(start);
    dist[start] = 0;

    int furthestNode = start;
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

                if (dist[i] > dist[furthestNode])
                {
                    furthestNode = i;
                }
            }
        }
    }

    return {furthestNode, dist[furthestNode]};
}

int MST::shortestDistance(int u, int v) const
{
    std::vector<bool> visited(getV(), false);
    int totalWeight = 0;

    if (dfs(u, v, visited, totalWeight))
    {
        return totalWeight;
    }
    else
    {
        return -1;
    }
}

bool MST::dfs(int u, int v, std::vector<bool> &visited, int &totalWeight) const
{
    if (u == v)
        return true;

    visited[u] = true;

    for (int i = 0; i < getV(); i++)
    {
        if (getAdjMatrix()[u][i] != 0 && !visited[i])
        {
            int weight = getAdjMatrix()[u][i];

            totalWeight += weight;

            if (dfs(i, v, visited, totalWeight))
                return true;

            totalWeight -= weight;
        }
    }

    return false;
}