///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT Set Dynamic Position
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSetDynamicPositionDecl();
CKERROR CreateSetDynamicPositionProto(CKBehaviorPrototype **pproto);
int SetDynamicPosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetDynamicPositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Dynamic Position");
    od->SetDescription("Follows an 3D-Object smoothly.");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfd4755f, 0x7de22dc8));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetDynamicPositionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetDynamicPositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Dynamic Position");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("On");
    proto->DeclareOutput("Off");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Force X", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Force Y", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Force Z", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Damping X", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Damping Y", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Damping Z", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Offset X", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Offset Y", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Offset Z", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Coordinate-System", CKPGUID_3DENTITY, "NULL");
    proto->DeclareInParameter("MaxDistance2Target", CKPGUID_FLOAT, "0.0");

    proto->DeclareOutParameter("OldPosition", CKPGUID_VECTOR);
    proto->DeclareOutParameter("DeltaTarget", CKPGUID_VECTOR);
    proto->DeclareOutParameter("NewPosition", CKPGUID_VECTOR);
    proto->DeclareOutParameter("fDist", CKPGUID_FLOAT);

    proto->DeclareLocalParameter("Status", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter("Old Position", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetDynamicPosition);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetDynamicPosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}