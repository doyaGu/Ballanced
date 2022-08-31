//////////////////////////////////////
//////////////////////////////////////
//
//        TT_CreateDirectory
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTCreateDirectoryBehaviorProto(CKBehaviorPrototype **pproto);
int TTCreateDirectory(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTCreateDirectoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CreateDirectory");
    od->SetDescription("Creates a Directory");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3442842,0x1df7a5f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTCreateDirectoryBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTCreateDirectoryBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_CreateDirectory");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("Directory", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTCreateDirectory);

    *pproto = proto;
    return CK_OK;
}

int TTCreateDirectory(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTCreateDirectoryCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}