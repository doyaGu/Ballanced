//***********************************************************************************************************
//
//  Sound Loader using Threads
//
//***********************************************************************************************************
#include "CKAll.h"

#ifdef WIN32
    #include <Windows.h>
    #define THREADED_LOADING
#endif

CKERROR CreateSoundLoaderProto(CKBehaviorPrototype **pproto);
int SoundLoader(const CKBehaviorContext &context);
CKERROR SoundLoaderCB(const CKBehaviorContext &behcontext);
CKERROR SoundLoaderCallback(const CKBehaviorContext &behcontext);

#ifdef THREADED_LOADING
unsigned int SoundLoaderThreadFunction(void *arg);

typedef struct LoadSoundThreadInfo
{
    XString SoundFile;
    CKBOOL Loaded;
    CKSound *Sound;
    CKBOOL Stop;
    CKContext *Context;
    CKSoundReader *sreader;
    CKBOOL Error;
    CKBOOL SoundStreamed;
    XArray<CKBYTE> SoundBuffer;
    int BufferSize;

    LoadSoundThreadInfo()
    {
        Loaded = 0;
        Sound = 0;
        Stop = 0;
        Context = 0;
        sreader = 0;
        Error = 0;
        SoundStreamed = 0;
        BufferSize = 0;
    };

} LoadSoundThreadInfo;

CKBOOL LoadBufferToCKSound(LoadSoundThreadInfo *LoadInfo, CKSound *sound);
#endif

CKObjectDeclaration *FillSoundLoaderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sound Load");
    od->SetDescription("Loads a sound file into a sound object");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Failed: </SPAN>is activated when the process has failed.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>name of the local file to read the sound from.<BR>
    <SPAN CLASS=pin>Streamed?: </SPAN>should the sound be streamed or enterily loaded in memory.<BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2bc537c, 0x147c22e0));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSoundLoaderProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    return od;
}

CKERROR CreateSoundLoaderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sound Load");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Failed");

    proto->DeclareInParameter("File", CKPGUID_STRING);
    proto->DeclareInParameter("Streamed ?", CKPGUID_BOOL);

    proto->DeclareLocalParameter("Loading Sound Thread", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Loading Info", CKPGUID_POINTER);
    proto->DeclareSetting("Use asynchonized loading", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTS));

    proto->SetFunction(SoundLoader);
    proto->SetBehaviorCallbackFct(SoundLoaderCallback, (CK_BEHAVIOR_CALLBACKMASK)CKCB_BEHAVIORRESET | CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORDETACH);

    *pproto = proto;
    return CK_OK;
}

