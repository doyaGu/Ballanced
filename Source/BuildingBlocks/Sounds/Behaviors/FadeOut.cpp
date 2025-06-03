/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FadeOut
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateFadeOutBehaviorProto(CKBehaviorPrototype **pproto);
int FadeOut(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFadeOutDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Fade Out");
    od->SetDescription("Fades out sound(s).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Fade Delay: </SPAN>time in milliseconds during which the sound(s) are to be faded out.<BR>
    <BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>time elapsed in miliseconds since the start of the fade out.<BR>
    <BR>
    This behavior should be used in conjunction with another building block, such as Wave Player.<BR>
    */
    od->SetCategory("Sounds/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1b82338f, 0x5a656975));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFadeOutBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(SOUND_MANAGER_GUID);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateFadeOutBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Fade Out");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Fade Delay", CKPGUID_INT, "1000");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_FLOAT, "0");

    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(FadeOut);

    *pproto = proto;

    return CK_OK;
}

int FadeOut(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSoundManager *sm = (CKSoundManager *)behcontext.Context->GetManagerByGuid(SOUND_MANAGER_GUID);
    if (!sm)
        return CKBR_OK;

    float timeelapsed = 0;

    int ti = 1000;
    beh->GetInputParameterValue(0, &ti);
    float totaltime = (float)ti;

    if (beh->IsInputActive(0))
    { // we trigger the behavior for the first time
        beh->ActivateInput(0, FALSE);

        beh->SetOutputParameterValue(0, &timeelapsed);

        CKListenerSettings set;
        set.m_GlobalGain = 1.0f;
        sm->UpdateListenerSettings(CK_LISTENERSETTINGS_GAIN, set);
    }
    else
    {
        beh->GetOutputParameterValue(0, &timeelapsed);
        timeelapsed += behcontext.DeltaTime;
        if (timeelapsed > totaltime)
            timeelapsed = totaltime;
        beh->SetOutputParameterValue(0, &timeelapsed);

        CKListenerSettings set;
        set.m_GlobalGain = 1.0f - timeelapsed / totaltime;
        sm->UpdateListenerSettings(CK_LISTENERSETTINGS_GAIN, set);

        if (timeelapsed == totaltime)
        { // the fade is finished
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}
