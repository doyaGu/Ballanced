/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT GetNearestCurvePosition
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorGetNearestCurvePositionDecl();
CKERROR CreateGetNearestCurvePositionProto(CKBehaviorPrototype **);
int GetNearestCurvePosition(const CKBehaviorContext &behcontext);
CKERROR GetNearestCurvePositionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetNearestCurvePositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetNearestCurvePosition");
    od->SetDescription("");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4DB6E14, 0x2C151F73));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetNearestCurvePositionProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateGetNearestCurvePositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetNearestCurvePosition");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Curve", CKPGUID_CURVE);
    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0.0,0.0,0.0");

    proto->DeclareOutParameter("Position", CKPGUID_VECTOR, "0.0,0.0,0.0");
    proto->DeclareOutParameter("Step", CKPGUID_FLOAT, "0.0");

    proto->DeclareSetting("Prefered Height Delta", CKPGUID_FLOAT, "2.5");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetNearestCurvePosition);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(GetNearestCurvePositionCallBack);

    *pproto = proto;
    return CK_OK;
}

int GetNearestCurvePosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    // TODO

    beh->ActivateOutput(0);
    return CKBR_OK;
}

CKERROR GetNearestCurvePositionCallBack(const CKBehaviorContext &behcontext)
{
    return CKBR_OK;
}
