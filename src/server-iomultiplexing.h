#include <iostream>

#include <poll.h>

#include "base-server.h"

using namespace std;

#define MAX_POLL_NUM 5
// set timeout -1, which means waiting forever.
#define POLL_TIME_OUT -1

class IOMultiplexingServer: public BaseServer
{
public:
    IOMultiplexingServer();
    ~IOMultiplexingServer();

protected:

private:
    int __handle_accept();
    int __handle_an_accept();

    void __log(const char* msg);

    struct pollfd __fds[MAX_POLL_NUM];
    size_t __nfds;


};