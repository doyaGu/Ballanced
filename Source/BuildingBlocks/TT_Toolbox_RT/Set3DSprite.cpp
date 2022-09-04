///////////////////////////////////
///////////////////////////////////
//
//        TT Set_3DSprite
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSet3DSpriteDecl();
CKERROR CreateSet3DSpriteProto(CKBehaviorPrototype **pproto);
int Set3DSprite(const CKBehaviorContext &behcontext);
CKERROR Set3DSpriteCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet3DSpriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_3DSprite");
    od->SetDescription("stellt Position, Groesse, Offset und UV-Mapping f�r 3D-Sprite ein");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x408e30b5, 0x22a07ada));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet3DSpriteProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSet3DSpriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_3DSprite");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Offset", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("UV Left,Top", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("UV Right,Bottom", CKPGUID_2DVECTOR, "1,1");

    proto->DeclareSetting("Add Input:", CKPGUID_3DSPRITEINPUT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set3DSprite);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(Set3DSpriteCallBack);

    *pproto = proto;
    return CK_OK;
}

int Set3DSprite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR Set3DSpriteCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}