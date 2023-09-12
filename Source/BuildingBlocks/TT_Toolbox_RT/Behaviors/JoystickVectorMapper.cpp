///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT_JoystickVectorMapper
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorJoystickVectorMapperDecl();
CKERROR CreateJoystickVectorMapperProto(CKBehaviorPrototype **pproto);
int JoystickVectorMapper(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorJoystickVectorMapperDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_JoystickVectorMapper");
    od->SetDescription("TT_JoystickVectorMapper");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f80efd, 0x76b47b33));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateJoystickVectorMapperProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateJoystickVectorMapperProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_JoystickVectorMapper");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Do");

    proto->DeclareOutput("Done");

    proto->DeclareInParameter("InVector", CKPGUID_VECTOR);
    proto->DeclareInParameter("JX+", CKPGUID_INT);
    proto->DeclareInParameter("JX-", CKPGUID_INT);
    proto->DeclareInParameter("JY+", CKPGUID_INT);
    proto->DeclareInParameter("JY-", CKPGUID_INT);

    proto->DeclareOutParameter("OutVector", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(JoystickVectorMapper);

    *pproto = proto;
    return CK_OK;
}

int JoystickVectorMapper(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}