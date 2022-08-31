//////////////////////////////
//////////////////////////////
//
//        TT_ListDir
//
//////////////////////////////
//////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTListDirBehaviorProto(CKBehaviorPrototype **pproto);
int TTListDir(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTListDirDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ListDir");
    od->SetDescription("Checks for existing of a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x496f429e,0x7d602e9d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTListDirBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTListDirBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ListDir");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OUT");

    proto->DeclareInParameter("SearchPath", CKPGUID_STRING);
    proto->DeclareInParameter("Array", CKPGUID_DATAARRAY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTListDir);

    *pproto = proto;
    return CK_OK;
}

int TTListDir(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTListDirCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}