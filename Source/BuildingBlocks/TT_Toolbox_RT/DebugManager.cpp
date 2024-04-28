#include "DebugManager.h"

DebugManager::DebugManager(CKContext *context) : CKBaseManager(context, TT_DEBUG_MANAGER_GUID, "TT_DebugManager")
{
    m_LogFile = NULL;
    m_DebugMode = false;
    m_LogFileNotOpened = true;
    context->RegisterNewManager(this);
}

DebugManager::~DebugManager() {}

void DebugManager::WriteToLogFile(const char *filename, const char *format, ...) {
    if (m_LogFileNotOpened)
    {
        m_LogFile = fopen(filename, "w");
        m_LogFileNotOpened = false;
    }
    else
    {
        m_LogFile = fopen(filename, "a");
    }

    va_list ap;
        va_start(ap, format);
    vfprintf(m_LogFile, format, ap);
        va_end(ap);

    fclose(m_LogFile);
    m_LogFile = NULL;
}
