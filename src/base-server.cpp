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
#include <netdb.h>


#include "base-server.h"
#include "session.h"
#include "util.h"

using namespace std;

BaseServer::BaseServer() {
    _server_type = 0;
    _serv_ctr_port = 8888;
    _serv_data_port = 8889;
    _create_cmd_reg_map();
    // to edit
    _account["hhr"] = "hhrjyy";
    _root_path = "/home/hhr/";
    _get_localhost();
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
    printf("Server is listening on port %d\n",ntohs(_serv_ctr_addr.sin_port));

    //listen
    r = listen(sockfd, BACKLOG);
    if (r == -1) {
        perror("listen");
        return -1;
    }
    cout << "listening on port "<< _serv_ctr_port << endl;
    cout << _ctr_sockfd << endl;
    return 0;
}

int BaseServer::__handle_accept() {
    cout << "you must rewrite method __handle_accept" << endl;
    return 0;
}

int BaseServer::__handle_an_accept() {
    cout << "you must rewrite method __handle_an_accept" << endl;
    return 0;
}

Session BaseServer::_create_session(int connect_fd) {
        // create a session
    Session session = Session(connect_fd, _localhost, _root_path);
    session.__write_response("220 FTP server from hhr ready.\r\n");
    return session;
}

int BaseServer::_handle_cmds(Session session) {
    char rev_buf[MSG_MAX_LEN_SHORT];
    while (true) {
        if (!session.is_alive()) {
            cout << "client quit" << endl;
            return -1;
        }
        if (session.__read_request(rev_buf, MSG_MAX_LEN_SHORT) == -1) {
            cout << "ERROR: read request from client" << endl;
            return -1;
        }
        const char* cmd = rev_buf;
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
            case CMD_QUIT:
                session.__handle_cmd_quit(cmd);
                break;
            default:
                cout << "Invalid command" << endl;
                break;
        }
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
            cout << "found:" << cmd_id << endl;
            return cmd_id;
        }
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
    pattern = "^PASS [a-zA-Z0-9_-]+";
    // regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    _cmd_reg_map[CMD_PASS] = pattern;

    // "pwd"
    pattern = "^PWD";
    // regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    _cmd_reg_map[CMD_PWD] = pattern;

    pattern = "^PASV";
    _cmd_reg_map[CMD_PASV] = pattern;

    pattern = "^RETR [a-zA-Z0-9/_-]+";
    _cmd_reg_map[CMD_RETR] = pattern;

    pattern = "^STOR [a-zA-Z0-9/_-]+";
    _cmd_reg_map[CMD_STOR] = pattern;


    // "quit"
    pattern = "^QUIT";
    // regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    _cmd_reg_map[CMD_QUIT] = pattern;

    return;
}


int BaseServer::_parse_config(char* config_path) {
    // FILE* config = fopen(config_path, "r");

    return 0;
}

int BaseServer::_get_localhost() {
    cout << "11111111111111111111111111" << endl;
    char hostname[MSG_MAX_LEN_TINY_SHORT];
    gethostname(hostname, sizeof(hostname));
    struct hostent* host = gethostbyname(hostname);
    char ip[32];
    _localhost = inet_ntop(AF_INET, host->h_addr, ip, sizeof(ip));
    cout << _localhost << endl;
    cout <<"lcoalhost: " << _localhost << endl;
    return 0;
}