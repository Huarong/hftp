#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <arpa/inet.h>

using namespace std;

#ifndef HFTP_CLIENT_CLIENT_H_
#define HFTP_CLIENT_CLIENT_H_

#define LOG_PATH_CLIENT "../log/client.log"

#define MSG_MAX_LEN_LONG 8192
#define MSG_MAX_LEN 1024
#define MSG_MAX_LEN_SHORT 256
#define MSG_MAX_LEN_TINY_SHORT 64
#define CRLF "\r\n"
#define BACKLOG 10
#define CONNECT_NO_READ 1



#define CMD_QUIT 80
#define CMD_EXIT 81
#define CMD_OPEN 4
#define CMD_USER 6
#define CMD_PASS 7
#define CMD_CWD 9
#define CMD_PWD 11
#define CMD_LIST 15
#define CMD_GET 25
#define CMD_PUT 35


// FTP status
#define STATUS_CONNECT 1
#define STATUS_USER 2
#define STATUS_LOGIN 4
#define STATUS_QUIT 8


class Client
{
public:
    Client();
    ~Client();
    int init();
    int handle_cmds(string cmd);
    int status();

private:
    map<string, int> __cmds_map;
    struct sockaddr_in __serv_ctr_addr;
    struct sockaddr_in __serv_data_addr;
    struct sockaddr_in __client_ctr_addr;
    struct sockaddr_in __client_data_addr;

    int __ctr_sockfd;
    int __data_sockfd;
    string __serv_ip;
    int __serv_ctr_port;
    int __serv_data_port;
    string __rev_buf;
    int __status;

    int __generate_sockaddr_in(struct sockaddr_in &__serv_addr, string host, int port);
    int __establish_port_mode();
    int __pasv();
    int __parse_serv_data_port(char* pasv_response);
    map<string, int> __creat_cmd_map();
    int print_rev_msg(const char* msg);
    bool __check_login();


    ///////////////////////////////////////////////////////////////////////////////
    // handle various commands
    int __handle_cmd_open(vector<string>::iterator ctr_iter);
    int __handle_cmd_user(vector<string>::iterator ctr_iter);
    int __handle_cmd_pass(vector<string>::iterator ctr_iter);
    int __handle_cmd_cwd(vector<string>::iterator ctr_iter);
    int __handle_cmd_pwd(vector<string>::iterator ctr_iter);
    int __handle_cmd_ls(vector<string>::iterator ctr_iter);
    int __handle_cmd_get(vector<string>::iterator ctr_iter);
    int __handle_cmd_put(vector<string>::iterator ctr_iter);
    int __handle_cmd_quit();
    int __handle_cmd_unknown();
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    int connect_server(int &__sockfd, const struct sockaddr_in &__serv_addr, int flags=0);
    int user(const string user="anonymous");
    int pass(const string password="");
    int port(const string host, const string port);
    int pwd();
    int cwd(const string path);
    int ls(const string path="");
    int __retr(string remote, string local);
    int __stor(string local, string remote);
    int __quit();
    ///////////////////////////////////////////////////////////////////////////////
};

#endif
