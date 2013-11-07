#include <iostream>

#include "server-iomultiplexing.h"

using namespace std;

int main(int argc, char const *argv[]) {
    IOMultiplexingServer server = IOMultiplexingServer();
    server.run();
    return 0;
}