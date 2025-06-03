////
//
//
//  ACD : CD Wrapper
//
//
////
#include "CKALL.h"
#include "ACD.h"

#ifdef WIN32
#include "windows.h"
#include "mmsystem.h"

//////////////////////////////////////////
//  CONSTRUCTOR
ACD::ACD()
{
    wDeviceID = 0;
    m_state = MCI_MODE_STOP;

    first_track = 1;
    end_track = 1;
    start_time = 0;
    end_time = 0;
}

//////////////////////////////////////////
//  DESTRUCTOR
ACD::~ACD()
{
}

//////////////////////////////////////////
//  ATTRIBUTES

//////////////////////////////////////////
//  OPERATIONS

//___________________________________/ OPEN DEVICE
CKBOOL ACD::OpenDevice(char *name)
{

    DWORD dwReturn;
    MCI_OPEN_PARMS mciOpenParms;

    // Opens a CD audio device by specifying the device name.
    mciOpenParms.lpstrDeviceType = name;
    if (dwReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE,
                                  (DWORD)(LPVOID)&mciOpenParms))
        return FALSE;

    m_state = MCI_MODE_STOP;
    wDeviceID = mciOpenParms.wDeviceID;
    return TRUE;
}

//___________________________________/ CLOSE DEVICE
CKBOOL ACD::CloseDevice()
{

    DWORD dwReturn;
    MCI_GENERIC_PARMS mciGenericParams;

    // Close a CD audio device by specifying the device id.
    if (dwReturn = mciSendCommand(wDeviceID, MCI_CLOSE, MCI_OPEN_TYPE,
                                  (DWORD)(LPVOID)&mciGenericParams))
        return FALSE;

    wDeviceID = 0;

    return TRUE;
}

//___________________________________/ GET TRACK COUNT
int ACD::GetTrackCount()
{

    DWORD dwReturn;
    MCI_STATUS_PARMS mciStatusParms;

    // Get the number of tracks.
    mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
    if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS,
                                  MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms))
        return -1;
    return mciStatusParms.dwReturn;
}

//___________________________________/ STOP
CKBOOL ACD::Stop()
{

    DWORD dwReturn;
    if (dwReturn = mciSendCommand(wDeviceID, MCI_STOP, 0, NULL))
        return FALSE;
    m_state = MCI_MODE_STOP;

    return TRUE;
}

//___________________________________/ PAUSE
CKBOOL ACD::Pause()
{

    DWORD dwReturn;
    MCI_STATUS_PARMS mciStatusParms;

    if (m_state == MCI_MODE_PAUSE)
    { // yet paused (or not playing)
        Play();
        m_state = MCI_MODE_PLAY;
        return TRUE;
    }

    if (m_state == MCI_MODE_PLAY)
    { // playing

        mciStatusParms.dwItem = MCI_STATUS_POSITION;
        if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS,
                                      MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms))
            return FALSE;

        int current_time = mciStatusParms.dwReturn;
        int minute = MCI_TMSF_MINUTE(current_time);
        int second = MCI_TMSF_SECOND(current_time);
        int frame = MCI_TMSF_FRAME(current_time);

        first_track = MCI_TMSF_TRACK(current_time);
        start_time = minute * 60000 + second * 1000 + frame * 75 / 1000;

        Stop();
        m_state = MCI_MODE_PAUSE;
        return TRUE;
    }

    return TRUE;
}

//___________________________________/ PLAY
CKBOOL ACD::Play()
{

    if (!wDeviceID)
        OpenDevice();

    DWORD dwReturn;
    MCI_SET_PARMS mciSetParms;
    MCI_PLAY_PARMS mciPlayParms;

    // special tests to know where to start and where to end
    int add;
    if (first_track == end_track)
    {
        if (end_time == 0)
        {
            add = 1;
            if (m_state != MCI_MODE_PAUSE)
                start_time = 0;
        }
        if (end_time > start_time)
            add = 0;
    }
    if (first_track < end_track)
        add = 1;

    // conversion
    int start_m = start_time / 60000;
    int start_s = (start_time - start_m * 60000) / 1000;
    int start_f = (start_time - start_m * 60000 - start_s * 1000) * 1000 / 75;
    int end_m = end_time / 60000;
    int end_s = (end_time - end_m * 60000) / 1000;
    int end_f = (end_time - end_m * 60000 - end_s * 1000) * 1000 / 75;

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
    if (dwReturn = mciSendCommand(wDeviceID, MCI_SET,
                                  MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&mciSetParms))
        return FALSE;

    mciPlayParms.dwFrom = MCI_MAKE_TMSF(first_track, start_m, start_s, start_f);
    mciPlayParms.dwTo = MCI_MAKE_TMSF(end_track + add, end_m, end_s, end_f);
    if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY,
                                  MCI_FROM | MCI_TO, (DWORD)(LPVOID)&mciPlayParms))
        return FALSE;

    m_state = MCI_MODE_PLAY;
    return TRUE;
}
#endif
