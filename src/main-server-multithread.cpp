#include <iostream>

#include "server-multithread.h"

using namespace std;

int main(int argc, char const *argv[]) {
    MultiThreadServer server = MultiThreadServer();
    server.run();
    return 0;
}