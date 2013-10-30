#include <iostream>
#include <string>
#include <map>

#include "client.h"

using namespace std;


int main(int argc, char const *argv[]) {
    string cmd;
    Client client;
    client.init();
    while(true) {
        cout << "hftp>> ";
        getline(cin, cmd);
        if (cmd.length() == 0) {
            continue;
        }
        client.handle_cmds(cmd);
        if (client.status() & STATUS_QUIT) {
            break;
        }
    }
    return 0;
}