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

// Function to return the shortest distance between u and v in the MST
int shortest_distance(int u, int v)
{
    std::vector<bool> visited(getV(), false); // to track visited vertices
    int totalWeight = 0;

    // Perform DFS to find the path and calculate the weight
    if (dfs(u, v, visited, totalWeight))
    {
        return totalWeight;
    }
    else
    {
        // Return a special value if no path is found (though this shouldn't happen in an MST)
        return -1;
    }
}
// DFS helper function to find the path from u to v
bool dfs(int u, int v, std::vector<bool> &visited, int &totalWeight)
{
    // If we've reached the destination vertex, return true
    if (u == v)
        return true;

    // Mark the current vertex as visited
    visited[u] = true;

    // Explore all adjacent vertices
    for (int i = 0; i < V; i++)
    {
        if (adjMatrix[u][i] != 0 && !visited[i])
        {
            int weight = adjMatrix[u][i];

            // Add the weight of the edge to the totalWeight
            totalWeight += weight;

            // Recursively continue DFS on the next vertex
            if (dfs(i, v, visited, totalWeight))
                return true;

            // If no path found, backtrack and subtract the edge weight
            totalWeight -= weight;
        }
    }

    // If no path is found, return false
    return false;
}