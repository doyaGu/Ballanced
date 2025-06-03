/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Blink
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBlinkDecl();
CKERROR CreateBlinkProto(CKBehaviorPrototype **pproto);
int Blink(const CKBehaviorContext &behcontext);
CKERROR BlinkCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBlinkDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Blink");
    od->SetDescription("Blinks the 3D Entity (alternates between show and hide).");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>activates the behavior.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Hidden Frames : </SPAN>number of frames the 3D Entity remains hidden (not rendered).<BR>
    <SPAN CLASS=pin>Shown Frames : </SPAN>number of frames the 3D Entity is shown (rendered).<BR>
    <BR>
    This behavior needs to be looped.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf00d010a, 0xfa0d010a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBlinkProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateBlinkProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Blink");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hidden frames", CKPGUID_INT, "10");
    proto->DeclareInParameter("Shown frames", CKPGUID_INT, "20");

    proto->DeclareLocalParameter("currentFrame", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Blink);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Blink(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the 3d entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKParameterIn *pin = NULL;
    // we get the hidden frames (input parameter)
    int hfnumber;
    beh->GetInputParameterValue(0, &hfnumber);
    if (hfnumber < 0)
        hfnumber = 0;

    // we get the shown frames (input parameter)
    int sfnumber = 10;
    beh->GetInputParameterValue(1, &sfnumber);
    if (sfnumber < 0)
        sfnumber = 0;

    if ((hfnumber + sfnumber) > 0)
    {
        // getting the currentFrames
        int cframe = 20;
        beh->GetLocalParameterValue(0, &cframe);

        if (cframe < hfnumber)
        { // we hide the object
            ent->Show(CKHIDE);
        }
        else
        { // we show the object
            ent->Show(CKSHOW);
        }

        cframe++;
        cframe = cframe % (hfnumber + sfnumber);

        // writing the stepsRemaining
        beh->SetLocalParameterValue(0, &cframe);
    }

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}
