/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Wrap Mode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetWrapModeDecl();
CKERROR CreateSetWrapModeProto(CKBehaviorPrototype **);
int SetWrapMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetWrapModeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Wrap Mode");
    od->SetDescription("Sets the way the texture applied to the mesh will be tiled.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Mode: </SPAN>None / V Looping (loops in V [Vertical on Texture]) / U Looping (loops in U [Horizontal on Texture]) / UV (loop in both directions).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12a5498d, 0x65657512));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetWrapModeProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetWrapModeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Wrap Mode");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Wrap Mode", CKPGUID_WRAPMODE, "None");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetWrapMode);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetWrapMode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKMesh *mesh = ent->GetCurrentMesh();

    int mode = VXTEXTUREWRAP_NONE;
    beh->GetInputParameterValue(0, &mode);

    mesh->SetWrapMode((VXTEXTURE_WRAPMODE)mode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
