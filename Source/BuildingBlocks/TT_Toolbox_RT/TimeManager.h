#ifndef TOOLBOX_TIMEMANAGER_H
#define TOOLBOX_TIMEMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

#define TT_TIME_MANAGER_GUID CKGUID(0x60632E28, 0x7D3B3C7D)

class TimeManager : public CKBaseManager
{
public:
    TimeManager(CKContext *ctx);
    ~TimeManager();

    CKERROR OnCKInit() { return CK_OK; }
    CKERROR OnCKEnd() { return CK_OK; }
    CKERROR OnCKPlay() { return CK_OK; }
    CKERROR OnCKReset() { return CK_OK; }
    CKERROR PreClearAll() { return CK_OK; }
    CKERROR PostClearAll() { return CK_OK; }
    CKERROR PostProcess() { return CK_OK; }
    CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_PostProcess |
               CKMANAGER_FUNC_PreClearAll |
               CKMANAGER_FUNC_PostClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_OnCKReset;
    }

    void Reset() { m_Timing = true; m_Time = 0.0f; }

    void Pause() { m_Timing = false; }

    void Play() { m_Timing = true; }

    float Now() const { return m_Time; }

    void Elapse(float delta) { if (m_Timing) m_Time += delta; }

    static TimeManager *GetManager(CKContext *context)
    {
        return (TimeManager *)context->GetManagerByGuid(TT_TIME_MANAGER_GUID);
    }

protected:
    bool m_Timing;
    float m_Time;
};

#endif // TOOLBOX_TIMEMANAGER_H