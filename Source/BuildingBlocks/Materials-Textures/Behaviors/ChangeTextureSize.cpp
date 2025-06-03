/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              ChangeTextureSize
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorChangeTextureSizeDecl();
CKERROR CreateChangeTextureSizeProto(CKBehaviorPrototype **);
int ChangeTextureSize(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChangeTextureSizeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Change Texture Size");
    od->SetDescription("Change the size of the texture.");

    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7a7c4d7b, 0x468d0f33));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChangeTextureSizeProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateChangeTextureSizeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Change Texture Size");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "64,64");
    proto->DeclareInParameter("Clear", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ChangeTextureSize);

    *pproto = proto;
    return CK_OK;
}

/******************************************************/
/*   Bbehavior Function
/******************************************************/
int ChangeTextureSize(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OWNERERROR;

    // Get Size
    Vx2DVector size(64.0f, 64.0f);
    beh->GetInputParameterValue(0, &size);

    // Clear ?
    CKBOOL clear = TRUE;
    beh->GetInputParameterValue(1, &clear);

    if (!(size.x >= 0 && size.y >= 0))
        return CKBR_PARAMETERERROR;

    tex->FreeVideoMemory();
    if (clear)
    {
        // Just re-create the texture if we do not care for previous pixels
        tex->Create(Near2Power((int)size.x), Near2Power((int)size.y));
    }
    else
    {
        // Resize Texture And Clear
        tex->ResizeImages(Near2Power((int)size.x), Near2Power((int)size.y));
    }

    return CKBR_OK;
}
