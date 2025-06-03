/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetTextureInfo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetTextureInfoDecl();
CKERROR CreateGetTextureInfoProto(CKBehaviorPrototype **);
int GetTextureInfo(const CKBehaviorContext &behcontext);

//-------------------------------------------------
// Declaration
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorGetTextureInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Texture Info");

    od->SetDescription("Gets informations (filename and priority) about a texture.");
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x250c4d8d, 0x288960d9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetTextureInfoProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

//-------------------------------------------------
// Prototype
//-------------------------------------------------
CKERROR CreateGetTextureInfoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Texture Info");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Exist");
    proto->DeclareOutput("Empty");

    proto->DeclareOutParameter("FileName", CKPGUID_STRING);
    proto->DeclareOutParameter("Priority", CKPGUID_INT);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetTextureInfo);

    *pproto = proto;
    return CK_OK;
}

//-------------------------------------------------
// Function
//-------------------------------------------------
int GetTextureInfo(const CKBehaviorContext &behcontext)
{
    // we get the behavior
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    // Gets the texture to cut
    CKTexture *texture = (CKTexture *)beh->GetTarget();
    if (!texture)
        return CKERR_OUTOFMEMORY;

    CKPathSplitter path(texture->GetSlotFileName(0));

    XString fileName = path.GetName();
    fileName << path.GetExtension();

    beh->SetOutputParameterValue(0, fileName.Str(), fileName.Length() + 1);

    int priority = texture->GetPriority();
    beh->SetOutputParameterValue(1, &priority);

    if (texture->GetHeight() == -1 && texture->GetWidth() == -1)
        beh->ActivateOutput(1);
    else
        beh->ActivateOutput(0);

    return CKBR_OK;
}