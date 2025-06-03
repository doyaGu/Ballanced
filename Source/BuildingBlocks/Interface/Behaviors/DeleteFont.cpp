/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DeleteFont
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorDeleteFontDecl();
CKERROR CreateDeleteFontProto(CKBehaviorPrototype **pproto);
int DeleteFont(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Delete a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorDeleteFontDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Delete Font");

    od->SetDescription("Delete a font (created by CreateFont or CreateSystemFont)");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Font : </SPAN>the font identifier of the font to delete.<BR>
    <BR>
    This block releases the resources occupied by a font. The font could be a font created by
    'Create Font', or 'Create System Font' blocks
    <BR>
    See Also: 'Create Font', 'Create System Font', 'Text2D', 'Text3D' (in the Interface/Text category).<BR>
  */

    // Category in Virtools interface
    od->SetCategory("Interface/Fonts");

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    // Unique identifier of this prototype
    od->SetGuid(CKGUID(0x0153342c, 0xe543674f));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);

    // Function that will create the behavior prototype
    od->SetCreationFunction(CreateDeleteFontProto);
    // Class ID of the objects to which the behavior can apply
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(FONT_MANAGER_GUID);

    return od;
}

//-------------------------------------------------
// Font behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateDeleteFontProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Delete Font");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Input/Outputs declaration
    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Font", CKPGUID_FONT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);

    // Function that will be called upon behavior execution
    proto->SetFunction(DeleteFont);

    *pproto = proto;
    return CK_OK;
}

//-------------------------------------------------
// DeleteFont behavior execution function
//
//-------------------------------------------------
int DeleteFont(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    CKFontManager *font = (CKFontManager *)behcontext.Context->GetManagerByGuid(FONT_MANAGER_GUID);

    int fontIndex = -1;
    beh->GetInputParameterValue(0, &fontIndex);
    if (fontIndex != -1)
    {
        // we delete the specified font
        font->DeleteFont(fontIndex);
    }

    beh->ActivateOutput(0);

    return CKBR_OK;
}
