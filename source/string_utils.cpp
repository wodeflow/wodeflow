#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <stdint.h>

#include "string_utils.hpp"

static const char *g_whitespaces = " \f\n\r\t\v";

std::string trimEnd(std::string line)
{
	std::string::size_type i = line.find_last_not_of(g_whitespaces);
	if (i == std::string::npos)
		line.clear();
	else
		line.resize(i + 1);
	return line;
}

std::string trim(std::string line)
{
	std::string::size_type i = line.find_last_not_of(g_whitespaces);
	if (i == std::string::npos)
	{
		line.clear();
		return line;
	}
	else
	{
		line.resize(i + 1);
	}
	
	i = line.find_first_not_of(g_whitespaces);
	if (i > 0)
		line.erase(0, i);
	return line;
}

std::string upperCase(std::string text)
{
	char c;

	for (std::string::size_type i = 0; i < text.size(); ++i)
	{
		c = text[i];
		if (c >= 'a' && c <= 'z')
			text[i] = c & 0xDF;
	}
	return text;
}

std::string lowerCase(std::string text)
{
	char c;

	for (std::string::size_type i = 0; i < text.size(); ++i)
	{
		c = text[i];
		if (c >= 'A' && c <= 'Z')
			text[i] = c | 0x20;
	}
	return text;
}

std::string unescNewlines(const std::string &text)
{
	std::string s;
	bool escaping = false;

	s.reserve(text.size());
	for (std::string::size_type i = 0; i < text.size(); ++i)
	{
		if (escaping)
		{
			switch (text[i])
			{
				case 'n':
					s.push_back('\n');
					break;
				default:
					s.push_back(text[i]);
			}
			escaping = false;
		}
		else if (text[i] == '\\')
			escaping = true;
		else
			s.push_back(text[i]);
	}
	return s;
}

std::string escNewlines(const std::string &text)
{
	std::string s;

	s.reserve(text.size());
	for (std::string::size_type i = 0; i < text.size(); ++i)
	{
		switch (text[i])
		{
			case '\n':
				s.push_back('\\');
				s.push_back('n');
				break;
			case '\\':
				s.push_back('\\');
				s.push_back('\\');
				break;
			default:
				s.push_back(text[i]);
		}
	}
	return s;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/

// Simplified use of sprintf
const char *fmt(const char *format, ...)
{
	enum {
		MAX_MSG_SIZE	= 512,
		MAX_USES		= 8
	};

	static char buffer[MAX_USES][MAX_MSG_SIZE];
	static int currentStr = 0;
	va_list va;

	currentStr = (currentStr + 1) % MAX_USES;
	va_start(va, format);
	vsnprintf(buffer[currentStr], MAX_MSG_SIZE, format, va);
	buffer[currentStr][MAX_MSG_SIZE - 1] = '\0';
	va_end(va);
	return buffer[currentStr];
}

// 
std::string sfmt(const char *format, ...)
{
	va_list va;
	int length;
	std::string s;

	va_start(va, format);
	length = vsnprintf(0, 0, format, va) + 1;
	va_end(va);
	s.resize(length + 1);
	va_start(va, format);
	vsnprintf(&s[0], length, format, va);
	va_end(va);
	
	s[length] = '\0';
	
	return s;
}

inline bool fmtCount(const wstringEx &format, int &i, int &s)
{
	int state = 0;

	i = 0;
	s = 0;
	for (unsigned int k = 0; k < format.size(); ++k)
	{
		if (state == 0)
		{
			if (format[k] == L'%')
				state = 1;
		}
		else if (state == 1)
		{
			switch (format[k])
			{
				case L'%':
					state = 0;
					break;
				case L'i':
				case L'd':
					state = 0;
					++i;
					break;
				case L's':
					state = 0;
					++s;
					break;
				default:
					return false;
			}
		}
	}
	return true;
}

// Only handles the cases i need for translations : plain %i and %s
bool checkFmt(const wstringEx &ref, const wstringEx &format)
{
	int s;
	int i;
	int refs;
	int refi;
	if (!fmtCount(ref, refi, refs))
		return false;
	if (!fmtCount(format, i, s))
		return false;
	return i == refi && s == refs;
}

wstringEx wfmt(const wstringEx &format, ...)
{
	va_list va;
	int length;
	
	wstringEx ws;

	va_start(va, format);
	length = vswprintf(0, 0, format.c_str(), va) + 1;
	
	va_end(va);
	
	ws.resize(length);
	
	va_start(va, format);
	vswprintf(&ws[0], length, format.c_str(), va);
	
	va_end(va);
	return ws;
}

std::vector<std::string> stringToVector(const std::string &text, char sep)
{
	std::vector<std::string> v;
	if (text.empty())
		return v;
	uint32_t count = 1;
	for (uint32_t i = 0; i < text.size(); ++i)
		if (text[i] == sep)
			++count;
	v.reserve(count);
	std::string::size_type off = 0;
	std::string::size_type i = 0;
	do
	{
		i = text.find_first_of(sep, off);
		if (i != std::string::npos)
		{
			std::string ws(text.substr(off, i - off));
			v.push_back(ws);
			off = i + 1;
		}
		else
			v.push_back(text.substr(off));
	} while (i != std::string::npos);
	return v;
}

std::vector<wstringEx> stringToVector(const wstringEx &text, char sep)
{
	std::vector<wstringEx> v;
	if (text.empty())
		return v;
	uint32_t count = 1;
	for (uint32_t i = 0; i < text.size(); ++i)
		if (text[i] == sep)
			++count;
	v.reserve(count);
	wstringEx::size_type off = 0;
	wstringEx::size_type i = 0;
	do
	{
		i = text.find_first_of(sep, off);
		if (i != wstringEx::npos)
		{
			wstringEx ws(text.substr(off, i - off));
			v.push_back(ws);
			off = i + 1;
		}
		else
			v.push_back(text.substr(off));
	} while (i != wstringEx::npos);
	return v;
}