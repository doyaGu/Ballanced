////////////////////////////////////
////////////////////////////////////
//
//        TT GetUV2DSprite
//
////////////////////////////////////
////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetUV2DSpriteDecl();
CKERROR CreateGetUV2DSpriteProto(CKBehaviorPrototype **pproto);
int GetUV2DSprite(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetUV2DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetUV2DSprite");
    od->SetDescription("holt UV-Koordinaten eines 2d-Sprites");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f16689a, 0x472b1c63));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetUV2DSpriteProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateGetUV2DSpriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetUV2DSprite");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareOutParameter("UV-Rect", CKPGUID_RECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetUV2DSprite);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetUV2DSprite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}