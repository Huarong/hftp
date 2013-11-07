#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <arpa/inet.h>
#include <regex.h>


using namespace std;

#ifndef HFTP_SRC_SESSION_H_
#define HFTP_SRC_SESSION_H_

#define MSG_MAX_LEN_LONG 8192
#define MSG_MAX_LEN 1024
#define MSG_MAX_LEN_SHORT 256
#define MSG_MAX_LEN_TINY_SHORT 64
#define CRLF "\r\n"
#define BACKLOG 10


class Session
{
public:
    Session(int sockfd, string localhost, string root_path);
    Session();
    ~Session();

    Session& operator=(const Session &);



    int __handle_cmd_user(const char* cmd);
    int __handle_cmd_pass(const char* cmd, map<string, string> &account);
    int __handle_cmd_pwd(const char* cmd);
    int __handle_cmd_get(const char* cmd, const string root_path);
    int __handle_cmd_put(const char* cmd);
    int __handle_cmd_pasv(const char* cmd);
    int __handle_cmd_retr(const char* cmd);
    int __handle_cmd_stor(const char* cmd);

    // read request from client
    int __read_request(char* rev_buf, size_t buf_len);
    // write response to client
    int __write_response(const char* send_buf, const size_t size=0);

    // send data via data socket
    int __send_data(const char* send_buf, const size_t size);

    int __print_rev_msg(const char* msg);

    char* __strip_CRLF(const char* cmd);

    string __cur_path;

private:
    string __root_path;
    int __connect_sockfd;
    int __data_sockfd;
    int __connect_data_sockfd;
    struct sockaddr_in __serv_data_addr;
    string __user;
    string __pass;
    string __localhost;
};

#endif