/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              SetMipMapLevel
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetMipMapLevelDecl();
CKERROR CreateSetMipMapLevelProto(CKBehaviorPrototype **);
int SetMipMapLevel(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetMipMapLevelDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set MipMap Level");
    od->SetDescription("Set the MipMap level of the texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Use MipMapping: </SPAN>if true mipmapping will be used for this texture.
    <BR>
    */
    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3ebe40f2, 0x3fa41377));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetMipMapLevelProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateSetMipMapLevelProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set MipMap Level");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Use MipMapping", CKPGUID_BOOL, "TRUE");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetMipMapLevel);

    *pproto = proto;
    return CK_OK;
}

int SetMipMapLevel(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CK_OK;

    CKBOOL activate = TRUE;
    beh->GetInputParameterValue(0, &activate);

    tex->UseMipmap(activate);

    return CKBR_OK;
}
