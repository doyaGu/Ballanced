#ifndef PLAYER_CMDLINEPARSER_H
#define PLAYER_CMDLINEPARSER_H

#include <string>
#include <vector>

class CmdlineArg
{
public:
    CmdlineArg() : m_Values(), m_ValueCount(0) {}
    CmdlineArg(const std::string *values, int valueCount) : m_Values(values), m_ValueCount(valueCount) {}

    bool IsValid() const
    {
        return m_Values != NULL;
    }

    const std::string *GetArg() const
    {
        if (IsValid())
            return m_Values;
        return NULL;
    }

    int GetValueCount() const
    {
        return m_ValueCount;
    }

    bool GetValue(int i, std::string &value) const;

    bool GetValue(int i, long &value) const;

private:
    const std::string *m_Values;
    int m_ValueCount;
};

class CmdlineParser
{
public:
    CmdlineParser(int argc, char **argv);

    bool Next(CmdlineArg &arg, const char *longopt, char opt = '\0', int maxValueCount = 0);

    bool Skip()
    {
        if (m_Index < m_Args.size())
        {
            ++m_Index;
            return true;
        }
        return false;
    }

    bool Done() const
    {
        return m_Index >= m_Args.size();
    }

    void Reset()
    {
        m_Index = 0;
    }

private:
    std::vector<std::string> m_Args;
    unsigned int m_Index;
};

#endif // PLAYER_CMDLINEPARSER_H