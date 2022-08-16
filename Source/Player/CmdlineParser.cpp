#include "CmdlineParser.h"

#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

bool CmdlineArg::GetValue(int i, std::string &value) const
{
    if (i >= GetValueCount())
        return false;

    value = m_Values[i];
    return true;
}

bool CmdlineArg::GetValue(int i, long &value) const
{
    if (i >= GetValueCount())
        return false;

    const std::string &v = m_Values[i];
    const char *s = v.c_str();
    const char *e = s + v.size();
    long val = strtol(s, const_cast<char **>(&e), 10);
    if (s == e)
        return false;
    value = val;
    return true;
}

CmdlineParser::CmdlineParser(int argc, char **argv) : m_Index(0)
{
    if (argc > 0)
        m_Args.reserve(argc - 1);
    for (int i = 1; i < argc; ++i)
        m_Args.push_back(argv[i]);
}

bool CmdlineParser::Next(CmdlineArg &arg, const char *longopt, char opt, int maxValueCount)
{
    if (Done())
        return false;

    const std::string &s = m_Args[m_Index];
    if (s.size() < 2 || s[0] != '-')
        return false;

    if (strncmp(s.c_str(), longopt, strlen(longopt)) != 0 &&
        !(opt != '\0' && s.size() == 2 && s[1] == opt))
        return false;

    ++m_Index;
    std::string *values = &m_Args[m_Index];

    if (maxValueCount != 0)
    {
        int valCount = 0;
        int maxValCount = (maxValueCount == -1) ? m_Args.size() - m_Index : maxValueCount;
        while (valCount < maxValCount)
        {
            const std::string &next = m_Args[m_Index];
            if (!next.empty() && next[0] == '-')
                break;
            ++valCount;
            ++m_Index;
        }

        arg = CmdlineArg(values, valCount);
    }

    return true;
}