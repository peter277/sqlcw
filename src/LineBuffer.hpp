#pragma once

#include <cctype>
#include <cstdio>
#include <string>

class LineBuffer {
    std::string line;
    bool wrote_nonspace = false;

public:
    LineBuffer& append(char c)
    {
        if (!isspace(c)) wrote_nonspace = true;
        line += c;
        return *this;
    }

    LineBuffer& append(const char *s)
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
        return *this;
    }

    LineBuffer& append(const std::string& s)
    {
        return append(s.c_str());
    }

    template<typename T>
    LineBuffer& operator+=(T s)
    {
        return append(s);
    }

    bool has_nonspace() const
    {
        return wrote_nonspace;
    }

    void write_line(FILE *fout)
    {
        fputs(line.c_str(), fout);
        clear();
    }

    void write_nonspace_line(FILE *fout)
    {
        if (wrote_nonspace) fputs(line.c_str(), fout);
        clear();
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
