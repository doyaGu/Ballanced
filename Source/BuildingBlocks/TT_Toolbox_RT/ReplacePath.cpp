//////////////////////////////////
//////////////////////////////////
//
//        TT_ReplacePath
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorReplacePathDecl();
CKERROR CreateReplacePathProto(CKBehaviorPrototype **pproto);
int ReplacePath(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReplacePathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ReplacePath");
    od->SetDescription("Copy a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x524a6bcb, 0x66f67774));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReplacePathProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReplacePathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ReplacePath");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Ok");
    proto->DeclareOutput("Failed");

    proto->DeclareInParameter("Old_relative_Path(\\..)", CKPGUID_STRING);
    proto->DeclareInParameter("New_relative_Path(\\..)", CKPGUID_STRING);
    proto->DeclareInParameter("CurrentWorking_Directory(Drive:\\..\\..)", CKPGUID_STRING);
    proto->DeclareInParameter("PathIDX(0=Texture/1=Data/2=Sound)", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReplacePath);

    *pproto = proto;
    return CK_OK;
}

int ReplacePath(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}