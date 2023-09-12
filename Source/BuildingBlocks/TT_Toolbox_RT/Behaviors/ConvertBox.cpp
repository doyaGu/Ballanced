/////////////////////////////////
/////////////////////////////////
//
//        TT_ConvertBox
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorConvertBoxDecl();
CKERROR CreateConvertBoxProto(CKBehaviorPrototype **pproto);
int ConvertBox(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorConvertBoxDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ConvertBox");
    od->SetDescription("Generates a bounding box from 2 vectors");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x694867ff, 0x7db16d1f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateConvertBoxProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateConvertBoxProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ConvertBox");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Vector Min", CKPGUID_VECTOR);
    proto->DeclareInParameter("Vector Max", CKPGUID_VECTOR);

    proto->DeclareOutParameter("BBox", CKPGUID_BOX);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ConvertBox);

    *pproto = proto;
    return CK_OK;
}

int ConvertBox(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxVector min;
    beh->GetInputParameterValue(0, &min);

    VxVector max;
    beh->GetInputParameterValue(0, &max);

    VxBbox bbox(min, max);
    beh->SetOutputParameterValue(0, &bbox);

    beh->ActivateOutput(0);
    return CKBR_OK;
}