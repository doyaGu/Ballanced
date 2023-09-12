//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_SetCurrentDirectory
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetCurrentDirectoryDecl();
CKERROR CreateSetCurrentDirectoryProto(CKBehaviorPrototype **pproto);
int SetCurrentDirectory(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetCurrentDirectoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetCurrentDirectory");
    od->SetDescription("Set Current Directory");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2d2a0d16, 0x6b955ee1));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetCurrentDirectoryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetCurrentDirectoryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetCurrentDirectory");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("CurrentDirectory", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCurrentDirectory);

    *pproto = proto;
    return CK_OK;
}

int SetCurrentDirectory(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSTRING dir = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (::SetCurrentDirectoryA(dir) == TRUE)
    {
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}