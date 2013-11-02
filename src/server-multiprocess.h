#include <iostream>

#include "base-server.h"

using namespace std;

class MultiProcessServer: public BaseServer
{
public:
    MultiProcessServer();
    ~MultiProcessServer();

protected:

private:
    int __handle_accept();
    int __handle_an_accept();
    void __log(const char* msg);

};