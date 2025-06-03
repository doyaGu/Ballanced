/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetListener
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateSetListenerBehaviorProto(CKBehaviorPrototype **pproto);
int SetListener(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetListenerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Listener");
    od->SetDescription("Sets the listener of the sound.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>activates the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Listener: </SPAN>specifies the listener of the sound. NULL sets it to the current camera.<BR>
    */
    od->SetCategory("Sounds/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x28cd6341, 0xcad5793));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetListenerBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(SOUND_MANAGER_GUID);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateSetListenerBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Listener");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Listener", CKPGUID_3DENTITY);

    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetListener);

    *pproto = proto;

    return CK_OK;
}

int SetListener(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSoundManager *sm = (CKSoundManager *)behcontext.Context->GetManagerByGuid(SOUND_MANAGER_GUID);
    if (!sm)
        return CKBR_OK;

    // Set 3D Listener
    CK3dEntity *listener = (CK3dEntity *)beh->GetInputParameterObject(0);

    sm->SetListener(listener);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
