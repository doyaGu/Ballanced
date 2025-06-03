/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Create Font
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorCreateFontDecl();
CKERROR CreateCreateFontProto(CKBehaviorPrototype **);
int CreateFont(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateFontDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Create Font");
    od->SetDescription("Creates a font from a provided texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Font Name: </SPAN>name of the font : must be unique.<BR>
    <SPAN CLASS=pin>Font Texture: </SPAN>the texture from which we create the font.<BR>
    <SPAN CLASS=pin>Horizontal/Vertical Character Number: </SPAN>number of character horizontally and vertically in the texture.<BR>
    <SPAN CLASS=pin>Proportional: </SPAN>should the character have proportional placement.<BR>
    <SPAN CLASS=pin>Font Zone: </SPAN>zone of the texture in pixel where the font should be read.<BR>
    <SPAN CLASS=pin>First Character: </SPAN>code ASCII of the first character present in the texture.<BR>
    <BR>
    The font created with this building block can be used with the "Text 2D" and the "Text 3D" building blocks.

    See Also: 'Create System Font', 'Delete Font', 'Text2D', 'Text3D' (in the Interface/Text category).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7d644088, 0x40dd0349));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateFontProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Interface/Fonts");
    return od;
}

CKERROR CreateCreateFontProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Create Font");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Font Name", CKPGUID_STRING, "MyFont");
    proto->DeclareInParameter("Font Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Horizontal/Vertical Character Number", CKPGUID_2DVECTOR, "16,8");
    proto->DeclareInParameter("Proportionnal", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Font Bounds", CKPGUID_RECT, "(0,0),(0,0)");
    proto->DeclareInParameter("First Character", CKPGUID_INT, "0");

    proto->DeclareSetting("Space Size", CKPGUID_PERCENTAGE, "30");

    proto->DeclareOutParameter("Font", CKPGUID_FONT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(CreateFont);

    *pproto = proto;
    return CK_OK;
}

int CreateFont(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // We get the fontname
    CKSTRING fname = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!fname)
        return 0;

    CKTexture *fonttexture = (CKTexture *)beh->GetInputParameterObject(1);
    if (!fonttexture)
        return CKBR_PARAMETERERROR;

    CKFontManager *fm = (CKFontManager *)ctx->GetManagerByGuid(FONT_MANAGER_GUID);

    ///
    // Reading Inputs

    Vx2DVector charnumber(16, 8);
    beh->GetInputParameterValue(2, &charnumber);

    CKBOOL prop = TRUE;
    beh->GetInputParameterValue(3, &prop);

    VxRect tzone;
    tzone.SetSize(Vx2DVector((float)fonttexture->GetWidth(), (float)fonttexture->GetHeight()));
    beh->GetInputParameterValue(4, &tzone);

    int firstc = 0;
    beh->GetInputParameterValue(5, &firstc);

    float spaceSize = 0.3f;
    beh->GetLocalParameterValue(0, &spaceSize);

    // The font creation
    int font = fm->CreateTextureFont(fname, fonttexture, tzone, charnumber, !prop, firstc, spaceSize);
    beh->SetOutputParameterValue(0, &font);

    return CKBR_OK;
}
