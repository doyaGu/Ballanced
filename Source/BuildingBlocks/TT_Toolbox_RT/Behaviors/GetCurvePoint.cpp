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
    od->SetDescription("Get Curve Point");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x50eb3a17, 0x15c5bf9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurvePointProto);
    od->SetCompatibleClassId(CKCID_CURVE);
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

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    CKCurve *curve = (CKCurve *)beh->GetTarget();
    if (!curve)
        return CKBR_OWNERERROR;

    int number = 0;
    beh->GetInputParameterValue(0, &number);

    CKCurvePoint *point = curve->GetControlPoint(number);
    if (!point)
    {
        context->OutputToConsole("no CurvePoint!");
        return CK_OK;
    }

    VxVector pos;
    point->GetPosition(&pos);
    point->SetName("Curvepoints", TRUE);

    beh->SetOutputParameterObject(0, point);
	return CK_OK;
}
