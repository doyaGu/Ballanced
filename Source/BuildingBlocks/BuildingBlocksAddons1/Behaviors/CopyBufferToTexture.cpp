/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              CopyBufferToTexture
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCopyBufferToTextureDecl();
CKERROR CreateCopyBufferToTextureProto(CKBehaviorPrototype **);
int CopyBufferToTexture(const CKBehaviorContext &behcontext);
CKERROR CopyBufferToTextureCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCopyBufferToTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Copy Buffer to Texture");
    od->SetDescription("Copy a part of the frame buffer to a texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    */
    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xef6070d, 0x67166889));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCopyBufferToTextureProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateCopyBufferToTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Copy Buffer to Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Source", CKPGUID_RECT, "(0,0),(64,64)");
    proto->DeclareInParameter("Destination", CKPGUID_RECT, "(0,0),(64,64)");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CopyBufferToTexture);

    *pproto = proto;
    return CK_OK;
}

int CopyBufferToTexture(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OWNERERROR;
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    VxRect r1, r2;
    beh->GetInputParameterValue(0, &r1);
    beh->GetInputParameterValue(1, &r2);

    //-- rects must have the same size for this to work...
    if ((r1.GetWidth() == r2.GetWidth()) &&
        (r1.GetHeight() == r2.GetHeight()))
    {
        tex->CopyContext(behcontext.CurrentRenderContext, &r1, &r2);

        CKBOOL updateSystem = FALSE;
        beh->GetLocalParameterValue(0, &updateSystem);
    }

    return CKBR_OK;
}
