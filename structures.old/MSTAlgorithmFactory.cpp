#include "MSTAlgorithmFactory.hpp"

Graph MSTAlgorithmFactory::MSTFactory(Graph &graph, std::string algorithm)
{
    if (algorithm == "kruskal")
    {
        Graph mst = kruskal(graph);
        return mst;
    }
    else if (algorithm == "prim")
    {
        Graph mst = prim(graph);
        return mst;
    }
    else if (algorithm == "boruvka")
    {
        Graph mst = boruvka(graph);
        return mst;
    }
    return graph;
}

Graph MSTAlgorithmFactory::prim(Graph &graph)
{
    int V = graph.getV();
    int **adjMatrix = new int *[V];
    for (int i = 0; i < V; i++)
        adjMatrix[i] = new int[V];

    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            adjMatrix[i][j] = graph.getAdjMatrix()[i][j];

    // Array to store constructed MST
    int parent[V];
    // Key values used to pick minimum weight edge in cut
    int key[V];
    // To represent set of vertices included in MST
    bool mstSet[V];

    // Initialize all keys as INFINITE
    for (int i = 0; i < V; i++)
        key[i] = INT_MAX, mstSet[i] = false;

    // Always include first 1st vertex in MST.
    // Make key 0 so that this vertex is picked as first
    // vertex.
    key[0] = 0;

    // First node is always root of MST
    parent[0] = -1;

    // The MST will have V vertices
    for (int count = 0; count < V - 1; count++)
    {
        // Pick the minimum key vertex from the
        // set of vertices not yet included in MST
        int u = minKey(key, mstSet, V);

        // Add the picked vertex to the MST Set
        mstSet[u] = true;

        // Update key value and parent index of
        // the adjacent vertices of the picked vertex.
        // Consider only those vertices which are not
        // yet included in MST
        for (int v = 0; v < V; v++)
            // graph[u][v] is non zero only for adjacent
            // vertices of m mstSet[v] is false for vertices
            // not yet included in MST Update the key only
            // if graph[u][v] is smaller than key[v]
            if (adjMatrix[u][v] && mstSet[v] == false && adjMatrix[u][v] < key[v])
                parent[v] = u, key[v] = adjMatrix[u][v];
    }

    // Create the MST graph
    Graph mstGraph(V);
    for (int i = 1; i < V; i++)
    {
        mstGraph.addEdge(parent[i], i, adjMatrix[i][parent[i]]);
    }

    return mstGraph;
}

Graph MSTAlgorithmFactory::kruskal(Graph &graph)
{
    // Get all edges from the adjacency matrix
    std::vector<std::vector<int>> edgeList;
    int V = graph.getV();
    int **adjMatrix = new int *[V];
    for (int i = 0; i < V; i++)
        adjMatrix[i] = new int[V];

    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            adjMatrix[i][j] = graph.getAdjMatrix()[i][j];

    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++) // To avoid duplicates, use only upper triangle of matrix
            if (adjMatrix[i][j] != 0)
                edgeList.push_back({adjMatrix[i][j], i, j});

    // Sort all edges by their weight
    sort(edgeList.begin(), edgeList.end());

    // Initialize the DSU
    UnionFind s(V);

    // Create a new adjacency matrix to store the MST
    Graph mst(V);

    for (auto edge : edgeList)
    {
        int w = edge[0];
        int x = edge[1];
        int y = edge[2];

        // Take this edge in MST if it does not form a cycle
        if (s.find(x) != s.find(y))
        {
            s.unite(x, y);
            mst.addEdge(x, y, w);
        }
    }
    return mst; // Return the MST adjacency matrix
}

Graph MSTAlgorithmFactory::boruvka(Graph &graph)
{
    int V = graph.getV();
    int **adj = new int *[V];
    for (int i = 0; i < V; i++)
        adj[i] = new int[V];

    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            adj[i][j] = graph.getAdjMatrix()[i][j];

    // Create a new graph to represent the MST
    Graph mst(V);

    UnionFind s(V);

    int numTrees = V;
    int MSTweight = 0;

    // Keep combining components (or sets) until there is only one tree
    while (numTrees > 1)
    {
        // Array to store the cheapest edge for each component
        std::vector<std::vector<int>> cheapest(V, std::vector<int>(3, -1)); // [u, v, weight]

        // Traverse all edges to find the cheapest edge for each component
        for (int u = 0; u < V; u++)
        {
            for (int v = 0; v < V; v++)
            {
                if (adj[u][v] != 0)
                { // There is an edge between u and v
                    int set1 = s.find(u);
                    int set2 = s.find(v);

                    if (set1 != set2)
                    {
                        // If it's the first edge for this component or a cheaper one
                        if (cheapest[set1][2] == -1 || cheapest[set1][2] > adj[u][v])
                        {
                            cheapest[set1] = {u, v, adj[u][v]};
                        }
                        if (cheapest[set2][2] == -1 || cheapest[set2][2] > adj[u][v])
                        {
                            cheapest[set2] = {u, v, adj[u][v]};
                        }
                    }
                }
            }
        }

        // Consider the cheapest edges and add them to the MST
        for (int node = 0; node < V; node++)
        {
            if (cheapest[node][2] != -1)
            { // If there is a cheapest edge
                int u = cheapest[node][0];
                int v = cheapest[node][1];
                int weight = cheapest[node][2];

                int set1 = s.find(u);
                int set2 = s.find(v);

                if (set1 != set2)
                {
                    // Include this edge in MST
                    mst.addEdge(u, v, weight);
                    MSTweight += weight;

                    // Perform union of the two components
                    s.unite(set1, set2);
                    numTrees--;
                }
            }
        }
    }

    return mst;
}

int MSTAlgorithmFactory::minKey(int key[], bool mstSet[], int V)
{
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++)
        if (mstSet[v] == false && key[v] < min)
            min = key[v], min_index = v;

    return min_index;
}
