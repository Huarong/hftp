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

Session::Session() {
}

Session::~Session() {
}

Session& Session::operator=(const Session &session) {
    __connect_sockfd = session.__connect_sockfd;
    __cur_path = session.__cur_path;
    __localhost = session.__localhost;
    __root_path = session.__root_path;
    return *this;
}


int Session::__handle_cmd_user(const char* cmd) {
    char* new_cmd = __strip_CRLF(cmd);
    __user = split(string(new_cmd), ' ')[1];
    __write_response("331 Please specify the password.\r\n");
    return 0;
}

int Session::__handle_cmd_pass(const char* cmd, map<string, string> &account) {
    char* new_cmd = __strip_CRLF(cmd);
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
    __data_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (__data_sockfd == -1) {
        perror("socket");
        return -1;
    }
    // initialize the sockaddr_in
    bzero(&__serv_data_addr,sizeof(__serv_data_addr));
    __serv_data_addr.sin_family = AF_INET;
    __serv_data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    __serv_data_addr.sin_port = 0;

    int r;
    r = bind(__data_sockfd, (struct sockaddr*) &__serv_data_addr, sizeof(__serv_data_addr));
    if (r == -1) {
        perror("bind");
        return -1;
    }

// get binded socket information
   socklen_t templen = sizeof(struct sockaddr);
   if (getsockname(__data_sockfd, (struct sockaddr *)&__serv_data_addr,&templen) == -1){
      perror("getsockname");
      return -1;
   }
    int port_h = ntohs(__serv_data_addr.sin_port);
    printf("Server is listening on port %d\n",port_h);
    printf("Server is listening on ip %d\n",ntohl(__serv_data_addr.sin_addr.s_addr));

    //listen
    r = listen(__data_sockfd, BACKLOG);
    if (r == -1) {
        perror("listen");
        return -1;
    }

    int port1 = port_h / 256;
    int port2 = port_h % 256;
    cout << "port:" << endl;
    cout << port1 << endl;
    cout << port2 << endl;
    port1 = (port_h >> 8) & 255;
    port2 = port_h & 255;
    cout << port1 << endl;
    cout << port2 << endl;
    string ip_port;
    ip_port = __localhost;
    replace(ip_port.begin(), ip_port.end(), '.', ',');
    ip_port  += "," + int_to_string(port1) + ',' + int_to_string(port2);
    string msg = "227 Entering Passive Mode (" + ip_port + ").\r\n";
    __write_response(msg.c_str());
    return 0;
}


int Session::__handle_cmd_retr(const char* cmd) {
    char* new_cmd = __strip_CRLF(cmd);
    string filename = split(string(new_cmd), ' ')[1];
    string abs_filename = __root_path + filename;
    FILE* f = fopen(abs_filename.c_str(), "rb");
    if (f == NULL) {
        perror("fopen");
        return -1;
    }

    // accept data request
    struct sockaddr_in client_data_addr;
    socklen_t client_data_addr_len = sizeof(client_data_addr);
    int connect_data_fd = accept(__data_sockfd, (struct sockaddr *) &client_data_addr, &client_data_addr_len);
    if (connect_data_fd == -1) {
        perror("accept");
        return -1;
    }
    __connect_data_sockfd = connect_data_fd;

    // get file size
    unsigned long file_size;
    fseek(f, 0, SEEK_END);
    file_size = ftell(f);

    // seek back to the beginning.
    fseek(f, 0, SEEK_SET);

    char msg[MSG_MAX_LEN_SHORT];
    sprintf(msg, "150 Opening BINARY mode data connection for %s (%lu bytes).\r\n", filename.c_str(), file_size);
    __write_response(msg);

    char send_buf[2048];
    int n_chunk;
    size_t chunk = 0;
    size_t chunk_size = 1;
    size_t chunk_count = 1024;
    while (!feof(f) && !ferror(f)) {
        n_chunk = fread(send_buf, chunk_size, chunk_count, f);
        cout << n_chunk << endl;
        __send_data(send_buf, chunk_size * n_chunk);
        cout << "number chunk: " << ++chunk << endl;
    }
    fclose(f);
    close(__data_sockfd);
    close(__connect_data_sockfd);
    __write_response("226 Transfer complete.\r\n");
    return 0;
}

int Session::__handle_cmd_stor(const char* cmd) {
    char* new_cmd = __strip_CRLF(cmd);
    string filename = split(string(new_cmd), ' ')[1];
    string abs_filename = __root_path + __cur_path + filename;
    abs_filename = replace_all(abs_filename, "//", "/");
    cout << "to send file:" << abs_filename << endl;

    FILE* f = fopen(abs_filename.c_str(), "wb");
    if (f == NULL) {
        perror("fopen");
        return -1;
    }

///////////////////////////////////////////////////////////////////////////////
    // accept data request
    struct sockaddr_in client_data_addr;
    socklen_t client_data_addr_len = sizeof(client_data_addr);
    int connect_data_fd = accept(__data_sockfd, (struct sockaddr *) &client_data_addr, &client_data_addr_len);
    if (connect_data_fd == -1) {
        perror("accept");
        return -1;
    }
    __connect_data_sockfd = connect_data_fd;


    __write_response("150 Ok to send data.\r\n");

    char rev_buf_long[MSG_MAX_LEN_LONG];
    size_t chunk = 0;
    int n_bytes;
    do {
        n_bytes = read(__connect_data_sockfd, rev_buf_long, MSG_MAX_LEN_LONG - 1);
        if (n_bytes == -1) {
            perror("read");
            close(__connect_data_sockfd);
            return -1;
        }
        else if (n_bytes > 0) {
            cout << "chunk number:" << ++chunk << endl;
            fwrite(rev_buf_long, 1, n_bytes, f);
        }
    } while(n_bytes > 0);
    fclose(f);
    close(__connect_data_sockfd);
    close(__data_sockfd);
    __write_response("226 Transfer complete.\r\n");
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
    char new_send_buf[len + 1];
    memcpy(new_send_buf, send_buf, len);
    new_send_buf[len + 1] = '\0';
    cout << "SEND RESPONSE: " <<  new_send_buf;
    return 0;
}

int Session::__send_data(const char* send_buf, const size_t size) {
    ssize_t n_bytes = write(__connect_data_sockfd, send_buf, size);
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }
    cout << "send:" << n_bytes << endl;
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
