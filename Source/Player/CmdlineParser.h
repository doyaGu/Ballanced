#ifndef PLAYER_CMDLINEPARSER_H
#define PLAYER_CMDLINEPARSER_H

#include <string>
#include <vector>

class CmdlineArg
{
public:
    CmdlineArg() {}
    CmdlineArg(const std::string *values, int size, bool jointed = false) : m_Values(values), m_Size(size), m_Jointed(jointed) {}

    bool GetValue(int i, std::string &value) const;
    bool GetValue(int i, long &value) const;

    int GetSize() const;

private:
    const std::string *m_Values;
    int m_Size;
    bool m_Jointed;
};

class CmdlineParser
{
public:
    CmdlineParser(int argc, char **argv);

    bool Next(CmdlineArg &arg, const char *longopt, char opt = '\0', int maxValueCount = 0);

    bool Skip();

    bool Done() const;

    void Reset();

private:
    std::vector<std::string> m_Args;
    size_t m_Index;
};

#endif // PLAYER_CMDLINEPARSER_H