/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PositionSound
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreatePositionSoundBehaviorProto(CKBehaviorPrototype **pproto);
int DoPositionSound(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPositionSoundDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Position Sound");
    od->SetDescription("Attaches a sound to an object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>object to which the sound will be attached (NULL = world).<BR>
    <SPAN CLASS=pin>Position: </SPAN>position of the sound, in the referential of the attached object, or in the world.<BR>
    <SPAN CLASS=pin>Direction: </SPAN>direction of the sound, in the referential of the attached object, or in the world.<BR>
    <SPAN CLASS=pin>Relative To Listener: </SPAN>specify if you want the sound to be positionned relative to the listener (when the listener will move the sound will move with it).<BR>
    <BR>
    If the object is different from NULL, the sound will follow it.<BR>
    <BR>
    */
    od->SetCategory("Sounds/3D Properties");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4ba42a5b, 0x3ca030dd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePositionSoundBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreatePositionSoundBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Position Sound");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Direction", CKPGUID_VECTOR, "0,0,1");
    //	proto->DeclareInParameter("Relative To Listener",CKPGUID_BOOL,"FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DoPositionSound);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int DoPositionSound(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    CK3dEntity *object = (CK3dEntity *)beh->GetInputParameterObject(0);

    VxVector pos(0, 0, 0), dir(0, 0, 1);
    // Position
    beh->GetInputParameterValue(1, &pos);
    // Orientation
    beh->GetInputParameterValue(2, &dir);

    // The core
    wave->PositionSound(object, &pos, &dir);
    /*
        CKBOOL headr = FALSE;
        beh->GetInputParameterValue(3,&headr);
        wave->SetHeadRelative(headr);
    */
    return CKBR_OK;
}
