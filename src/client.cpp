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

#include "client.h"
#include "util.h"

using namespace std;


// public member functions ///////////////////////////////////
Client::Client() {
    __rev_buf = "";
    bzero(&__serv_ctr_addr, sizeof(__serv_ctr_addr));
    bzero(&__serv_data_addr, sizeof(__serv_data_addr));
    bzero(&__client_ctr_addr, sizeof(__client_ctr_addr));
    bzero(&__client_data_addr, sizeof(__client_data_addr));
}

Client::~Client() {    
}

int Client::init() {
    __cmds_map = __creat_cmd_map();
    __status = 0;
    return 0;
}


int Client::handle_cmds(string cmd) {
    vector<string> vec_cmd = split(cmd, ' ');
    vector<string>::iterator ctr_iter = vec_cmd.begin();
    map<string, int>::const_iterator cmd_iter = __cmds_map.find(*ctr_iter);
    print_vector<string>(vec_cmd.begin(), vec_cmd.end());
    cout << endl;
    int cmd_id = cmd_iter -> second;
    switch(cmd_id) {
        case CMD_QUIT:
            __handle_cmd_quit();
            break;
        case CMD_EXIT:
            __handle_cmd_quit();
            break;
        case CMD_OPEN:
            __handle_cmd_open(ctr_iter);
            break;
        case CMD_USER:
            __handle_cmd_user(ctr_iter);
            break;
        case CMD_PASS:
            __handle_cmd_pass(ctr_iter);
            break;
        case CMD_CWD:
            __handle_cmd_cwd(ctr_iter);
            break;
        case CMD_PWD:
            __handle_cmd_pwd(ctr_iter);
            break;
        case CMD_LIST:
            __handle_cmd_ls(ctr_iter);
            break;
        case CMD_GET:
            __handle_cmd_get(ctr_iter);
            break;
        case CMD_PUT:
            __handle_cmd_put(ctr_iter);
            break;
        default:
            __handle_cmd_unknown();
            break;
    }
    return cmd_id;
}

int Client::__handle_cmd_open(vector<string>::iterator ctr_iter) {
    cout << "want to connect to a ftp server" << endl;
    __serv_ip = *(++ctr_iter);
    __serv_ctr_port = atoi((*(++ctr_iter)).c_str());
    __generate_sockaddr_in(__serv_ctr_addr, __serv_ip, __serv_ctr_port);
    int r = connect_server(__ctr_sockfd, __serv_ctr_addr);
    if (r == -1) {
        cout << "ERROR: can not connect to ftp server" << endl;
    } else {
        cout << "conneted to ftp server" << __serv_ip << ':' << __serv_ctr_port << endl;
    }
    __status = __status | STATUS_CONNECT;
    return 0;
}

int Client::__handle_cmd_user(vector<string>::iterator ctr_iter) {
    cout << "want to login with user name" << endl;
    if (!(__status & STATUS_CONNECT)) {
        cout << "please connect to ftp server using open command." << endl;
        return -1;
    }
    string username = *(++ctr_iter);
    int r = user(username);
    if (r == -1) {
        cout << "ERROR: can not send user" << endl;
    }
    __status = __status | STATUS_USER;
    return 0;
}


int Client::__handle_cmd_pass(vector<string>::iterator ctr_iter) {
    string password = *(++ctr_iter);
    if (!(__status & STATUS_USER)) {
        cout << "please input username using user command." << endl;
        return -1;
    }
    cout << "password:" << password << endl;
    int r = pass(password);
    if (r == -1) {
        cout << "ERROR: can not send password" << endl;
    }
    __status = __status | STATUS_LOGIN;
    return 0;
}

int Client::__handle_cmd_cwd(vector<string>::iterator ctr_iter) {
    if (!__check_login()) {
        return -1;
    }
    cout << "want to change working directory" << endl;
    string path = *(++ctr_iter);
    int r = cwd(path);
    if (r == -1) {
        cout << "ERROR: CWD failed" << endl;
    }
    return 0;
}


int Client::__handle_cmd_pwd(vector<string>::iterator ctr_iter) {
    if (!__check_login()) {
        return -1;
    }
    int r = pwd();
    if (r == -1) {
        cout << "ERROR:  PWD failed" << endl;
    }
    return 0;
}

int Client::__handle_cmd_quit() {
    if (__status & STATUS_CONNECT) {
        __quit();
        close(__ctr_sockfd);
    }
    __status = __status | STATUS_QUIT;
    return 0;
}

int Client::__quit() {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "QUIT%s", CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_TINY_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}

