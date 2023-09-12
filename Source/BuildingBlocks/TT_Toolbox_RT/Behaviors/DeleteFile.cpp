/////////////////////////////////
/////////////////////////////////
//
//        TT_DeleteFile
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorDeleteFileDecl();
CKERROR CreateDeleteFileProto(CKBehaviorPrototype **pproto);
int DeleteFile(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDeleteFileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_DeleteFile");
    od->SetDescription("Delete a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x562f6aab, 0x7ea0f0b));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeleteFileProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDeleteFileProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_DeleteFile");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("File", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeleteFile);

    *pproto = proto;
    return CK_OK;
}

int DeleteFile(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSTRING file = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (::DeleteFileA(file) == TRUE)
    {
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}