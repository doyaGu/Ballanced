//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_SetCurrentDirectory
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSetCurrentDirectoryBehaviorProto(CKBehaviorPrototype **pproto);
int TTSetCurrentDirectory(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSetCurrentDirectoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetCurrentDirectory");
    od->SetDescription("Set Current Directory");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2d2a0d16,0x6b955ee1));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSetCurrentDirectoryBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSetCurrentDirectoryBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetCurrentDirectory");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Fail");

    proto->DeclareInParameter("CurrentDirectory", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSetCurrentDirectory);

    *pproto = proto;
    return CK_OK;
}

int TTSetCurrentDirectory(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSetCurrentDirectoryCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}