////////////////////////////////////
////////////////////////////////////
//
//        TT GetUV2DSprite
//
////////////////////////////////////
////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTGetUV2DSpriteBehaviorProto(CKBehaviorPrototype **pproto);
int TTGetUV2DSprite(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTGetUV2DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetUV2DSprite");
    od->SetDescription("holt UV-Koordinaten eines 2d-Sprites");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f16689a,0x472b1c63));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTGetUV2DSpriteBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTGetUV2DSpriteBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetUV2DSprite");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareOutParameter("UV-Rect", CKPGUID_RECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTGetUV2DSprite);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTGetUV2DSprite(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTGetUV2DSpriteCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}