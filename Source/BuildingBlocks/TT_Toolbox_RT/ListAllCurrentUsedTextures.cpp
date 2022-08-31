/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//        TT_ListAllCurrentUsedTextures
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTListAllCurrentUsedTexturesBehaviorProto(CKBehaviorPrototype **pproto);
int TTListAllCurrentUsedTextures(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTListAllCurrentUsedTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ListAllCurrentUsedTextures");
    od->SetDescription("List All Current Used Textures in a DataArray");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6b6d781e,0x4b531860));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTListAllCurrentUsedTexturesBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTListAllCurrentUsedTexturesBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ListAllCurrentUsedTextures");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Array", CKPGUID_DATAARRAY);

    proto->DeclareSetting("OnInVideoMemory", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTListAllCurrentUsedTextures);

    *pproto = proto;
    return CK_OK;
}

int TTListAllCurrentUsedTextures(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTListAllCurrentUsedTexturesCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}