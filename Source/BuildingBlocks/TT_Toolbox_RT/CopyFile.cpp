///////////////////////////////
///////////////////////////////
//
//        TT_CopyFile
//
///////////////////////////////
///////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorCopyFileDecl();
CKERROR CreateCopyFileProto(CKBehaviorPrototype **pproto);
int CopyFile(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCopyFileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CopyFile");
    od->SetDescription("Copy a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x22e510f9, 0x70f528a));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCopyFileProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCopyFileProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_CopyFile");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Copy");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("Source", CKPGUID_STRING);
    proto->DeclareInParameter("Destination", CKPGUID_STRING);
    proto->DeclareInParameter("Overwrite", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CopyFile);

    *pproto = proto;
    return CK_OK;
}

int CopyFile(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}