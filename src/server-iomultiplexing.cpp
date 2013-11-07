#include <iostream>
#include <cstdio>
#include <cstring>

#include <sys/ioctl.h>
#include <poll.h>

#include "server-iomultiplexing.h"
#include "session.h"
#include "util.h"

using namespace std;

IOMultiplexingServer::IOMultiplexingServer(): BaseServer() {
}

IOMultiplexingServer::~IOMultiplexingServer() {
}

int IOMultiplexingServer::__handle_accept() {
    int on = 1;
    int r = ioctl(_ctr_sockfd, FIONBIO, (char *)&on);
    if (r < 0) {
        perror("ioctl() failed");
        close(_ctr_sockfd);
        return -1;
    }

    memset(__fds, 0, sizeof(__fds));

    for (int i = 1; i < MAX_POLL_NUM; ++i)
    {
        __fds[i].fd = -1;
    }

    __fds[0].fd = _ctr_sockfd;
    __fds[0].events = POLLIN;
    __nfds = 1;

    while (true) {
        __handle_an_accept();
    }
    return 0;
}


int IOMultiplexingServer::__handle_an_accept() {
    cout << "begin to execute poll()" << endl;
    cout << "__nfds:" << __nfds << endl;
    cout << "fd:" << __fds[0].fd << endl;
    cout << "events:" << __fds[0].events << endl;
    cout << "revents:" << __fds[0].revents << endl;

    int nready= poll(__fds, __nfds, POLL_TIME_OUT);
    if (nready < 0) {
        perror("poll");
        return -1;
    } else if (nready == 0) {
        cout << "poll TIME OUT" << endl;
        return -1;
    }

    if (__fds[0].revents != POLLIN) {
        cout << "poll() not return POLLIN" << endl;
        return -1;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int connect_fd = accept(_ctr_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
    if (connect_fd == -1) {
        perror("accept");
        return -1;
    }

    // find a place in __fds to store connect_fd.
    if (__nfds >= MAX_POLL_NUM) {
        cout << "ERROR: too many connections to the server!" << endl;
        return -1;
    }
    for (int i = 1; i < MAX_POLL_NUM; ++i)
    {
        if (__fds[i].fd < 0) {
            __fds[i].fd = connect_fd;
            __fds[i].events = POLLIN;
            cout << "__nfds++" << endl;
            __nfds++;
            break;
        }
    }
    cout << "__fds[0].fd: " <<  __fds[0].fd << endl;
    cout << "__fds[0].events: " << __fds[0].events << endl;
    cout << "__fds[0].revents: " << __fds[0].revents << endl;

    for (int i = 1; i < MAX_POLL_NUM; ++i)
    {
        cout << i << endl;
        cout << "fd:" << __fds[i].fd << endl;
        cout << "events:" << __fds[i].events << endl;
        cout << "revents:" << __fds[i].revents << endl;
        if (__fds[i].fd < 0) {
            continue;
        }

        cout << "begin to create a session" << endl;
        Session session = _create_session(__fds[i].fd);
        _handle_cmds(session);
        close(__fds[i].fd);
        __fds[i].fd = -1;
        cout << "__nfds--" << endl;
        __nfds--;
    }
    return 0;
}

void IOMultiplexingServer::__log(const char* msg) {
    FILE* pfile;
    pfile = fopen(LOG_PATH_SERVER_IO, "a+");
    fputs(msg, pfile);
    fclose(pfile);
}