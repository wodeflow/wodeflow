#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <vector>
#include "wstringEx.hpp"


std::string trimEnd(std::string line);
std::string trim(std::string line);
std::string upperCase(std::string text);
std::string lowerCase(std::string text);
std::string unescNewlines(const std::string &text);
std::string escNewlines(const std::string &text);


/******************************************************************************/
/*                                                                            */
/******************************************************************************/

const char *fmt(const char *format, ...);

std::string sfmt(const char *format, ...);
inline bool fmtCount(const wstringEx &format, int &i, int &s);

// Only handles the cases i need for translations : plain %i and %s
bool checkFmt(const wstringEx &ref, const wstringEx &format);
wstringEx wfmt(const wstringEx &format, ...);
std::vector<std::string> stringToVector(const std::string &text, char sep);
std::vector<wstringEx> stringToVector(const wstringEx &text, char sep);


#endif // STRING_UTILS_HPP