/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Align 3D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

#include "Layout.h" // Use Flow alignment defines

CKObjectDeclaration *FillBehaviorAlign3DDecl();
CKERROR CreateAlign3DProto(CKBehaviorPrototype **);
int Align3D(const CKBehaviorContext &behcontext);

#define CKPGUID_ALIGNMENT CKGUID(0x2e1e2209, 0x47da44b5)

CKObjectDeclaration *FillBehaviorAlign3DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Align 3D");
    od->SetDescription("Aligns 3D Entities on a model.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Model: </SPAN>model on which the frame will be aligned.<BR>
    <SPAN CLASS=pin>Alignment: </SPAN>type of alignment, on the X and Y axis.<BR>
    <SPAN CLASS=pin>Support: </SPAN>type of alignment, on the Z axis.<BR>
    <SPAN CLASS=pin>Center: </SPAN>if a center alignment is desired, either vertically or horizontally, should
    the frame be centered in the model or left alone.<BR>
    <SPAN CLASS=pin>Margins: </SPAN>margins of alignment in axis X and Y, added to the model extents.<BR>
    <SPAN CLASS=pin>Depth Margins: </SPAN>margins of alignment, in axis Z, added to the model extents.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf384bc3, 0x1b34102f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAlign3DProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Interface/Layouts");
    return od;
}

CKERROR CreateAlign3DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Align 3D");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Model", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Alignment", CKPGUID_ALIGNMENT, "Top-Left");
    proto->DeclareInParameter("Support", CKPGUID_FLOW_SUPPORT, "Front");
    proto->DeclareInParameter("Center", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Margins", CKPGUID_RECT, "(0,0),(0,0)");
    proto->DeclareInParameter("Depth Margins", CKPGUID_2DVECTOR, "0,0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(Align3D);

    *pproto = proto;
    return CK_OK;
}

#define VALIGN_TOP 4
#define VALIGN_BOTTOM 8
#define HALIGN_LEFT 1
#define HALIGN_RIGHT 2

#define ALIGN_FRONT 1
#define ALIGN_IN 2
#define ALIGN_REAR 4

int Align3D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *frame = (CK3dEntity *)beh->GetTarget();
    if (!frame)
        return CKBR_OWNERERROR;

    CK3dEntity *model = (CK3dEntity *)beh->GetInputParameterObject(0);

    if (!model)
        return CKBR_PARAMETERERROR;

    int align = VALIGN_TOP | HALIGN_LEFT, support = ALIGN_FRONT;
    beh->GetInputParameterValue(1, &align);
    beh->GetInputParameterValue(2, &support);

    CKBOOL center = FALSE;
    beh->GetInputParameterValue(3, &center);

    VxRect margin;
    beh->GetInputParameterValue(4, &margin);

    Vx2DVector depthMargins;
    beh->GetInputParameterValue(5, &depthMargins);

    VxVector modelPosition, framePosition, modelScale, frameScale, dir, up;

    model->GetPosition(&modelPosition);
    frame->GetPosition(&framePosition);

    model->GetScale(&modelScale, FALSE);
    frame->GetScale(&frameScale, FALSE);

    model->GetOrientation(&dir, &up);
    frame->SetOrientation(&dir, &up);

    // X align
    if (align & HALIGN_LEFT)
    {
        framePosition.x = modelPosition.x - modelScale.x + frameScale.x + margin.left;
    }
    else
    {
        if (align & HALIGN_RIGHT)
        {
            framePosition.x = modelPosition.x + modelScale.x - frameScale.x - margin.right;
        }
        else if (center)
        {
            framePosition.x = modelPosition.x;
        }
    }

    // Y align
    if (align & VALIGN_TOP)
    {
        framePosition.y = modelPosition.y + modelScale.y - frameScale.y - margin.top;
    }
    else
    {
        if (align & VALIGN_BOTTOM)
        {
            framePosition.y = modelPosition.y - modelScale.y + frameScale.y + margin.bottom;
        }
        else if (center)
        {
            framePosition.y = modelPosition.y;
        }
    }

    // Z align
    if (support & ALIGN_FRONT)
    {
        framePosition.z = modelPosition.z - modelScale.z + frameScale.z + depthMargins.x;
    }
    else
    {
        if (support & ALIGN_REAR)
        {
            framePosition.z = modelPosition.z + modelScale.z - frameScale.z - depthMargins.y;
        }
        else if (center)
        {
                framePosition.z = modelPosition.z;
        }
    }
    frame->SetPosition(&framePosition);

    return CKBR_OK;
}
