#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstdlib>

// http://stackoverflow.com/a/236803/1282982
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (item.length() > 0) {
            elems.push_back(item);
        }
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
///////////////////////////////////////////////////////////////////////////////

//http://stackoverflow.com/a/217605/1282982

// strip from start
inline std::string &lstrip(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// strip from end
inline std::string &rstrip(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// strip from both ends
inline std::string &strip(std::string &s) {
        return lstrip(rstrip(s));
}
///////////////////////////////////////////////////////////////////////////////

// convert from int to string
std::string int_to_string(const int n) {
    std::stringstream ss;
    ss << n;
    std::string str = ss.str();
    return str;
}

//////////////////////////////////////////////////////////////////////////////

// int main(int argc, char const *argv[]) {
//     const std::string cmd("    hello,  world   ,   ,  may space");
//     std::vector<std::string> elems = split(cmd, ' ');
//     for (std::vector<std::string>::iterator iter = elems.begin(); iter != elems.end(); ++iter) {
//         strip(*iter);
//         std::cout << *iter << std::endl;
//     }
//     return 0;
// }