///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT_JoystickVectorMapper
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTJoystickVectorMapperBehaviorProto(CKBehaviorPrototype **pproto);
int TTJoystickVectorMapper(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTJoystickVectorMapperDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_JoystickVectorMapper");
    od->SetDescription("TT_JoystickVectorMapper");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f80efd,0x76b47b33));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTJoystickVectorMapperBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTJoystickVectorMapperBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_JoystickVectorMapper");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Do");

    proto->DeclareOutput("Done");

    proto->DeclareInParameter("InVector", CKPGUID_VECTOR);
    proto->DeclareInParameter("JX+", CKPGUID_INT);
    proto->DeclareInParameter("JX-", CKPGUID_INT);
    proto->DeclareInParameter("JY+", CKPGUID_INT);
    proto->DeclareInParameter("JY-", CKPGUID_INT);

    proto->DeclareOutParameter("OutVector", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTJoystickVectorMapper);

    *pproto = proto;
    return CK_OK;
}

int TTJoystickVectorMapper(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTJoystickVectorMapperCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}