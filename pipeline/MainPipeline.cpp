#include <iostream>
#include "ServerPipelineActiveObject.hpp"

int main (/* int argv, char* argc[] */)
{
    Server srv(12345);
    srv.run();

    return 0;
}