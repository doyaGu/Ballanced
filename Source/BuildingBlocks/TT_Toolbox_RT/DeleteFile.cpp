/////////////////////////////////
/////////////////////////////////
//
//        TT_DeleteFile
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

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
    // TODO: To be finished.
    return CKBR_OK;
}