int Client::__handle_cmd_ls(vector<string>::iterator ctr_iter) {
    if (!__check_login()) {
        return -1;
    }
    cout << "want to list the directory" << endl;
    int r = __pasv();
    if (r == -1) {
        cout << "ERROR: can not send pasv command" << endl;
    }
    __generate_sockaddr_in(__serv_data_addr, __serv_ip, __serv_data_port);
    connect_server(__data_sockfd, __serv_data_addr, CONNECT_NO_READ);

    r = ls();
    if (r == -1) {
        cout << "ERROR: LS failed" << endl;
    }
    close(__data_sockfd);
    return 0;
}



int Client::connect_server(int &sockfd, const struct sockaddr_in &serv_addr, int flags) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }
    sockfd = fd;

    int r;
    r = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (r == -1) {
        perror("connect");
        return -1;
    }

    // read the response
    if (flags != CONNECT_NO_READ) {
    char rev_buf[MSG_MAX_LEN];
    int n_bytes = read(sockfd, rev_buf, MSG_MAX_LEN - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);  
    }
    return 0;
}

int Client::user(const string user) {
    char send_buf[MSG_MAX_LEN];
    sprintf(send_buf, "USER %s%s", user.c_str(), CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {

    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}


int Client::pass(const string password) {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "PASS %s%s", password.c_str(), CRLF);
    printf("%s", send_buf);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_TINY_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {

    print_rev_msg(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}

int Client::__pasv() {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "PASV%s", CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_TINY_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    __parse_serv_data_port(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}

int Client::__parse_serv_data_port(char* pasv_response) {
    const char* pattern = "227.+\\([0-9]+,[0-9]+,[0-9]+,[0-9]+,([0-9]+),([0-9]+)\\)\\.";
    int port_high;
    int port_low;
    regex_t reg;
    regcomp(&reg, pattern, REG_EXTENDED|REG_NEWLINE);
    int status;
    char temp[5];
    regmatch_t pmatch[3];
    status = regexec(&reg, pasv_response, 3, pmatch, 0);
    if (status == REG_NOMATCH) {
        cout << "could not find server data port" << endl;
        return -1;
    }
    else if (status == 0) {
        int i, j;
        i = pmatch[1].rm_so;
        j = pmatch[1].rm_eo;
        bzero(temp, sizeof(temp));
        memcpy(temp, &pasv_response[i], j - i);
        port_high = atoi(temp);
        i = pmatch[2].rm_so;
        j = pmatch[2].rm_eo;
        string port_low_str = string(&pasv_response[i], j - i);
        bzero(temp, sizeof(temp));
        memcpy(temp, &pasv_response[i], j - i);
        port_low = atoi(temp);
        __serv_data_port = port_high * 256 + port_low;
    }
    regfree(&reg);
    return 0;
}

int Client::cwd(const string path) {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "CWD %s%s", path.c_str(), CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}

int Client::pwd() {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "PWD%s", CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_TINY_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}

int Client::ls(const string path) {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    if (path == "") {
        sprintf(send_buf, "LIST%s", CRLF);
    }
    else {
        sprintf(send_buf, "LIST %s%s", path.c_str(), CRLF);
    }
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_TINY_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN_TINY_SHORT - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    char rev_buf_long[MSG_MAX_LEN_LONG];
    n_bytes = read(__data_sockfd, rev_buf_long, MSG_MAX_LEN_LONG - 1);
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {
    rev_buf_long[n_bytes] = '\0';
    print_rev_msg(rev_buf_long);
    }

    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN_TINY_SHORT - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    } else {
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    }
    return 0;
}

int Client::__handle_cmd_get(vector<string>::iterator ctr_iter) {
    if (!__check_login()) {
        return -1;
    }
    int r = __pasv();
    if (r == -1) {
        cout << "ERROR: can not send pasv command" << endl;
    }
    __generate_sockaddr_in(__serv_data_addr, __serv_ip, __serv_data_port);
    connect_server(__data_sockfd, __serv_data_addr, CONNECT_NO_READ);
    string remote = *(++ctr_iter);
    string local(remote);
    r = __retr(remote, local);
    if (r == -1) {
        cout << "ERROR: RETR failed" << endl;
    }
    close(__data_sockfd);

    return 0;
}


// get remote file to save to local.
int Client::__retr(string remote, string local) {
    cout << remote << endl;
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "RETR %s%s", remote.c_str(), CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN_SHORT - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    int local_fd = open(local.c_str(), O_RDWR | O_CREAT | O_TRUNC, 00664);
    char rev_buf_long[MSG_MAX_LEN_LONG];
    size_t chunk = 0;
    do {
        n_bytes = read(__data_sockfd, rev_buf_long, MSG_MAX_LEN_LONG - 1);
        if (n_bytes == -1) {
            perror("read");
            close(local_fd);
            return -1;
        }
        else if (n_bytes > 0) {
            cout << "chunk number:" << ++chunk << endl;
            write(local_fd, rev_buf_long, n_bytes);
        }
    } while(n_bytes > 0);
    close(local_fd);

    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN_SHORT - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }

    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);


    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    return 0;
}

int Client::__handle_cmd_put(vector<string>::iterator ctr_iter) {
    if (!__check_login()) {
        return -1;
    }
    int r = __pasv();
    if (r == -1) {
        cout << "ERROR: can not send pasv command" << endl;
    }
    __generate_sockaddr_in(__serv_data_addr, __serv_ip, __serv_data_port);
    connect_server(__data_sockfd, __serv_data_addr, CONNECT_NO_READ);

    string local = *(++ctr_iter);
    string remote(local);
    r = __stor(local, remote);
    if (r == -1) {
        cout << "ERROR: STOR failed" << endl;
    }
    return 0;
}

int Client::__stor(string local, string remote) {
    char send_buf[MSG_MAX_LEN_TINY_SHORT];
    sprintf(send_buf, "STOR %s%s", local.c_str(), CRLF);
    ssize_t n_bytes = write(__ctr_sockfd, send_buf, strlen(send_buf));
    if (n_bytes == -1) {
        perror("write");
        return -1;
    }

    char rev_buf[MSG_MAX_LEN_SHORT];
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN_SHORT - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }
    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);

    int local_fd = open(local.c_str(), O_RDONLY);
    char send_buf_long[MSG_MAX_LEN_LONG];
    int n_write;
    int chunk = 0;
    do {
        n_bytes = read(local_fd, send_buf_long, 1024);
        if (n_bytes == -1) {
            perror("read");
            close(local_fd);
            return -1;
        }
        else if (n_bytes > 0) {
            n_write = write(__data_sockfd, send_buf_long, n_bytes);
            cout << "chunk number:" << ++chunk << endl;
        }
    } while(n_bytes > 0 && n_write > 0);
    close(local_fd);
    close(__data_sockfd);
    n_bytes = read(__ctr_sockfd, rev_buf, MSG_MAX_LEN_SHORT - 1);    
    if (n_bytes == -1) {
        perror("read");
        return -1;
    }

    rev_buf[n_bytes] = '\0';
    print_rev_msg(rev_buf);


    FILE* pfile;
    pfile = fopen(LOG_PATH_CLIENT, "a+");
    fputs(rev_buf, pfile);
    fclose(pfile);

    return 0;
}

