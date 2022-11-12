//////////////////////////////////////
//////////////////////////////////////
//
//        TT_PreloadTextures
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorPreloadTexturesDecl();
CKERROR CreatePreloadTexturesProto(CKBehaviorPrototype **pproto);
int PreloadTextures(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPreloadTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_PreloadTextures");
    od->SetDescription("Copies all textures of the current scene to the video memory of the graphics card");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x416b4f0e, 0x192d339a));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePreloadTexturesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreatePreloadTexturesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_PreloadTextures");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("NumberOfTextures", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PreloadTextures);

    *pproto = proto;
    return CK_OK;
}

int PreloadTextures(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    int count = 0;
    int texCount = context->GetObjectsCountByClassID(CKCID_TEXTURE);
    CK_ID *texIds = context->GetObjectsListByClassID(CKCID_TEXTURE);
    for (int i = 0; i < texCount; ++i)
    {
        CKTexture *texture = (CKTexture *)context->GetObject(texIds[i]);
        if (!texture->IsInVideoMemory() && texture->SystemToVideoMemory(behcontext.CurrentRenderContext))
            ++count;
    }
    beh->SetOutputParameterValue(0, &count);

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}