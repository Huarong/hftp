#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <arpa/inet.h>
#include <regex.h>

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


class BaseServer
{
public:
    BaseServer();
    virtual ~BaseServer() = 0;
    int run();

protected:
    int _init();
    int _handle_cmd();

    // read request from client
    int _read_request(const int sockfd, char* rev_buf, size_t buf_len);
    // write response to client
    int _write_response(const int sockfd, const char* send_buf);

    int _print_rev_msg(const char* msg);

    int _identify_cmd(const char* msg);

    void _create_cmd_reg_map();

    int _handle_cmd_user(const char* user);
    int _handle_cmd_pass(const char* user, const char* pass);
    int _handle_cmd_pwd(const char* cur_path);
    int _handle_cmd_get(const char* server_file);
    int _handle_cmd_put(const char* client_file);

    int _parse_config(char* config_path);


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
    virtual void __log(const char* buf) = 0;

};