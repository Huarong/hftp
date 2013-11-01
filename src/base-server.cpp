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
    _create_cmd_reg_map();
    // to edit
    _account["hhr"] = "hhrjyy";
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
    cout << "listening on port "<< _serv_ctr_port << endl;
    __handle_accept();
    return 0;
}

int BaseServer::__handle_accept() {
    cout << "you must rewrite method __handle_accept" << endl;
    return 0;
}

int BaseServer::_read_request(const int sockfd, char* rev_buf, size_t buf_len) {
    int n_bytes = read(sockfd, rev_buf, buf_len -1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    _print_rev_msg(rev_buf);
    __log(rev_buf);
    return 0;
}

int BaseServer::_write_response(const int sockfd, const char* send_buf) {
    ssize_t n_bytes = write(sockfd, send_buf, strlen(send_buf));
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

int BaseServer::_identify_cmd(const char* msg) {
    int status;
    regmatch_t pmath[1];
    regex_t reg;
    int cmd_id =  -1;
    for (map<int, const char*>::iterator it = _cmd_reg_map.begin(); it != _cmd_reg_map.end(); ++it) {
        cmd_id = it -> first;
        const char* pattern = it -> second;
        regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
        status = regexec(&reg, msg, 1, pmath, 0);
        regfree(&reg);
        if (status == 0) {
            cout << "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh" << endl;
            cout << "found:" << cmd_id << endl;
            break;
        }
    }
    if (-1 != cmd_id) {
        cout << "cmd_id:" << cmd_id << endl;
        return cmd_id;
    }
    // command not found
    cout << "command not found" << endl;
    return -1;
}

void BaseServer::_create_cmd_reg_map() {
    const char* pattern;
    // regex_t reg;

    // "user jay"
    pattern = "^USER [a-zA-Z0-9_-]+";
    // regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    _cmd_reg_map[CMD_USER] = pattern;

    // "pass xxx"
    pattern = "^PASS [a-zA-Z0-9_-]+\\r\\n";
    // regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    _cmd_reg_map[CMD_PASS] = pattern;

    // "pwd"
    pattern = "^PWD\\r\\n";
    // regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    _cmd_reg_map[CMD_PWD] = pattern;

    return;
}

int BaseServer::_handle_cmd_user(const char* user) {
    _write_response(_ctr_sockfd, "331 Please specify the password.\r\n");
    return 0;
}

int BaseServer::_handle_cmd_pass(const char* user, const char* pass) {
    // string user_str(user);
    map<string, string>::iterator it = _account.find(string(user));
    if (it == _account.end()) {
        cout << "user not exist" << endl;
        return -1;
    }
    if (string(pass) != it -> second) {
        _write_response(_ctr_sockfd, "530 Login incorrect.\r\n");
        return -1;
    }
    _write_response(_ctr_sockfd, "230 Login successful.\r\n");
    return 0;
}

int BaseServer::_handle_cmd_pwd(const char* cur_path) {
    char msg[MSG_MAX_LEN_SHORT];
    sprintf(msg, "257 \"%s\"", cur_path);
    _write_response(_ctr_sockfd, msg);
    return 0;
}

int BaseServer::_handle_cmd_get(const char* server_file) {
    return 0;
}
int BaseServer::_handle_cmd_put(const char* client_file) {
    return 0;
}

int BaseServer::_parse_config(char* config_path) {
    // FILE* config = fopen(config_path, "r");

    return 0;
}