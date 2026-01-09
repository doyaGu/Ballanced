////////////////////////////////
////////////////////////////////
//
//        TT_SaveAlpha
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSaveAlphaDecl();
CKERROR CreateSaveAlphaProto(CKBehaviorPrototype **pproto);
int SaveAlpha(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSaveAlphaDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SaveAlpha");
    od->SetDescription("Copies the alpha channel to the texture's RGB values and saves them to a TGA file");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ca62d31, 0x5d2031b8));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSaveAlphaProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateSaveAlphaProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SaveAlpha");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Filename", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SaveAlpha);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SaveAlpha(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKTexture *texture = (CKTexture *)beh->GetTarget();
    if (!texture)
        return CKBR_PARAMETERERROR;

    int width = texture->GetWidth();
    int height = texture->GetHeight();

    // Copy alpha channel to RGB
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            CKDWORD pixel = texture->GetPixel(x, y);
            // Extract alpha and put it in RGB (alpha -> RGB, same value in R, G, B)
            CKDWORD alpha = (pixel >> 24) & 0xFF;
            CKDWORD newPixel = (alpha << 16) | (alpha << 8) | alpha;
            texture->SetPixel(x, y, newPixel);
        }
    }

    // Get filename and save
    CKSTRING filename = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    texture->SaveImage(filename, 0);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}