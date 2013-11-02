#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstdlib>

#include "util.h"

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


// replace all substring
// replace_all_distinct(string("12212"),"12","21");
// RETURN: 21221
// http://www.vimer.cn/2009/11/string%E6%9B%BF%E6%8D%A2%E6%89%80%E6%9C%89%E6%8C%87%E5%AE%9A%E5%AD%97%E7%AC%A6%E4%B8%B2%EF%BC%88c%EF%BC%89.html
std::string&  replace_all(std::string&   str, const  std::string&  old_value, const  std::string&  new_value)   
{   
    for(std::string::size_type pos(0);  pos!=std::string::npos;  pos += new_value.length()) {
        if(   (pos=str.find(old_value,pos))!=std::string::npos   )   
            str.replace(pos,old_value.length(),new_value);   
        else   break;   
    }   
    return   str;   
}   




//////////////////////////////////////////////////////////////////////////////

// int main(int argc, char const *argv[]) {
//     // write2screen(3, "hello", "hwo", "shit");
//     return 0;
// }

