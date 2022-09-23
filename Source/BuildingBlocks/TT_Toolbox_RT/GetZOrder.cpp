////////////////////////////////
////////////////////////////////
//
//        TT_GetZOrder
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetZOrderDecl();
CKERROR CreateGetZOrderProto(CKBehaviorPrototype **pproto);
int GetZOrder(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetZOrderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetZOrder");
    od->SetDescription("Changing the mapping and creating planar mapping");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9163112, 0x1d2b4454));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetZOrderProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateGetZOrderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetZOrder");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Rendering Priority", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetZOrder);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int GetZOrder(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    CK3dObject *target = (CK3dObject *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    int zOrder = 0;
    zOrder = target->GetZOrder();
    beh->SetOutputParameterValue(0, &zOrder);

    return CKBR_OK;
}