int SoundLoader(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
#ifdef THREADED_LOADING
    LoadSoundThreadInfo *LoadInfo;
    VxThread *VXT;
#endif
    CKWaveSound *snd = (CKWaveSound *)beh->GetTarget();
    CKERROR res = CKERR_INVALIDOBJECT;

    if (!snd)
    {
        beh->ActivateOutput(1);
        return CKBR_PARAMETERERROR;
    }

    CKBOOL UseThread = FALSE;
#ifdef THREADED_LOADING
    beh->GetLocalParameterValue(2, &UseThread);
#endif

    if (beh->IsInputActive(0)) // IN
    {
        beh->ActivateInput(0, FALSE);
        XString fname((CKSTRING)beh->GetInputParameterReadDataPtr(0));
        ctx->GetPathManager()->ResolveFileName(fname, SOUND_PATH_IDX);

        CKBOOL LocalFile = FALSE;

        if (fname.Length() > 3)
        {
            if (fname[1] == ':' && fname[2] == '\\') // c:\\ style
            {
                LocalFile = TRUE;
            }
            else if ((strncmp(fname.Str(), "file:", 5) == 0) || strncmp(fname.Str(), "FILE:", 5) == 0) // File Protocol
            {
                LocalFile = TRUE;
            }
            else if (strncmp(fname.Str(), "\\\\", 2) == 0) // Lan drive
            {
                LocalFile = TRUE;
            }
        }


        if (!UseThread)
        {
            // -----------------------------------------------------------------------------
            //  Do not use thread
            // -----------------------------------------------------------------------------
            // Loading
            if (snd)
            {
                snd->Stop();
                if (snd->SetSoundFileName(fname.Str()) != CK_OK)
                {
                    res = CKERR_INVALIDFILE;
                }
                else
                {
                    CKBOOL b;
                    beh->GetInputParameterValue(1, &b);
                    res = snd->SetFileStreaming(b, TRUE);
                }
            }
            if (res == CK_OK)
                beh->ActivateOutput(0);
            else
                beh->ActivateOutput(1);
            return CKBR_OK;
        }
#ifdef THREADED_LOADING
        else
        {
            // -----------------------------------------------------------------------------
            //  Use Thread
            // -----------------------------------------------------------------------------
            beh->GetLocalParameterValue(0, &VXT);
            if (!VXT) // Start a Thread if needed
            {
                VXT = new VxThread();
                VXT->SetName("Sound Loader Thread");
                VXT->SetPriority(VXTP_NORMAL);
            }

            beh->GetLocalParameterValue(1, &LoadInfo);
            if (!LoadInfo)
            {
                LoadInfo = new LoadSoundThreadInfo;
            }

            // fill loadInfo structure
            LoadInfo->SoundFile = fname;
            LoadInfo->Sound = (CKSound *)beh->GetTarget();
            LoadInfo->Context = behcontext.Context;
            LoadInfo->sreader = NULL;
            LoadInfo->Loaded = FALSE;
            LoadInfo->Stop = FALSE;
            LoadInfo->Error = FALSE;
            LoadInfo->BufferSize = snd->m_SoundManager->GetStreamedBufferSize();

            // -------------------------------------------------------------------------------------------
            // normalement, cette partie se fait avant le open (dans le code)
            //
            CKWaveSound *wave = (CKWaveSound *)LoadInfo->Sound;
            // dans le setfilestreaming
            CKBOOL EnableStream;
            beh->GetInputParameterValue(1, &EnableStream);
            if (EnableStream)
                wave->m_State |= CK_WAVESOUND_FILESTREAMED;

            LoadInfo->SoundStreamed = EnableStream;

            // dans le recreate
            CKWaveSoundSettings setting;
            CKWaveSound3DSettings setting3D;
            if (wave->m_Source)
            {
                wave->m_SoundManager->UpdateSettings(wave->m_Source, CK_WAVESOUND_SETTINGS_ALL, setting, FALSE);
                wave->m_SoundManager->Update3DSettings(wave->m_Source, CK_WAVESOUND_3DSETTINGS_ALL, setting3D, FALSE);
            }

            // We release the current source
            wave->Release();

            // If no filename are provided, we exit
            // if(!wave->m_FileName) return CKERR_INVALIDPARAMETER;

            // if the file must be streamed from file by a reader
            // We reset the buffer position
            wave->m_BufferPos = -1;

            /// ------------------------------------------------------------------------------------------

            beh->SetLocalParameterValue(0, &VXT, sizeof(VxThread *));
            beh->SetLocalParameterValue(1, &LoadInfo, sizeof(LoadSoundThreadInfo *));

            if (!VXT->IsCreated())
                VXT->CreateThread(SoundLoaderThreadFunction, LoadInfo);
            return CKBR_ACTIVATENEXTFRAME;
        }
#endif
    }
    if (beh->IsInputActive(1)) // Kill thread
    {
#ifdef THREADED_LOADING
        beh->GetLocalParameterValue(0, &VXT);
        beh->GetLocalParameterValue(1, &LoadInfo);
        if (VXT)
        {
            LoadInfo->Stop = TRUE;
            VXT->Wait();
            delete VXT;
            VXT = NULL;
            beh->SetLocalParameterValue(0, &VXT);
        }

        if (LoadInfo)
        {
            delete LoadInfo;
            LoadInfo = NULL;
            beh->SetLocalParameterValue(1, &LoadInfo);
        }
#endif
        return CKBR_OK;
    }

    // no activation, check if finished (for thread)
    else
    {
#ifdef THREADED_LOADING
        if (UseThread)
        {
            beh->GetLocalParameterValue(0, &VXT);
            beh->GetLocalParameterValue(1, &LoadInfo);
            if (LoadInfo->Loaded) // the sound is loaded
            {
                if (!LoadInfo->Error)
                {
                    LoadBufferToCKSound(LoadInfo, LoadInfo->Sound);
                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }
                else
                {
                    beh->ActivateOutput(1, TRUE);
                    return CKBR_OK;
                }
            }
        }
#endif
    }

    return CKBR_ACTIVATENEXTFRAME;
}

