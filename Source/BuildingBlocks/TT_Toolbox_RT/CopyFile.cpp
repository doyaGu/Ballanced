///////////////////////////////
///////////////////////////////
//
//        TT_CopyFile
//
///////////////////////////////
///////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTCopyFileBehaviorProto(CKBehaviorPrototype **pproto);
int TTCopyFile(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTCopyFileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CopyFile");
    od->SetDescription("Copy a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x22e510f9,0x70f528a));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTCopyFileBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTCopyFileBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_CopyFile");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Copy");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("Source", CKPGUID_STRING);
    proto->DeclareInParameter("Destination", CKPGUID_STRING);
    proto->DeclareInParameter("Overwrite", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTCopyFile);

    *pproto = proto;
    return CK_OK;
}

int TTCopyFile(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTCopyFileCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}