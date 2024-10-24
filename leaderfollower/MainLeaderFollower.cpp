#include <iostream>
#include "ServerLeaderFollower.hpp"

int main(/* int argv, char* argc[] */)
{
    Server srv(12345, 10);
    srv.run();

    return 0;
}