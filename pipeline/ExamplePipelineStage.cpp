#include "ExamplePipelineStage.hpp"
#include <sstream>

ExamplePipelineStage::ExamplePipelineStage() : newGraphFlag(false), activeObject(),  graph(std::make_unique<Graph>()),
                                               mst(std::make_unique<MST>()) {}

ExamplePipelineStage::~ExamplePipelineStage()
{
}

std::string PLprintMenu()
{
    std::stringstream ss;
    ss << "Menu:\n";
    ss << " 1. Create New Graph           - `newgraph v e`\n";
    ss << " 2. Add Edge                   - `addedge i j w`\n";
    ss << " 3. Remove Edge                - `removeedge i j`\n";
    ss << " 4. Compute MST                - `boruvka`/`kruskal`/`prim`\n";
    ss << " 5. Get Longest Path           - `longestpath`\n";
    ss << " 6. Get Shortest Path          - `shortestpath\n";
    ss << " 7. Get Average Path           - `avgpath`\n";
    ss << " 8. Get Total Path             - `totalweight`\n";
    ss << " 9. Print                      - `print`\n";
    ss << "10. Exit                       - `exit`\n";
    ss << "11. Help (show this help text) - `help`\n";
    return ss.str();
}

void ExamplePipelineStage::PLaddedge(std::string uStr, std::string vStr, std::string wStr)
{
    int u = 0, v = 0, w = 0;
    try
    {
        u = std::stoi(uStr) - 1;
        v = std::stoi(vStr) - 1;
        w = std::stoi(wStr);
        this->graph->addEdge(u, v, w);
    }
    catch (const std::invalid_argument &)
    {
        std::cerr << "Invalid arguments for LFaddedge!\n";
    }
}

std::string ExamplePipelineStage::PLparse(std::string input)
{
    std::string result = "";
    int u = 0, v = 0, n = 0, m = 0;

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
        return "Invalid Command!\n";
    }

    const std::string &command = tokens[0];

    if (tokens.size() > 1)
    {
        if (command == "newgraph" || this->newGraphFlag)
        {
            if (this->newGraphFlag)
            {
                PLaddedge(tokens[0], tokens[1], tokens[2]);
                this->counter--;
                if (this->counter == 0)
                    this->newGraphFlag = false;
            }
            else
            {
                try
                {
                    n = std::stoi(tokens[1]);
                    m = std::stoi(tokens[2]);
                    this->counter = m;
                    this->graph = std::make_unique<Graph>(n);

                    newGraphFlag = true;
                }
                catch (const std::invalid_argument &)
                {
                    std::cerr << "Invalid arguments for newgraph!\n";
                }
            }
        }
        else if (command == "addedge")
        {
            PLaddedge(tokens[1], tokens[2], tokens[3]);

            result = "Edge added successfully.\n";
        }
        else if (command == "removeedge")
        {
            try
            {
                u = std::stoi(tokens[1]) - 1;
                v = std::stoi(tokens[2]) - 1;
                this->graph->removeEdge(u, v);
            }
            catch (const std::invalid_argument &)
            {
                std::cerr << "Invalid arguments for removeedge!\n";
            }

            result = "Edge removed successfully.\n";
        }
    }
    else if (command == "kruskal" || command == "prim" || command == "boruvka")
    {
        if (this->graph == nullptr)
            result = "Must create graph first!\n";
        else
        {
            this->mst = std::make_unique<MST>(*this->graph, command);
            result = "Created MST using " + command + " algorithm.\n";
        }
    }
    else if (command == "longestpath")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Longest path of the MST: " + std::to_string(mst->getLongestDistance()) + ".\n";
    }
    else if (command == "shortestpath")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Shortest path of the MST: " + std::to_string(mst->getShortestDistance()) + ".\n";
    }
    else if (command == "avgpath")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Average path of the MST: " + std::to_string(mst->getAverageDistance()) + ".\n";
    }
    else if (command == "totalweight")
    {
        if (this->mst == nullptr)
            result = "Must create MST first!\n";
        else
            result = "Total Weight of the MST: " + std::to_string(mst->getTotalWeight()) + ".\n";
    }
    else if (command == "print")
    {
        if (this->graph != nullptr)
        {
            result = "Graph:\n";
            result += this->graph->printGraph();
        }
        if (this->mst != nullptr)
        {
            result += "MST:\n";
            result += this->mst->printGraph();
        }
    }
    else if (command == "exit")
        exit(EXIT_SUCCESS);
    else if (command == "help")
        result = PLprintMenu();
    else
    {
        std::cerr << "Unrecognized command.\n";
        result = "Unrecognized command.\n";
    }

    return result;
}

// void ExamplePipelineStage::process(const std::string &input, std::function<void(const std::string &)> outputCallback)
// {
//     // Wrap outputCallback in a shared_ptr
//     auto outputCallbackPtr = std::make_shared<std::function<void(const std::string &)>>(outputCallback);

//     activeObject.enqueue([input, this, outputCallbackPtr]()
//                          {
//         std::string output = PLparse(input);
//         try
//         {
//             (*outputCallbackPtr)(output);
//         }
//         catch (const std::exception &e)
//         {
//             std::cerr << "Exception in ExamplePipelineStage: " << e.what() << std::endl;
//         } });
// }

void ExamplePipelineStage::process(const std::string& input, std::function<void(const std::string&)> outputCallback) {
    activeObject.enqueue([input, outputCallback, this]() 
    {
        std::string output = PLparse(input);
        outputCallback(output);
    });
}