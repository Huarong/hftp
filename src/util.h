#include <iostream>
#include <string>
#include <sstream>
#include <vector>


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
inline std::string &lstrip(std::string &s);
inline std::string &rstrip(std::string &s);
inline std::string &strip(std::string &s);

template<typename T>
int print_vector(typename std::vector<T>::iterator first, typename std::vector<T>::iterator last, char delim=' ') {
    for (typename std::vector<T>::iterator iter = first; iter != last; ++iter) {
            std::cout << *iter << delim;
    }
    return 0;
}

std::string int_to_string(const int n);