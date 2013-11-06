#include <iostream>
#include <queue>
#include <vector>
#include <set>

#include "base-server.h"

using namespace std;


#define MAX_THREAD_NUM 50


class MultiThreadServer: public BaseServer
{
public:
    MultiThreadServer();
    ~MultiThreadServer();

    Session create_session(int connect_fd);
    int handle_cmds(Session& session);


protected:

private:
    int __handle_accept();
    int __handle_an_accept();

    void __log(const char* msg);
    int __create_thread(int connect_fd);
    void __wait_thread();

    pthread_mutex_t __mutex;
    int __thread_count;

    set<pthread_t> __thread_set;

};

struct a_thread_args
{
    int connect_fd;
    pthread_mutex_t* mutex;
    MultiThreadServer* server;
};

void *a_thread(void *);
