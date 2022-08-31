////////////////////////////////////
////////////////////////////////////
//
//        TT_GeneratePlane
//
////////////////////////////////////
////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTGeneratePlaneBehaviorProto(CKBehaviorPrototype **pproto);
int TTGeneratePlane(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTGeneratePlaneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GeneratePlane");
    od->SetDescription("generiert Plane");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ae72f89,0x57eb5413));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTGeneratePlaneBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTGeneratePlaneBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GeneratePlane");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Scale", CKPGUID_VECTOR, "1,1,1");
    proto->DeclareInParameter("Size X", CKPGUID_INT, "1");
    proto->DeclareInParameter("Size Y", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTGeneratePlane);

    *pproto = proto;
    return CK_OK;
}

int TTGeneratePlane(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTGeneratePlaneCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}