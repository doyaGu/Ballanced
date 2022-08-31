//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_GetFileNameFromPath
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTGetFileNameFromPathBehaviorProto(CKBehaviorPrototype **pproto);
int TTGetFileNameFromPath(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTGetFileNameFromPathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetFileNameFromPath");
    od->SetDescription("Get File Name From Path");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x68e631f7,0x13f914fe));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTGetFileNameFromPathBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTGetFileNameFromPathBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetFileNameFromPath");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OK");

    proto->DeclareInParameter("Path+Name", CKPGUID_STRING);

    proto->DeclareOutParameter("FileName", CKPGUID_STRING);

    proto->DeclareSetting("Use Slash '/'", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTGetFileNameFromPath);

    *pproto = proto;
    return CK_OK;
}

int TTGetFileNameFromPath(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTGetFileNameFromPathCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}