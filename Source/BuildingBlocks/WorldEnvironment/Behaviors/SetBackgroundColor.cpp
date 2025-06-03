/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetBackgroundColor
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetBackgroundColorDecl();
CKERROR CreateSetBackgroundColorProto(CKBehaviorPrototype **);
int SetBackgroundColor(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetBackgroundColorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Background Color");
    od->SetDescription("Sets a background color.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Background Color: </SPAN>background color in RGBA.<BR>
    <BR>
    This behavior can be applied to any object in a scene.
    */
    od->SetCategory("World Environments/Background");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf5faaaaa, 0xfdd5bd00));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetBackgroundColorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetBackgroundColorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Background Color");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Background Color", CKPGUID_COLOR, "128,128,128,255");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetBackgroundColor);

    *pproto = proto;
    return CK_OK;
}

int SetBackgroundColor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get Background color
    VxColor col;
    beh->GetInputParameterValue(0, &col);

    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    CKMaterial *mat = rcontext->GetBackgroundMaterial();
    if (mat)
        mat->SetDiffuse(col);

    return CKBR_OK;
}
