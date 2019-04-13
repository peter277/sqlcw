
#pragma once

#include <cctype>
#include <string>

class LineBuffer {
    std::string line;
    bool wrote_nonspace = false;

public:
    void append(char c)
    {
        if (!isspace(c)) wrote_nonspace = true;
        line += c;
    }

    void append(const char *s)
    {
        if (!wrote_nonspace)
        {
            for (int i = 0; s[i] != 0; i++)
            {
                if (!isspace(s[i]))
                {
                    wrote_nonspace = true;
                    break;
                }
            }
        }

        line += s;
    }

    void append(const std::string& s)
    {
        append(s.c_str());
    }

    template<typename T>
    LineBuffer& operator+=(T s)
    {
        append(s);
        return *this;
    }

    void clear()
    {
        line.clear();
        wrote_nonspace = false;
    }

    const char * c_str() const
    {
        return line.c_str();
    }
};
