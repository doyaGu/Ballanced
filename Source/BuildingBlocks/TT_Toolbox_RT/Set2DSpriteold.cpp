///////////////////////////////////////
///////////////////////////////////////
//
//        TT Set_2DSprite_old
//
///////////////////////////////////////
///////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSet2DSpriteoldBehaviorProto(CKBehaviorPrototype **pproto);
int TTSet2DSpriteold(const CKBehaviorContext& behcontext);
CKERROR TTSet2DSpriteoldCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSet2DSpriteoldDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set_2DSprite_old");
    od->SetDescription("Alt! stellt Position, Groesse und UV-Mapping f�r Sprite ein");
    od->SetCategory("TT Toolbox/Sprite");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56680b62,0x23083054));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSet2DSpriteoldBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSet2DSpriteoldBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set_2DSprite_old");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("UV Left,Rigth", CKPGUID_2DVECTOR, "0,1");
    proto->DeclareInParameter("UV Top,Bottom", CKPGUID_2DVECTOR, "0,1");

    proto->DeclareSetting("Add Input:", , "Position,Size,UV-Mapping");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSet2DSpriteold);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TTSet2DSpriteoldCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTSet2DSpriteold(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSet2DSpriteoldCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}