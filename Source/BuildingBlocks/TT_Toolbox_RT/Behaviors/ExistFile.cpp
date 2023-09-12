////////////////////////////////
////////////////////////////////
//
//        TT_ExistFile
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorExistFileDecl();
CKERROR CreateExistFileProto(CKBehaviorPrototype **pproto);
int ExistFile(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorExistFileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ExistFile");
    od->SetDescription("Checks for existing of a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53ba46bc, 0x31782f60));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateExistFileProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateExistFileProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ExistFile");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("File", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ExistFile);

    *pproto = proto;
    return CK_OK;
}

int ExistFile(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    
    CKSTRING file = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (::GetFileAttributesA(file) != -1)
    {
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}