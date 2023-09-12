//////////////////////////////////////
//////////////////////////////////////
//
//        TT_CreateDirectory
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorCreateDirectoryDecl();
CKERROR CreateCreateDirectoryProto(CKBehaviorPrototype **pproto);
int CreateDirectory(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateDirectoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CreateDirectory");
    od->SetDescription("Creates a Directory");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3442842, 0x1df7a5f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateDirectoryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCreateDirectoryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_CreateDirectory");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("Directory", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CreateDirectory);

    *pproto = proto;
    return CK_OK;
}

int CreateDirectory(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSTRING dir = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (::CreateDirectoryA(dir, NULL) == TRUE)
    {
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}