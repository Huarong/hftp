#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>


#include "base-server.h"
#include "util.h"

using namespace std;

BaseServer::BaseServer() {
    _serv_ctr_port = 21;
    _serv_data_port = 20;
}

BaseServer::~BaseServer() {

}


int BaseServer::run() {
    int r = _init();
    if (r == -1) {
        return -1;
    }
    __handle_accept();
    return 0;
}

// socket, bind, listen
int BaseServer::_init() {
    // create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }
    _ctr_sockfd = sockfd;

    // initialize the sockaddr_in
    bzero(&_serv_ctr_addr,sizeof(_serv_ctr_addr));
    _serv_ctr_addr.sin_family = AF_INET;
    _serv_ctr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _serv_ctr_addr.sin_port = htons(_serv_ctr_port);

    // blind
    // if "bind: Permission denied", use "sudo ./server-multiprocess"
    int r;
    r = bind(sockfd, (struct sockaddr*) &_serv_ctr_addr, sizeof(_serv_ctr_addr));
    if (r == -1) {
        perror("bind");
        return -1;
    }
    cout << "The sockfd is:" << sockfd << endl;

    //listen
    r = listen(sockfd, BACKLOG);
    if (r == -1) {
        perror("listen");
        return -1;
    }
    return 0;
}

void BaseServer::__handle_accept() {
    cout << "you must rewrite method __handle_accept" << endl;
    return;
}

int BaseServer::_read_request() {
    char rev_buf[MSG_MAX_LEN];
    int n_bytes = read(_ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    _print_rev_msg(rev_buf);
    __log(rev_buf);

    return 0;
}

int BaseServer::_write_response(const char* send_buf) {
    ssize_t n_bytes = write(_ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }
    return 0;
}

int BaseServer::_print_rev_msg(const char* msg) {
    string msg_s = string(msg, strlen(msg));
    int len = msg_s.length();
    if (len == 0) {
        return 0;
    }
    cout << msg_s;
    return len;
}