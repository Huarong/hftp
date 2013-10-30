#include <iostream>

#include "server-multiprocess.h"

using namespace std;

int main(int argc, char const *argv[]) {
    MultiProcessServer server = MultiProcessServer();
    // MultiProcessServer server = new MultiProcessServer;
    cout << "hello" << endl;
    server.run();
    return 0;
}