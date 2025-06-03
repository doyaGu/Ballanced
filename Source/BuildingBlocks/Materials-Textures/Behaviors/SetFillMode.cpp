/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set FillMode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetFillModeDecl();
CKERROR CreateSetFillModeProto(CKBehaviorPrototype **);
int SetFillMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFillModeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Fill Mode");
    od->SetDescription("Sets the fill mode of a Material : 1 = Point, 2 = WireFrame, 3 = Solid.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Fill Mode: </SPAN>Point / WireFrame / Solid.<BR>
    <BR>
    Point: the object is rendered as a set of points representing its vertices.<BR>
    WireFrame: the object is rendered as a set lines representing its edges.<BR>
    Solid: the object is rendered as a set of filled polygons representing its surface.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xc5c4c3c2, 0x8c8c8c8c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetFillModeProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetFillModeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Fill Mode");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Fill Mode", CKPGUID_FILLMODE, "WireFrame");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFillMode);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetFillMode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    int mode = VXFILL_WIREFRAME;
    beh->GetInputParameterValue(0, &mode);
    mat->SetFillMode((VXFILL_MODE)mode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
