#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <arpa/inet.h>
#include <regex.h>

#include "session.h"

using namespace std;


#define MSG_MAX_LEN_LONG 8192
#define MSG_MAX_LEN 1024
#define MSG_MAX_LEN_SHORT 256
#define MSG_MAX_LEN_TINY_SHORT 64
#define CRLF "\r\n"
#define BACKLOG 10

// log path for multi-process server
#define LOG_PATH_SERVER_MP "../log/server-mp.log"
// log path for multi-thread server
#define LOG_PATH_SERVER_MT "../log/server-mt.log"
// log path for IO multiplexing server
#define LOG_PATH_SERVER_IO "../log/server-io.log"

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
#define CMD_PASV 66
#define CMD_RETR 70
#define CMD_STOR 71


class BaseServer
{
public:
    BaseServer();
    virtual ~BaseServer() = 0;
    int run();

protected:
    int _init();
    int _handle_cmds(Session session);


    Session _create_session(int connect_fd);
    int _print_rev_msg(const char* msg);

    int _identify_cmd(const char* msg);

    void _create_cmd_reg_map();

    int _get_localhost();

    int _parse_config(char* config_path);

    string _root_path;

    string _localhost;

    map<string, string> _account;
    map<int, const char*> _cmd_reg_map;

    int _ctr_sockfd;
    int _data_sockfd;

    struct sockaddr_in _serv_ctr_addr;
    struct sockaddr_in _serv_data_addr;

    int _serv_ctr_port;
    int _serv_data_port;



private:
    virtual int __handle_accept();
    virtual int __handle_an_accept();

    virtual void __log(const char* buf) = 0;

};