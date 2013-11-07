#include <iostream>

#include "server-multiprocess.h"

using namespace std;

int main(int argc, char const *argv[]) {
    MultiProcessServer server = MultiProcessServer();
    server.run();
    return 0;
}