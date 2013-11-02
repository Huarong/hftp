#include <iostream>
#include <cstdio>

#include "server-multiprocess.h"
#include "session.h"

using namespace std;

MultiProcessServer::MultiProcessServer(): BaseServer() {
}

MultiProcessServer::~MultiProcessServer() {
}

int MultiProcessServer::__handle_accept() {
    while (true) {
        __handle_an_accept();
    }
    return 0;
}


int MultiProcessServer::__handle_an_accept() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (connect_fd == -1) {
        perror("accept");
        return -1;
    }
    pid_t pid = fork();
    if (0 == pid) {
        close(_ctr_sockfd);
        Session session = _create_session(connect_fd);
        _handle_cmds(session);
        close(connect_fd);
        return 0;
    }
    close(connect_fd);
    return 0;
}

void MultiProcessServer::__log(const char* msg) {
    FILE* pfile;
    pfile = fopen(LOG_PATH_SERVER_MP, "a+");
    fputs(msg, pfile);
    fclose(pfile);
}