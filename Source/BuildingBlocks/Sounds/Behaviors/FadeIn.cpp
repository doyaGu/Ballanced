/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FadeIn
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateFadeInBehaviorProto(CKBehaviorPrototype **pproto);
int FadeIn(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFadeInDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Fade In");
    od->SetDescription("Fades in sound(s).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Fade Delay: </SPAN>time in milliseconds during which the sound(s) are to be faded in.<BR>
    <BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>time elapsed in miliseconds since the start of the fade in.<BR>
    <BR>
    This behavior should be used in conjunction with another building block, such as Wave Player.<BR>
    */
    od->SetCategory("Sounds/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x742215ea, 0x8a06c46));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFadeInBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(SOUND_MANAGER_GUID);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateFadeInBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Fade In");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Fade Delay", CKPGUID_INT, "1000");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_FLOAT, "0");

    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(FadeIn);

    *pproto = proto;

    return CK_OK;
}

int FadeIn(const CKBehaviorContext &behcontext)
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
        set.m_GlobalGain = 0.0f;
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
        set.m_GlobalGain = timeelapsed / totaltime;
        sm->UpdateListenerSettings(CK_LISTENERSETTINGS_GAIN, set);

        if (timeelapsed == totaltime)
        { // the fade is finished
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}
