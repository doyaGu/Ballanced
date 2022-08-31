//////////////////////////////////
//////////////////////////////////
//
//        TT PushButton2
//
//////////////////////////////////
//////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTPushButton2BehaviorProto(CKBehaviorPrototype **pproto);
int TTPushButton2(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTPushButton2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PushButton2");
    od->SetDescription("push button without material changes");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x14d325d1,0x6748654e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTPushButton2BehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTPushButton2BehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PushButton2");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Released");
    proto->DeclareOutput("Roll Over");
    proto->DeclareOutput("Mouse Down");

    proto->DeclareLocalParameter("", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTPushButton2);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTPushButton2(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTPushButton2CallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}