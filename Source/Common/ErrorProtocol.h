#ifndef COMMON_ERRORPROTOCOL_H
#define COMMON_ERRORPROTOCOL_H

#include <exception>

class CErrorProtocolException : public std::exception {};

class CErrorProtocol
{
public:
    static CErrorProtocol &GetInstance();
    void Open(const char *filename, const char *path, bool overwrite);
    void Write(const char *srcfile, const char *func, const char *msg);
    void Write(const char *srcfile, const char *func, const char *msg, bool showMsgBox);

private:
    CErrorProtocol();
	CErrorProtocol(const CErrorProtocol&);
	CErrorProtocol& operator=(const CErrorProtocol&);

    char m_FullPath[512];
    char m_FileName[512];
    char m_Path[512];
};

inline void CErrorProtocol::Write(const char *srcfile, const char *func, const char *msg)
{
    Write(srcfile, func, msg, false);
}

#define TT_ERROR_OPEN(filename, path, overwrite) CErrorProtocol::GetInstance().Open(filename, path, overwrite)
#define TT_ERROR(srcfile, func, msg) CErrorProtocol::GetInstance().Write(srcfile, func, msg)
#define TT_ERROR_BOX(srcfile, func, msg) CErrorProtocol::GetInstance().Write(srcfile, func, msg, true)

#endif // COMMON_ERRORPROTOCOL_H