int Client::__generate_sockaddr_in(struct sockaddr_in &__serv_addr, string host, int port) {
    __serv_addr.sin_family = AF_INET;
    __serv_addr.sin_port = htons(port);
    int r = inet_pton(AF_INET, host.c_str(), &__serv_addr.sin_addr);
    if (r == 0) {
         cout << "inet_pton: invalid host" << endl;
        return -1;
    }
    else if (r == -1) {
        perror("inet_pton");
        return -1;
    }
    return 0;
}

int Client::__handle_cmd_unknown() {
    cout << "Unknown Command. Please use one of the following:" << endl;
    cout << "#######################################################" << endl;
    for (map<string, int>::iterator it = __cmds_map.begin(); it != __cmds_map.end(); ++it) {
        cout << it -> first << ' ';
    }
    cout << endl;
    cout << "#######################################################" << endl;
    return 0;
}

int Client::status() {
    return __status;
}


// private member functions //////////////////
map<string, int> Client::__creat_cmd_map() {
    map<string, int> m;
    m["quit"] = CMD_QUIT;
    m["exit"] = CMD_EXIT;
    m["open"] = CMD_OPEN;
    m["user"] = CMD_USER;
    m["pass"] = CMD_PASS;
    m["cd"] = CMD_CWD;
    m["pwd"] = CMD_PWD;
    m["ls"] = CMD_LIST;
    m["get"] = CMD_GET;
    m["put"] = CMD_PUT;

    return m;
}


int Client::print_rev_msg(const char* msg) {
    string msg_s = string(msg, strlen(msg));
    int len = msg_s.length();
    if (len == 0) {
        return 0;
    }
    cout << msg_s;
    __rev_buf = msg_s;
    return len;
}

bool Client::__check_login() {
    if (__status & STATUS_LOGIN) {
        return true;
    }
    cout << "please login first" << endl;
    return false;
}