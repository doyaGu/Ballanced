//////////////////////////////////////
//////////////////////////////////////
//
//        TT_RemoveDirectory
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorRemoveDirectoryDecl();
CKERROR CreateRemoveDirectoryProto(CKBehaviorPrototype **pproto);
int RemoveDirectory(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveDirectoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_RemoveDirectory");
    od->SetDescription("Remove Directory");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7a2105fa, 0x282d2a22));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveDirectoryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRemoveDirectoryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_RemoveDirectory");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("Directory", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RemoveDirectory);

    *pproto = proto;
    return CK_OK;
}

int RemoveDirectory(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSTRING dir = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (::RemoveDirectoryA(dir) == TRUE)
    {
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}