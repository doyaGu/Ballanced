#ifndef COMMON_FIXEDSTRING_H
#define COMMON_FIXEDSTRING_H

#include <string.h>
#include <tchar.h>

class CFixedString
{
public:
    CFixedString() : m_Str(NULL) {}

	CFixedString(const CFixedString &rhs) : m_Str(NULL)
    {
        Assign(rhs.m_Str);
    }

	CFixedString(LPCSTR str) : m_Str(NULL)
    {
        Assign(str);
    }

    ~CFixedString()
    {
        Clear();
    }

    CFixedString &operator=(const CFixedString &str)
    {
        Assign(str.m_Str);
    }

    CFixedString &operator=(LPCSTR str)
    {
        Assign(str);
    }

    void Assign(LPCSTR str)
    {
        if (str && m_Str != str)
        {
			if (m_Str)
			{
				delete m_Str;
			}
            m_Str = new TCHAR[_tcslen(str) + 1];
            _tcscpy(m_Str, str);
        }
    }
    
    void Clear()
    {
        if (m_Str)
        {
            delete m_Str;
            m_Str = NULL;
        }
    }

    bool Empty()
    {
        return (m_Str == NULL || _tcslen(m_Str) == 0);
    }

    size_t Length()
    {
        return (m_Str) ? _tcslen(m_Str) : 0;
    }

    LPCSTR Str() const
    {
        return m_Str;
    }

private:
    LPSTR m_Str;
};

#endif // COMMON_FIXEDSTRING_H