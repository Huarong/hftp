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

#include "session.h"
#include "util.h"

using namespace std;



Session::Session(int sockfd, string localhost, string root_path) {
    __connect_sockfd = sockfd;
    __cur_path = "/";
    __localhost = localhost;
    __root_path = root_path;
}

Session::~Session() {
}


int Session::__handle_cmd_user(const char* cmd) {
    char* new_cmd = __strip_CRLF(cmd);
    __user = split(string(new_cmd), ' ')[1];
    cout << "user:" << __user << endl;
    __write_response("331 Please specify the password.\r\n");
    return 0;
}

int Session::__handle_cmd_pass(const char* cmd, map<string, string> &account) {
    char* new_cmd = __strip_CRLF(cmd);
    cout << "new cmd:" << new_cmd << endl;
    __pass = split(string(new_cmd), ' ')[1];
    map<string, string>::iterator it = account.find(string(__user));
    if (it == account.end()) {
        cout << "user not exist" << endl;
        return -1;
    }
    if (string(__pass) != it -> second) {
        __write_response("530 Login incorrect.\r\n");
        return -1;
    }
    __write_response("230 Login successful.\r\n");
    return 0;
}

int Session::__handle_cmd_pwd(const char* cmd) {
    string msg = "257 \"" + __cur_path + "\"\r\n";
    __write_response(msg.c_str());
    return 0;
}

int Session::__handle_cmd_get(const char* cmd, const string root_path) {
    char* new_cmd = __strip_CRLF(cmd);
    string filename = split(new_cmd, ' ')[1];
    string abs_filename = root_path + filename;

    return 0;
}
int Session::__handle_cmd_put(const char* cmd) {
    return 0;
}


int Session::__handle_cmd_pasv(const char* cmd) {

    // create a data socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }
    __data_sockfd = sockfd;

    // initialize the sockaddr_in
    bzero(&__serv_data_addr,sizeof(__serv_data_addr));
    __serv_data_addr.sin_family = AF_INET;
    __serv_data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    __serv_data_addr.sin_port = 0;

    int r;
    r = bind(sockfd, (struct sockaddr*) &__serv_data_addr, sizeof(__serv_data_addr));
    if (r == -1) {
        perror("bind");
        return -1;
    }

// get binded socket information
   socklen_t templen = sizeof(struct sockaddr);
   if (getsockname(sockfd, (struct sockaddr *)&__serv_data_addr,&templen) == -1){
      perror("getsockname");
      return -1;
   }
    int port_h = ntohs(__serv_data_addr.sin_port);
    printf("Server is listening on port %d\n",port_h);
    printf("Server is listening on ip %d\n",ntohl(__serv_data_addr.sin_addr.s_addr));

    //listen
    r = listen(sockfd, 1);
    if (r == -1) {
        perror("listen");
        return -1;
    }

    int port1 = port_h / 256;
    int port2 = port_h % 256;
    string ip_port;
    ip_port = __localhost;
    replace(ip_port.begin(), ip_port.end(), '.', ',');
    ip_port  += "," + int_to_string(port1) + ',' + int_to_string(port2);
    string msg = "227 Entering Passive Mode (" + ip_port + ").";
    __write_response(msg.c_str());
    return 0;
}


int Session::__handle_cmd_retr(const char* cmd) {
    char* new_cmd = __strip_CRLF(cmd);
    string filename = split(string(new_cmd), ' ')[1];
    cout << "----------------------" << endl;
    cout << filename << endl;
    string abs_filename = __root_path + filename;
    FILE* f = fopen(abs_filename.c_str(), "rb");
    if (f == NULL) {
        perror("fopen");
        return -1;
    }
    char send_buf[2048];
    int n_chunk;
    size_t chunk = 0;
    size_t chunk_size = 1024;
    while (true) {
        n_chunk = fread(send_buf, chunk_size, 2, f);
        if (0 == n_chunk) {
            cout << "send the file completly" << endl;
            break;
        }
        ///////////////////////////////////////////////////////////////////////////////
        // todo:
        // wront number size and send_buf
        __send_data(send_buf, chunk_size * n_chunk);
        ///////////////////////////////////////////////////////////////////////////////
        cout << "number chunk: " << ++chunk << endl;
    }
    fclose(f);
    close(__data_sockfd);
    __write_response("226 Transfer complete.");
    return 0;
}

int Session::__handle_cmd_stor(const char* cmd) {
    return 0;
}


int Session::__read_request(char* rev_buf, size_t buf_len) {
    int n_bytes = read(__connect_sockfd, rev_buf, buf_len -1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    __print_rev_msg(rev_buf);
    return 0;
}

int Session::__write_response(const char* send_buf, const size_t size) {
    size_t len = size;
    if (0 == size) {
        len = strlen(send_buf);
    }
    ssize_t n_bytes = write(__connect_sockfd, send_buf, len);
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }
    return 0;
}

int Session::__send_data(const char* send_buf, const size_t size) {
    cout << "000000000000000000000000000000000" << endl;
    ssize_t n_bytes = write(__data_sockfd, send_buf, size);
    cout << "1111111111111111111111111111111111" << endl;
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }
    cout << "33333333333333333333333333333333333333" << endl;
    return 0;
}

int Session::__print_rev_msg(const char* msg) {
    string msg_s = string(msg, strlen(msg));
    int len = msg_s.length();
    if (len == 0) {
        return 0;
    }
    cout << msg_s;
    return len;
}

char* Session::__strip_CRLF(const char* cmd) {
    string str_cmd(cmd);
    return const_cast<char*>(str_cmd.substr(0, str_cmd.length() -2).c_str());
}
