#include <iostream>
#include <cstdio>

#include "server-multiprocess.h"

using namespace std;

MultiProcessServer::MultiProcessServer(): BaseServer() {
}

MultiProcessServer::~MultiProcessServer() {
}

int MultiProcessServer::__handle_accept() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (connect_fd == -1) {
            perror("accept");
            continue;
        }
        cout << "Server: client " << inet_ntoa(client_addr.sin_addr) << " connect" << endl;
        _write_response(connect_fd, "220 FTP server from hhr ready.\r\n");
        cout << "///////////////////////////////////////////////////////////////////////////////" << endl;
        if (_read_request(connect_fd, __rev_buf, MSG_MAX_LEN_SHORT) == -1) {
            cout << "ERROR: read request from client" << endl;
            return -1;
        }
        cout << "7777777777777777777777777777777777777777777" << endl;
        cout << "88888888888888888888888888888888888888888888888" << endl;
        int cmd_id = _identify_cmd(__rev_buf);
        cout << "11111111111111111111111111111111111111111" << endl;
        // cout << "COMMAND:" << __rev_buf << endl;
        switch (cmd_id) {
            case CMD_USER:
                cout << "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu" << endl;
                // 
                break;
            case CMD_PASS:
                // 
                break;
            case CMD_PWD:
                // 
                break;
            case CMD_GET:
                // 
                break;
            case CMD_PUT:
                // 
                break;
            default:
                cout << "Invalid command" << endl;
                break;
        }
    }
    return 0;
}

void MultiProcessServer::__log(const char* msg) {
    FILE* pfile;
    pfile = fopen(LOG_PATH_SERVER_MP, "a+");
    fputs(msg, pfile);
    fclose(pfile);
}