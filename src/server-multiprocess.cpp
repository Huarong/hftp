#include <iostream>
#include <cstdio>

#include "server-multiprocess.h"

using namespace std;

MultiProcessServer::MultiProcessServer(): BaseServer() {
}

MultiProcessServer::~MultiProcessServer() {
}

void MultiProcessServer::__handle_accept() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (connect_fd == -1) {
            perror("accept");
            continue;
        }
        cout << "Server: client " << inet_ntoa(client_addr.sin_addr) << " connect" << endl;

        if (_read_request() == -1) {
            cout << "ERROR: read request from client" << endl;
            return;
        }
        // TODO///////////////////////////////////////////////////////////////////////////////
    }
    return;
}

void MultiProcessServer::__log(const char* msg) {
    FILE* pfile;
    pfile = fopen(LOG_PATH_SERVER_MP, "a+");
    fputs(msg, pfile);
    fclose(pfile);
}