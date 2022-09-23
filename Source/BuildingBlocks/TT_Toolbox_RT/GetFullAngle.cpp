///////////////////////////////////
///////////////////////////////////
//
//        TT_GetFullAngle
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetFullAngleDecl();
CKERROR CreateGetFullAngleProto(CKBehaviorPrototype **pproto);
int GetFullAngle(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetFullAngleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetFullAngle");
    od->SetDescription("Calculate angles from 2 2D vectors");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7a18014d, 0x9d2e73));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetFullAngleProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetFullAngleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetFullAngle");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Vector A", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Vector B", CKPGUID_2DVECTOR);

    proto->DeclareOutParameter("Angle RAD", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Angle Degree", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetFullAngle);

    *pproto = proto;
    return CK_OK;
}

int GetFullAngle(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxVector a;
    beh->GetInputParameterValue(0, &a);

    VxVector b;
    beh->GetInputParameterValue(0, &b);

    // TODO

    beh->ActivateOutput(0);
    return CKBR_OK;
}