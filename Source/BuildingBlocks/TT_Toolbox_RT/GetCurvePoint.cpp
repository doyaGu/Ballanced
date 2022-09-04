////////////////////////////////////
////////////////////////////////////
//
//        TT GetCurvePoint
//
////////////////////////////////////
////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetCurvePointDecl();
CKERROR CreateGetCurvePointProto(CKBehaviorPrototype **pproto);
int GetCurvePoint(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetCurvePointDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetCurvePoint");
    od->SetDescription("Gibt CurvePoint zur�ck");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x50eb3a17, 0x15c5bf9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurvePointProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetCurvePointProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetCurvePoint");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Number", CKPGUID_INT);

    proto->DeclareOutParameter("CurvePoint", CKPGUID_CURVEPOINT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurvePoint);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int GetCurvePoint(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
	return CK_OK;
}
