#include <iostream>
#include <cstdio>
#include <cstring>

#include <sys/ioctl.h>
#include <poll.h>

#include "server-iomultiplexing.h"
#include "session.h"
#include "util.h"

using namespace std;

IOMultiplexingServer::IOMultiplexingServer(): BaseServer() {
    _server_type = SERVER_TYPE_IO;
}

IOMultiplexingServer::~IOMultiplexingServer() {
}

int IOMultiplexingServer::__handle_accept() {
    int on = 1;
    int r = ioctl(_ctr_sockfd, FIONBIO, (char *)&on);
    if (r < 0) {
        perror("ioctl() failed");
        close(_ctr_sockfd);
        return -1;
    }

    memset(__fds, 0, sizeof(__fds));

    for (int i = 1; i < MAX_POLL_NUM; ++i)
    {
        __fds[i].fd = -1;
    }

    __fds[0].fd = _ctr_sockfd;
    __fds[0].events = POLLIN;
    __nfds = 1;

    while (true) {
        __handle_an_request();
    }
    return 0;
}


int IOMultiplexingServer::__handle_an_request() {
    cout << "poll() is waiting..." << endl;
    int nready= poll(__fds, __nfds, POLL_TIME_OUT);
    if (nready < 0) {
        perror("poll");
        return -1;
    } else if (nready == 0) {
        cout << "poll TIME OUT" << endl;
        return -1;
    }

    // find a place in __fds to store connect_fd.
    if (__nfds >= MAX_POLL_NUM) {
        cout << "ERROR: too many connections to the server!" << endl;
        return -1;
    }

    if (__fds[0].revents == POLLIN) {

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
        __fds[0].revents = 0;
        if (connect_fd == -1) {
            perror("accept");
            return -1;
        }

        for (int i = 1; i < MAX_POLL_NUM; ++i)
        {
            if (__fds[i].fd < 0) {
                __fds[i].fd = connect_fd;
                __fds[i].events = POLLIN;
                __nfds++;
                cout << "begin to create a session" << endl;
                Session s =  _create_session(connect_fd);
                __session_array[i] = s;
                break;
            }
        }

    }

    for (int i = 1; i < MAX_POLL_NUM; ++i)
    {
        if (__fds[i].fd < 0) {
            continue;
        }
        if (__fds[i].revents & POLLIN) {
            __handle_cmds(i);
            __fds[i].revents = 0;

        }
    }
    return 0;
}

void IOMultiplexingServer::__log(const char* msg) {
    FILE* pfile;
    pfile = fopen(LOG_PATH_SERVER_IO, "a+");
    fputs(msg, pfile);
    fclose(pfile);
}

int IOMultiplexingServer::__handle_cmds(size_t fd_i) {
    Session* session = &__session_array[fd_i];
    char rev_buf[MSG_MAX_LEN_SHORT];
            if (session->__read_request(rev_buf, MSG_MAX_LEN_SHORT) == -1) {
                cout << "ERROR: read request from client" << endl;
                return -1;
            }
            const char* cmd = rev_buf;
            int cmd_id = _identify_cmd(cmd);
            cout << "COMMAND:" << cmd << endl;
            switch (cmd_id) {
                case CMD_USER:
                    session->__handle_cmd_user(cmd);
                    break;
                case CMD_PASS:
                    session->__handle_cmd_pass(cmd, _account);
                    break;
                case CMD_PWD:
                    session->__handle_cmd_pwd(cmd);
                    break;
                case CMD_GET:
                    session->__handle_cmd_get(cmd, _root_path);
                    break;
                case CMD_PUT:
                    session->__handle_cmd_put(cmd);
                    break;
                case CMD_PASV:
                    session->__handle_cmd_pasv(cmd);
                    break;
                case CMD_RETR:
                    session->__handle_cmd_retr(cmd);
                    break;
                case CMD_STOR:
                    session->__handle_cmd_stor(cmd);
                    break;
                case CMD_QUIT:
                    session->__handle_cmd_quit(cmd);
                    break;
                default:
                    cout << "Invalid command" << endl;
                    break;

    }
    return 0;
}