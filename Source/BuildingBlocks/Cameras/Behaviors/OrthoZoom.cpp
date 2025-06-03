/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          OrthographicZoom
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetOrthographicZoomDecl();
CKERROR CreateSetOrthographicZoomProto(CKBehaviorPrototype **pproto);
int SetOrthographicZoom(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetOrthographicZoomDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Orthographic Zoom");
    od->SetDescription("Emulates a zoom when the viewing mode is orthographic.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Zoom: </SPAN>zoom value. The higher the value, the greater the zooming in. Typical values are between 0 and 3.<BR>
    <BR>
    See Also: 'Set Projection'.<BR>
    */
    od->SetCategory("Cameras/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x0a125555, 0x0eee5488));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSetOrthographicZoomProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateSetOrthographicZoomProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Orthographic Zoom");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Zoom", CKPGUID_FLOAT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetOrthographicZoom);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetOrthographicZoom(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    // Get Zoom
    float zoom = 1.0f;

    beh->GetInputParameterValue(0, &zoom);
    if (beh->GetVersion() < 0x00020000)
        zoom *= 0.01f;

    cam->SetOrthographicZoom(zoom);

    return CKBR_OK;
}
