///////////////////////////////////
///////////////////////////////////
//
//        TT Set_2DSprite
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSet2DSpriteDecl();
CKERROR CreateSet2DSpriteProto(CKBehaviorPrototype **pproto);
int Set2DSprite(const CKBehaviorContext &behcontext);
CKERROR Set2DSpriteCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_2DSprite");
    od->SetDescription("stellt Position, Groesse und UV-Mapping f�r Sprite ein");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3c392558, 0x419d2680));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DSpriteProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateSet2DSpriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_2DSprite");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("UV-Rect", CKPGUID_RECT, "0,0,1,1");

    proto->DeclareSetting("Add Input:", CKPGUID_2DSPRITEINPUT, "Position,Size,UV-Mapping");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set2DSprite);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(Set2DSpriteCallBack);

    *pproto = proto;
    return CK_OK;
}

int Set2DSprite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR Set2DSpriteCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}