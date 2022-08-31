//////////////////////////////////////
//////////////////////////////////////
//
//        TT_IsObjectVisible
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTIsObjectVisibleBehaviorProto(CKBehaviorPrototype **pproto);
int TTIsObjectVisible(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTIsObjectVisibleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_IsObjectVisible");
    od->SetDescription("testet ob Objekt sichtbar ist");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x39cf3dd3,0x11a92281));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTIsObjectVisibleBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTIsObjectVisibleBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_IsObjectVisible");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTIsObjectVisible);

    *pproto = proto;
    return CK_OK;
}

int TTIsObjectVisible(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTIsObjectVisibleCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}