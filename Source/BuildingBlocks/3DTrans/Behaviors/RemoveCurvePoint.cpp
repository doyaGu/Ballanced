/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        Remove Control Point
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveCurvePointDecl();
CKERROR CreateRemoveCurvePointProto(CKBehaviorPrototype **);
int RemoveCurvePoint(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveCurvePointDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Control Point");
    od->SetDescription("Allows to shoot a control point of the curve");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>CurvePoint Object: </SPAN>handle of the curve point to remove.<BR>
    <BR>
    This behavior removes a control point of the curve.<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1234effa, 0xffbec112));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveCurvePointProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    return od;
}

CKERROR CreateRemoveCurvePointProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Control Point");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("CurvePoint Object", CKPGUID_CURVEPOINT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RemoveCurvePoint);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RemoveCurvePoint(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCurve *ent = (CKCurve *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get CurvePoint
    CKCurvePoint *borntodie = (CKCurvePoint *)beh->GetInputParameterObject(0);

    // Behavior processing

    return (ent->RemoveControlPoint(borntodie, FALSE));
}
