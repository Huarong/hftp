#include <iostream>
#include <cstdio>
#include <cstring>

#include <pthread.h>

#include "server-multithread.h"
#include "session.h"
#include "util.h"

using namespace std;

a_thread_args g_args;

MultiThreadServer::MultiThreadServer(): BaseServer() {
    __thread_count = 0;
}

MultiThreadServer::~MultiThreadServer() {
}

int MultiThreadServer::__handle_accept() {
    pthread_mutex_init(&__mutex, NULL);
    cout << "I am the main thread. I can have multi child threads" << endl;
    while (true) {
        __handle_an_accept();
    }
    cout << "I am the main thread, waiting for child threads to finish" << endl;
    __wait_thread();
    return 0;
}


int MultiThreadServer::__handle_an_accept() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (connect_fd == -1) {
        perror("accept");
        return -1;
    }
    __thread_count++;
    cout << "read to create a thread" << endl;
    int r;
    r = __create_thread(connect_fd);
    if (r == -1) {
        close(connect_fd);
        return -1;
    }
    return 0;
}

int MultiThreadServer::__create_thread(int connect_fd) {
    int temp;
    pthread_t thread;

    g_args.connect_fd = connect_fd;
    g_args.mutex = &__mutex;
    g_args.server = this;

    temp = pthread_create(&thread, NULL, a_thread, NULL);
    if (temp != 0) {
        cout << "ERROR: fail to create a thread." << endl;
        cout << temp << endl;
        return -1;
    }
    return 0;
}

void MultiThreadServer::__wait_thread() {
    for (std::set<pthread_t>::iterator it = __thread_set.begin(); it != __thread_set.end(); ++it) {
        if (*it != 0) {
            pthread_join(*it, NULL);
            cout << "a thread finished" << endl;
            __thread_set.erase(it);
        }
    }
    return;
}

void MultiThreadServer::__log(const char* msg) {
    FILE* pfile;
    pfile = fopen(LOG_PATH_SERVER_MT, "a+");
    fputs(msg, pfile);
    fclose(pfile);
}

Session MultiThreadServer::create_session(int connect_fd) {
    return _create_session(connect_fd);
}

int MultiThreadServer::handle_cmds(Session& session) {
    return _handle_cmds(session);
}

void *a_thread(void *) {
    cout << "begin a thread." << endl;
    pthread_mutex_lock(g_args.mutex);
    Session session = g_args.server->create_session(g_args.connect_fd);
    pthread_mutex_unlock(g_args.mutex);
    g_args.server->handle_cmds(session);
    close(g_args.connect_fd);
    pthread_exit(NULL);
}

