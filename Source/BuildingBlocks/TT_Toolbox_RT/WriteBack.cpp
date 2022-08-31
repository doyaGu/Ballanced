////////////////////////////////
////////////////////////////////
//
//        TT WriteBack
//
////////////////////////////////
////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTWriteBackBehaviorProto(CKBehaviorPrototype **pproto);
int TTWriteBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTWriteBackDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT WriteBack");
    od->SetDescription("Schreibt einen Wert in ein dynamisch ersetzten Parameter.");
    od->SetCategory("TT Toolbox/Advanced");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78b31127,0xa875030));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTWriteBackBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTWriteBackBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT WriteBack");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Off");

    proto->DeclareInParameter("Parameter", CKPGUID_INT, "0");
    proto->DeclareInParameter("Value", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTWriteBack);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    *pproto = proto;
    return CK_OK;
}

int TTWriteBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTWriteBackCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}