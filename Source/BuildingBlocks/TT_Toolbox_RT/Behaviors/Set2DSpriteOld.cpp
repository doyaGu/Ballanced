///////////////////////////////////////
///////////////////////////////////////
//
//        TT Set_2DSprite_old
//
///////////////////////////////////////
///////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSet2DSpriteOldDecl();
CKERROR CreateSet2DSpriteOldProto(CKBehaviorPrototype **pproto);
int Set2DSpriteOld(const CKBehaviorContext &behcontext);
CKERROR Set2DSpriteOldCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DSpriteOldDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_2DSprite_old");
    od->SetDescription("Old! Sets position, size and UV mapping for sprite");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56680b62, 0x23083054));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DSpriteOldProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateSet2DSpriteOldProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_2DSprite_old");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("UV Left,Rigth", CKPGUID_2DVECTOR, "0,1");
    proto->DeclareInParameter("UV Top,Bottom", CKPGUID_2DVECTOR, "0,1");

    proto->DeclareSetting("Add Input:", CKPGUID_2DSPRITEINPUT, "UV-Mapping,Size,Position");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set2DSpriteOld);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(Set2DSpriteOldCallBack);

    *pproto = proto;
    return CK_OK;
}

int Set2DSpriteOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR Set2DSpriteOldCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}