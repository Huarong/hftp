#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdarg>

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


// n: number of member to be print
// template <typename T>
// void write2screen_no_endl(int n, const T& s1, ...) {
//     T s;
//     va_list ap;
//     va_start(ap, s1);
//     for (int i = 0; i < n; ++i)
//     {
//         s = va_arg(ap, T);
//         std::cout << s;
//     }
//     va_end(ap);
//     return;
// }


// template <typename T>
// void write2screen(int n, const T& s1, ...) {
//     T s;
//     va_list ap;
//     va_start(ap, s1);
//     for (int i = 0; i < n; ++i)
//     {
//         s = va_arg(ap, T);
//         std::cout << s;
//     }
//     std::cout << std::endl;
//     va_end(ap);
//     return;
// }
