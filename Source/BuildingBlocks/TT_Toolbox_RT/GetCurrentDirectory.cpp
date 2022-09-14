//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_GetCurrentDirectory
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetCurrentDirectoryDecl();
CKERROR CreateGetCurrentDirectoryProto(CKBehaviorPrototype **pproto);
int GetCurrentDirectory(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetCurrentDirectoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetCurrentDirectory");
    od->SetDescription("Get Current Directory");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x556e6df3, 0x24207d37));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurrentDirectoryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetCurrentDirectoryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetCurrentDirectory");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareOutParameter("CurrentDirectory", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurrentDirectory);

    *pproto = proto;
    return CK_OK;
}

int GetCurrentDirectory(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}