/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//        TT_ListAllCurrentUsedTextures
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorListAllCurrentUsedTexturesDecl();
CKERROR CreateListAllCurrentUsedTexturesProto(CKBehaviorPrototype **pproto);
int ListAllCurrentUsedTextures(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListAllCurrentUsedTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ListAllCurrentUsedTextures");
    od->SetDescription("List All Current Used Textures in a DataArray");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6b6d781e, 0x4b531860));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateListAllCurrentUsedTexturesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateListAllCurrentUsedTexturesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ListAllCurrentUsedTextures");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Array", CKPGUID_DATAARRAY);

    proto->DeclareSetting("OnInVideoMemory", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ListAllCurrentUsedTextures);

    *pproto = proto;
    return CK_OK;
}

int ListAllCurrentUsedTextures(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}