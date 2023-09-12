/////////////////////////////////
/////////////////////////////////
//
//        TT PushButton
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorPushButtonDecl();
CKERROR CreatePushButtonProto(CKBehaviorPrototype **pproto);
int PushButton(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPushButtonDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PushButton");
    od->SetDescription("Transforms a 2D Frame into a push button.");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4df00dac, 0x562c27da));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePushButtonProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreatePushButtonProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PushButton");
    if (!proto) return CKERR_OUTOFMEMORY;

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
    proto->SetFunction(PushButton);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int PushButton(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}