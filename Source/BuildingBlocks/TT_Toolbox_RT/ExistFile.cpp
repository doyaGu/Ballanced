////////////////////////////////
////////////////////////////////
//
//        TT_ExistFile
//
////////////////////////////////
////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTExistFileBehaviorProto(CKBehaviorPrototype **pproto);
int TTExistFile(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTExistFileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ExistFile");
    od->SetDescription("Checks for existing of a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53ba46bc,0x31782f60));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTExistFileBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTExistFileBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ExistFile");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("File", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTExistFile);

    *pproto = proto;
    return CK_OK;
}

int TTExistFile(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTExistFileCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}