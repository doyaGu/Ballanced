/////////////////////////////////
/////////////////////////////////
//
//        TT PushButton
//
/////////////////////////////////
/////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTPushButtonBehaviorProto(CKBehaviorPrototype **pproto);
int TTPushButton(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTPushButtonDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PushButton");
    od->SetDescription("Transforms a 2D Frame into a push button.");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4df00dac,0x562c27da));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTPushButtonBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTPushButtonBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PushButton");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Released");
    proto->DeclareOutput("Roll Over");
    proto->DeclareOutput("Mouse Down");
    proto->DeclareOutput("Mouse Up");

    proto->DeclareInParameter("Released Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Pressed Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("RollOver Material", CKPGUID_MATERIAL);

    proto->DeclareLocalParameter("", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTPushButton);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTPushButton(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTPushButtonCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}