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
    void __log(const char* msg);

    char __rev_buf[MSG_MAX_LEN_SHORT];
    char __send_buf[MSG_MAX_LEN];
};