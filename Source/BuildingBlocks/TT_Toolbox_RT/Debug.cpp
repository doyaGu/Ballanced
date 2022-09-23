////////////////////////////
////////////////////////////
//
//        TT_Debug
//
////////////////////////////
////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorDebugDecl();
CKERROR CreateDebugProto(CKBehaviorPrototype **pproto);
int Debug(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDebugDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Debug");
    od->SetDescription("Debug");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a446c43, 0x66fa2375));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDebugProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDebugProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Debug");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->DeclareInParameter("DebugText", CKPGUID_STRING);
    proto->DeclareInParameter("Console", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Log-File", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Enable", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Debug);

    *pproto = proto;
    return CK_OK;
}

int Debug(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}