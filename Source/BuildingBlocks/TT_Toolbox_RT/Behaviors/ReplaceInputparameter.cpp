////////////////////////////////////////////
////////////////////////////////////////////
//
//        TT ReplaceInputparameter
//
////////////////////////////////////////////
////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorReplaceInputparameterDecl();
CKERROR CreateReplaceInputparameterProto(CKBehaviorPrototype **pproto);
int ReplaceInputparameter(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReplaceInputparameterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ReplaceInputparameter");
    od->SetDescription("Links an input parameter to another parameter.");
    od->SetCategory("TT Toolbox/Advanced");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x54a776b7, 0x20fc1e7c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReplaceInputparameterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReplaceInputparameterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ReplaceInputparameter");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Replace!");

    proto->DeclareInParameter("Get-Script", CKPGUID_SCRIPT);
    proto->DeclareInParameter("Get-Parametername", CKPGUID_STRING);
    proto->DeclareInParameter("Set-Script", CKPGUID_SCRIPT);
    proto->DeclareInParameter("Set-Parametername", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReplaceInputparameter);

    *pproto = proto;
    return CK_OK;
}

int ReplaceInputparameter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}