#include "ExamplePipelineStage.hpp"
#include <sstream>

ExamplePipelineStage::ExamplePipelineStage() : activeObject() {}

std::string printMenu()
{
    std::stringstream ss;
    ss << "Menu:\n";
    ss << " 1. Create New Graph           - `newgraph v, e`\n";
    ss << " 2. Add Edge                   - `addedge i, j, w`\n";
    ss << " 3. Remove Edge                - `removeedge i, j`\n";
    ss << " 4. Compute MST                - `boruvka`/`kruskal`/`prim`\n";
    ss << " 5. Get Longest Path           - `longestpath`\n";
    ss << " 6. Get Shortest Path          - `shortestpath\n";
    ss << " 7. Get Average Path           - `avgpath`\n";
    ss << " 8. Get Total Path             - `totalweight`\n";
    ss << " 9. Print MST                  - `print`\n";
    ss << "10. Exit                       - `exit`\n";
    ss << "11. Help (show this help text) - `help`\n";
    return ss.str();
}

void ExamplePipelineStage::addEdge(std::vector<std::string> &tokens)
{
    std::istringstream uv_stream(tokens[1]);
    std::string u_str, v_str, w_str;
    if (std::getline(uv_stream, u_str, ',') && std::getline(uv_stream, v_str, ',') && std::getline(uv_stream, w_str))
    {
        try
        {
            int u = std::stoi(u_str) - 1;
            int v = std::stoi(v_str) - 1;
            int w = std::stoi(w_str);
            this->graph->addEdge(u, v, w);
        }
        catch (const std::invalid_argument &)
        {
            std::cerr << "Invalid arguments for addedge.\n";
        }
    }
}

std::string ExamplePipelineStage::parse(std::string input)
{
    std::string result;

    // Remove trailing newline character if present
    if (!input.empty() && input.back() == '\n')
        input.pop_back();

    // Use a string stream to tokenize the input
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }

    if (tokens.empty())
    {
        // Empty input, do nothing
        return "Invalid Command";
    }

    const std::string& command = tokens[0];

    if(tokens.size() > 1)
    {
        if (command == "newgraph" || this->newGraphFlag)
        {
            if (this->newGraphFlag)
            {
                // insert edges
                addEdge(tokens);
                this->counter--;
                if (this->counter == 0)
                {
                    this->newGraphFlag = false;
                }
            }
            else
            {
                std::istringstream nm_stream(tokens[1]);
                std::string n_str, m_str;
                if (std::getline(nm_stream, n_str, ',') && std::getline(nm_stream, m_str))
                {
                    try
                    {
                        int n = std::stoi(n_str);
                        int m = std::stoi(m_str);
                        this->counter = m;
                        Graph my_graph(n);
                        this->graph = new Graph(my_graph);

                        newGraphFlag = true;
                    }
                    catch (const std::invalid_argument&)
                    {
                        std::cerr << "Invalid arguments for newgraph.\n";
                    }
                }
            }
        }
        else if (command == "addedge")
        {
            this->addEdge(tokens);

            result = "Edge added successfully";
        }
        else if (command == "removeedge")
        {
            std::istringstream uv_stream(tokens[1]);
            std::string u_str, v_str;
            if (std::getline(uv_stream, u_str, ',') && std::getline(uv_stream, v_str))
            {
                try
                {
                    int u = std::stoi(u_str) - 1;
                    int v = std::stoi(v_str) - 1;
                    this->graph->removeEdge(u, v);
                }
                catch (const std::invalid_argument&)
                {
                    std::cerr << "Invalid arguments for removeedge.\n";
                }
            }

        result = "Edge removed successfully";
        }
    }
    else if (command == "kruskal" || command == "prim" || command == "boruvka")
    {
        this->mst = new MST(*this->graph, command);
        result = "Created MST using " + command + " algorithm";
    }
    else if (command == "longestpath")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Longest path of the MST: " + mst->getLongestDistance();
    }
    else if (command == "shortestpath")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Shortest path of the MST: " + mst->getShortestDistance();
    }
    else if (command == "avgpath")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Average path of the MST: " + mst->getAverageDistance();
    }
    else if (command == "totalweight")
    {
        if (this-> mst == nullptr)
            result = "Must create MST first!";
        else
            result = "Total Weight of the MST: " + mst->getTotalWeight();
    }
    else if (command == "exit")
    {
        exit(EXIT_SUCCESS);
    }
    else if (command == "help")
    {
        result = printMenu();
    }
    else
    {
        // Unrecognized command
        std::cerr << "Unrecognized command.\n";
    }

    return result;
}

void ExamplePipelineStage::process(const std::string& input, std::function<void(const std::string&)> outputCallback) {
    activeObject.enqueue([input, outputCallback, this]() 
    {
        std::string output = parse(input);
        outputCallback(output);
    });
}