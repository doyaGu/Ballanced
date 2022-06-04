/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT GetCurvePoint
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorGetCurvePointDecl();
CKERROR CreateGetCurvePointProto(CKBehaviorPrototype **);
int GetCurvePoint(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetCurvePointDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetCurvePoint");
    od->SetDescription("Returns CurvePoint");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x50EB3A17, 0x15C5BF9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurvePointProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    return od;
}

CKERROR CreateGetCurvePointProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetCurvePoint");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Number", CKPGUID_INT);
    
    proto->DeclareOutParameter("CurvePoint", CKPGUID_CURVEPOINT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurvePoint);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_TARGETABLE));

    *pproto = proto;
    return CK_OK;
}

int GetCurvePoint(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}
