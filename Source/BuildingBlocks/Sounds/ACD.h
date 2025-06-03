////
//
//
//  ACD : CD Wrapper
//
//
////

#include "CKTypes.h"

class ACD
{
    // Construction / Destructor
public:
    ACD();
    virtual ~ACD();

    // Attributes
public:
    unsigned int wDeviceID;
    int m_state;
    int first_track;
    int end_track;
    int start_time;
    int end_time;

    // Operations
public:
    CKBOOL OpenDevice(char *name = "cdaudio");
    CKBOOL Play();
    CKBOOL Stop();
    CKBOOL Pause();
    CKBOOL CloseDevice();
    int GetTrackCount();
};
