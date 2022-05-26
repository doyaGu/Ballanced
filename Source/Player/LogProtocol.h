#ifndef PLAYER_LOGPROTOCOL_H
#define PLAYER_LOGPROTOCOL_H

class CLogProtocol
{
public:
	static CLogProtocol &GetInstance();
	void Open(const char *filename, const char *path, bool overwrite);
	void Write(const char *str);

private:
	CLogProtocol();
	CLogProtocol(const CLogProtocol &);
	CLogProtocol& operator=(const CLogProtocol&);
	
	char m_FullPath[512];
	char m_FileName[512];
	char m_Path[512];
};

#define TT_LOG_OPEN(filename, path, overwrite) CLogProtocol::GetInstance().Open(filename, path, overwrite)
#define TT_LOG(str) CLogProtocol::GetInstance().Write(str)

#endif // PLAYER_LOGPROTOCOL_H
