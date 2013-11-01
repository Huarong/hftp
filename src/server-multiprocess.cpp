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
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    cout << _ctr_sockfd << endl;
    int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (connect_fd == -1) {
        perror("accept");
        return -1;
    }
    cout << "Server: client " << inet_ntoa(client_addr.sin_addr) << " connect" << endl;
    // create a session
    Session session = Session(connect_fd, _localhost, _root_path);
    session.__write_response("220 FTP server from hhr ready.\r\n");

    while (true) {
        if (session.__read_request(__rev_buf, MSG_MAX_LEN_SHORT) == -1) {
            cout << "ERROR: read request from client" << endl;
            return -1;
        }
        const char* cmd = __rev_buf;
        int cmd_id = _identify_cmd(cmd);
        cout << "COMMAND:" << cmd << endl;
        switch (cmd_id) {
            case CMD_USER:
                session.__handle_cmd_user(cmd);
                break;
            case CMD_PASS:
                session.__handle_cmd_pass(cmd, _account);
                break;
            case CMD_PWD:
                session.__handle_cmd_pwd(cmd);
                break;
            case CMD_GET:
                session.__handle_cmd_get(cmd, _root_path);
                break;
            case CMD_PUT:
                session.__handle_cmd_put(cmd);
                break;
            case CMD_PASV:
                session.__handle_cmd_pasv(cmd);
                break;
            case CMD_RETR:
                session.__handle_cmd_retr(cmd);
                break;
            case CMD_STOR:
                session.__handle_cmd_stor(cmd);
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