/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Align 2D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAlign2DDecl();
CKERROR CreateAlign2DProto(CKBehaviorPrototype **);
int Align2D(const CKBehaviorContext &behcontext);

#define CKPGUID_ALIGNMENT CKGUID(0x2e1e2209, 0x47da44b5)

CKObjectDeclaration *FillBehaviorAlign2DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Align 2D");
    od->SetDescription("Aligns a 2D Entity on a model.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Model: </SPAN>model on which the frame will be aligned.<BR>
    <SPAN CLASS=pin>Alignment: </SPAN>type of alignement.<BR>
    <SPAN CLASS=pin>Center: </SPAN>if a center alignement is desired, either vertically or horizontally, should
    the frame be centered in the model or left alone.<BR>
    <SPAN CLASS=pin>Margins: </SPAN>margins of alignement, added to the model extents.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3385bc3, 0x1034102f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAlign2DProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Interface/Layouts");
    return od;
}

CKERROR CreateAlign2DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Align 2D");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Model", CKPGUID_2DENTITY);
    proto->DeclareInParameter("Alignment", CKPGUID_ALIGNMENT, "Left");
    proto->DeclareInParameter("Center", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Margins", CKPGUID_RECT, "(0,0),(0,0)");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(Align2D);

    *pproto = proto;
    return CK_OK;
}

#define VALIGN_TOP 4
#define VALIGN_BOTTOM 8

#define HALIGN_LEFT 1
#define HALIGN_RIGHT 2

int Align2D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK2dEntity *frame = (CK2dEntity *)beh->GetTarget();
    if (!frame)
        return CKBR_OWNERERROR;

    CK2dEntity *model = (CK2dEntity *)beh->GetInputParameterObject(0);

    VxRect modelrect;
    if (model)
    {
        model->GetRect(modelrect);
    }
    else
    { // no model entity given, we use the screen
        CKRenderContext *rc = ctx->GetPlayerRenderContext();
        rc->GetViewRect(modelrect);
    }

    int align = 0;
    beh->GetInputParameterValue(1, &align);

    CKBOOL center = FALSE;
    beh->GetInputParameterValue(2, &center);

    VxRect margin;
    beh->GetInputParameterValue(3, &margin);

    modelrect.left += margin.left;
    modelrect.top += margin.top;
    modelrect.bottom -= margin.bottom;
    modelrect.right -= margin.right;

    VxRect framerect;
    frame->GetRect(framerect);

    // Horizontal align
    if (align & HALIGN_LEFT)
    {
        framerect.HMove(modelrect.left);
    }
    else
    {
        if (align & HALIGN_RIGHT)
        {
            framerect.HMove(modelrect.right - framerect.GetWidth());
        }
        else if (center)
        {
            framerect.HMove(modelrect.left + modelrect.GetWidth() * 0.5f - framerect.GetWidth() * 0.5f);
        }
    }

    // Vertical align
    if (align & VALIGN_TOP)
    {
        framerect.VMove(modelrect.top);
    }
    else
    {
        if (align & VALIGN_BOTTOM)
        {
            framerect.VMove(modelrect.bottom - framerect.GetHeight());
        }
        else if (center)
        {
            framerect.VMove(modelrect.top + modelrect.GetHeight() * 0.5f - framerect.GetHeight() * 0.5f);
        }
    }

    frame->SetRect(framerect);

    return CKBR_OK;
}
