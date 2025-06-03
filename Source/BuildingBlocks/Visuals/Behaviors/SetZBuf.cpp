/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetZBuf
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetZBufDecl();
CKERROR CreateSetZBufProto(CKBehaviorPrototype **pproto);
int SetZBuf(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetZBufDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Z Buffer");
    od->SetDescription("Change the way the object read and write into the ZBuffer.");
    /* rem:
  <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
  <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Read: </SPAN>boolean value to check if you want to enable the 3dobject to read into the zbuf when it draws itself.<BR>
    <SPAN CLASS=pin>Write: </SPAN>boolean value to check if you want to enable the 3dobject to write into the zbuf when it draws itself.<BR>
    <BR>
    */
    /* warning:
    - This building block sets a flag in the object state, therefore, as for 'set diffuse' or building block like that, once it's done, the object will remain in the new state.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1d2d378d, 0x4a4f36b6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetZBufProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateSetZBufProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Z Buffer");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Read", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Write", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetZBuf);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetZBuf(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dObject *obj = (CK3dObject *)beh->GetTarget();
    if (!obj)
        return CKBR_OWNERERROR;

    CKBOOL enable_read = TRUE;
    beh->GetInputParameterValue(0, &enable_read);

    CKBOOL enable_write = TRUE;
    beh->GetInputParameterValue(1, &enable_write);

    CKDWORD flags = obj->GetMoveableFlags();

    if (enable_read)
        flags &= ~VX_MOVEABLE_NOZBUFFERTEST;
    else
        flags |= VX_MOVEABLE_NOZBUFFERTEST;

    if (enable_write)
        flags &= ~VX_MOVEABLE_NOZBUFFERWRITE;
    else
        flags |= VX_MOVEABLE_NOZBUFFERWRITE;

    obj->SetMoveableFlags(flags);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
