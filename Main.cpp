#include <getopt.h>
#include <iostream>

#include "pipeline/ServerPipelineActiveObject.hpp"
#include "leaderfollower/ServerLeaderFollower.hpp"

void printUsage(char *argv)
{
    std::cout << "Usage: " << argv << " [-h/--help] [--lf <leaderfollower threads>/--pl] [-p/--port <port>]" << std::endl;
}

int main(int argc, char *argv[])
{
    int c;
    bool lf = false;
    bool pipeline = false;
    int leaderFollowerThreads = 0;
    int port = 0;

    if (argc == 1)
    {
        std::cout << "No arguments provided" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    if (argc > 5)
    {
        std::cout << "Too many arguments" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"help", no_argument, 0, 0},
            {"pl", no_argument, 0, 0},
            {"lf", required_argument, 0, 0},
            {"port", required_argument, 0, 0},
            {0, 0, 0, 0}};

        c = getopt_long(argc, argv, "h:p:",
                        long_options, &option_index);
        if (c == -1)
            break;

        if (c == 0)
        {
            std::cout << "option " << long_options[option_index].name << std::endl;
            if (optarg)
                std::cout << "with arg " << optarg << std::endl;
            std::string optionStr = long_options[option_index].name;
            if (optionStr == "pl")
                pipeline = true;
            if (optionStr == "lf")
            {
                lf = true;
                if (optarg)
                {
                    leaderFollowerThreads = std::stoi(optarg);
                }
                else
                {
                    std::cout << "leaderfollower requires a number of threads" << std::endl;
                    return 1;
                }
            }
            if (optionStr == "port")
            {
                if (optarg)
                {
                    port = std::stoi(optarg);
                }
                else
                {
                    std::cout << "port requires a value" << std::endl;
                }
            }
            if (optionStr == "help")
                printUsage(argv[0]);
            std::cout << std::endl;
        }

        else if (c == 'h')
            printUsage(argv[0]);

        else if (c == 'p')
            port = atoi(optarg);

        else
            std::cout << "?? getopt returned character code 0" << c << " ??" << std::endl;
    }

    if (lf && pipeline)
    {
        std::cout << "cannot have both leaderfollower and pipeline" << std::endl;
        return 1;
    }

    std::cout << "port: " << port << std::endl;
    std::cout << "lf: " << lf << ", leaderFollowerThreads: " << leaderFollowerThreads << std::endl;
    std::cout << "pl: " << pipeline << std::endl;

    if (pipeline)
    {
        PLServer server(port);
        server.run();
    }
    else if (lf)
    {
        LFServer server(port, leaderFollowerThreads);
        server.run();
    }

    return 0;
}