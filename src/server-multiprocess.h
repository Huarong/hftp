#include <iostream>

#include "base-server.h"

using namespace std;

class MultiProcessServer: public BaseServer
{
public:
    MultiProcessServer();
    ~MultiProcessServer();

private:
    void __handle_accept();
    void __log(const char* msg);
};