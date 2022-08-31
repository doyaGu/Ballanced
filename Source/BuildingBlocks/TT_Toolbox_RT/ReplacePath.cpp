//////////////////////////////////
//////////////////////////////////
//
//        TT_ReplacePath
//
//////////////////////////////////
//////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTReplacePathBehaviorProto(CKBehaviorPrototype **pproto);
int TTReplacePath(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTReplacePathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ReplacePath");
    od->SetDescription("Copy a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x524a6bcb,0x66f67774));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTReplacePathBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTReplacePathBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ReplacePath");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Ok");
    proto->DeclareOutput("Failed");

    proto->DeclareInParameter("Old_relative_Path(\..)", CKPGUID_STRING);
    proto->DeclareInParameter("New_relative_Path(\..)", CKPGUID_STRING);
    proto->DeclareInParameter("CurrentWorking_Directory(Drive:\..\..)", CKPGUID_STRING);
    proto->DeclareInParameter("PathIDX(0=Texture/1=Data/2=Sound)", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTReplacePath);

    *pproto = proto;
    return CK_OK;
}

int TTReplacePath(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTReplacePathCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}