//
// Thread Function
//
#ifdef THREADED_LOADING
unsigned int SoundLoaderThreadFunction(void *arg)
{
    LoadSoundThreadInfo *LoadInfo = (LoadSoundThreadInfo *)arg;

    while (!LoadInfo->Stop)
    {
        if (!LoadInfo->Loaded)
        {
            // parcours des readers possibles pour trouver le bon
            CKPathSplitter sp(LoadInfo->SoundFile.Str());
            CKFileExtension ext(sp.GetExtension());
            LoadInfo->sreader = CKGetPluginManager()->GetSoundReader(ext);
            // No reader exist for this type of sound
            if (!LoadInfo->sreader)
            {
                LoadInfo->Error = TRUE;
            }
            else
            {
                if (LoadInfo->sreader->OpenFile(LoadInfo->SoundFile.Str()) != CK_OK)
                {
                    LoadInfo->Error = TRUE;
                }
                else
                {
                    // LoadInfo->sreader->Seek(0);

                    if (!LoadInfo->SoundStreamed)
                    {
                        int sz = (int)(LoadInfo->sreader->GetDataSize());
                        if (sz)
                        {
                            LoadInfo->SoundBuffer.Resize(sz);
                            int pos = 0;
                            // we write the totality of the sound in the buffer
                            LoadInfo->sreader->Play();
                            CKDWORD res;
                            while ((res = LoadInfo->sreader->Decode()) != CKSOUND_READER_EOF && !(LoadInfo->Stop))
                            {
                                if (res == CK_OK)
                                {
                                    CKBYTE *buf = NULL;
                                    int bufsize = 0;
                                    // We read the data from the reader
                                    if (LoadInfo->sreader->GetDataBuffer(&buf, &bufsize) == CK_OK)
                                    {
                                        if (pos + bufsize > LoadInfo->SoundBuffer.Size())
                                        {
                                            LoadInfo->SoundBuffer.Resize(pos + bufsize);
                                        }
                                        memcpy(&LoadInfo->SoundBuffer[pos], buf, bufsize);
                                        pos += bufsize;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        CKWaveFormat wfe;
                        LoadInfo->sreader->GetWaveFormat(&wfe);
                        CKDWORD buffersize = (int)((float)LoadInfo->BufferSize * (float)wfe.nAvgBytesPerSec * 0.001f);
                        int sz = __min((unsigned int)(LoadInfo->sreader->GetDataSize()), buffersize);
                        if (sz)
                        {
                            LoadInfo->SoundBuffer.Resize(sz);
                            int pos = 0;
                            // we write the totality of the sound in the buffer
                            LoadInfo->sreader->Play();
                            CKDWORD res;
                            while ((res = LoadInfo->sreader->Decode()) != CKSOUND_READER_EOF && !(LoadInfo->Stop))
                            {
                                if (res == CK_OK)
                                {
                                    CKBYTE *buf = NULL;
                                    int bufsize = 0;
                                    // We read the data from the reader
                                    if (LoadInfo->sreader->GetDataBuffer(&buf, &bufsize) == CK_OK)
                                    {
                                        if (pos + bufsize > LoadInfo->SoundBuffer.Size())
                                        {
                                            LoadInfo->SoundBuffer.Resize(pos + bufsize);
                                        }

                                        memcpy(&LoadInfo->SoundBuffer[pos], buf, bufsize);
                                        pos += bufsize;
                                    }

                                    if (pos >= sz)
                                        break;
                                }
                            }
                        }
                    }
                    LoadInfo->Error = FALSE;
                }
            }
            LoadInfo->Loaded = TRUE;
        }
        else
            Sleep(100);
    }

    return 0x00001234;
}
#endif

//
//  CallBack
//
CKERROR SoundLoaderCallback(const CKBehaviorContext &behcontext)
{
#ifdef THREADED_LOADING
    VxThread *VXT = NULL;
    LoadSoundThreadInfo *LoadInfo = NULL;
#endif
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDETACH:
    {
#ifdef THREADED_LOADING
        beh->GetLocalParameterValue(0, &VXT);
        beh->GetLocalParameterValue(1, &LoadInfo);
        if (VXT)
        {
            if (LoadInfo)
            {
                LoadInfo->Stop = TRUE;
            }
            VXT->Wait();
            delete VXT;
            VXT = NULL;
            beh->SetLocalParameterValue(0, &VXT);
        }
        if (LoadInfo)
        {
            delete LoadInfo;
            LoadInfo = NULL;
            beh->SetLocalParameterValue(1, &LoadInfo);
        }
#endif
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL UseThread = FALSE;
        beh->GetLocalParameterValue(2, &UseThread);
        if (UseThread)
        {
            if (beh->GetInputCount() == 1)
                beh->CreateInput("Kill Thread");
        }
        else
        {
            if (beh->GetInputCount() == 2)
                beh->RemoveInput(1);
        }
    }
    break;
    }
    return CKBR_OK;
}

#ifdef THREADED_LOADING

CKBOOL LoadBufferToCKSound(LoadSoundThreadInfo *LoadInfo, CKSound *sound)
{
    // open file
    CKWaveSoundSettings setting;

    CKSoundReader *SoundReader = LoadInfo->sreader;

    CKWaveSound *wave = (CKWaveSound *)sound;

    wave->m_SoundReader = SoundReader;

    CKWaveSound3DSettings setting3D;

    SoundReader->GetWaveFormat(&wave->m_WaveFormat);

    // We clear some runtime data and flags
    wave->m_BufferPos = -1;
    wave->m_DataRead = 0;
    wave->m_DataPlayed = 0;
    wave->m_OldCursorPos = 0;
    wave->m_State &= ~(CK_WAVESOUND_STREAMOVERLAP | CK_WAVESOUND_STREAMFULLYLOADED | CK_WAVESOUND_NEEDREWIND);

    wave->m_DataToRead = SoundReader->GetDataSize();

    int duration = 0;
    duration = SoundReader->GetDuration();

    if (wave->m_DataToRead <= 0) // the reader was unable to calculate sound size
    {
        if (wave->m_Duration > 0)
            wave->m_DataToRead = (int)((float)wave->m_WaveFormat.nAvgBytesPerSec * (float)wave->m_Duration * 0.001f); // Caculate from saved duration.
        else
            return CKERR_INVALIDPARAMETER; // Unable to get the sound size !
    }

    if (duration > 0)
        wave->m_Duration = duration;

    CKDWORD buffersize = (int)((float)wave->m_SoundManager->GetStreamedBufferSize() * (float)wave->m_WaveFormat.nAvgBytesPerSec * 0.001f);

    if (wave->GetFileStreaming())
        wave->m_BufferSize = __min((int)buffersize, (int)wave->m_DataToRead);
    else
        wave->m_BufferSize = wave->m_DataToRead;

    wave->m_BufferSize = __max(wave->m_BufferSize, LoadInfo->SoundBuffer.Size());

    // We create the source
    wave->m_Source = wave->m_SoundManager->CreateSource(CK_WAVESOUND_TYPE(wave->m_State & CK_WAVESOUND_ALLTYPE), &wave->m_WaveFormat, wave->m_BufferSize, wave->GetFileStreaming());
    if (!wave->m_Source)
        return CKERR_INVALIDPARAMETER;

    // Restore values of the source
    wave->m_SoundManager->UpdateSettings(wave->m_Source, CK_WAVESOUND_SETTINGS_ALL, setting);
    // Restore values of the source
    wave->m_SoundManager->Update3DSettings(wave->m_Source, CK_WAVESOUND_3DSETTINGS_ALL, setting3D);

    if (wave->GetFileStreaming() && (wave->m_BufferSize < wave->m_DataToRead)) // only a part of the sound can be in the buffer
    {
        // We calculate the size of the buffer to allocate
        if (LoadInfo->SoundBuffer.Size())
        {
            wave->WriteData(LoadInfo->SoundBuffer.Begin(), LoadInfo->SoundBuffer.Size());
            LoadInfo->SoundBuffer.Clear();
        }
        else
        {
            VxTimeProfiler tpf;
            wave->WriteDataFromReader();

            {
                float tp = tpf.Current();
                char str[256];
                sprintf(str, "%f\n", tp);
                OutputDebugString(str);
            }
        }
    }
    else
    { // The sound can be put entirely in the buffer
        wave->m_State |= CK_WAVESOUND_STREAMFULLYLOADED;

        if (LoadInfo->SoundBuffer.Size())
        {
            wave->WriteData(LoadInfo->SoundBuffer.Begin(), LoadInfo->SoundBuffer.Size());
            LoadInfo->SoundBuffer.Clear();
        }
        else
        {
            // we write the totality of the sound in the buffer
            wave->m_SoundReader->Play();
            CKDWORD res;
            while ((res = wave->m_SoundReader->Decode()) != CKSOUND_READER_EOF)
            {
                if (res == CK_OK)
                {
                    CKBYTE *buf = NULL;
                    int bufsize = 0;
                    // We read the data from the reader
                    if (wave->m_SoundReader->GetDataBuffer(&buf, &bufsize) == CK_OK)
                        wave->WriteData(buf, bufsize);
                }
            }
        }
    }

#ifdef _CKEXCEPTION
} // Try
catch (...)
{
    char ErrorMsg[256];
    sprintf(ErrorMsg, "SoundReader Error : %s", m_FileName);
    wave->m_Context->OutputToConsole(ErrorMsg);
    wave->m_SoundReader->Release();
    wave->m_SoundReader = NULL;
    wave->Release();

    return CKERR_INVALIDPARAMETER;
} // catch
#endif

return CKBR_OK;
}
#endif