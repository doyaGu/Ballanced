/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          SetProjection Camera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_PROJECTIONTYPE CKDEFINEGUID(0x1ee22148, 0x602c1ca1)

CKObjectDeclaration *FillBehaviorSetProjectionDecl();
CKERROR CreateSetProjectionProto(CKBehaviorPrototype **pproto);
int SetProjection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetProjectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Projection");
    od->SetDescription("Sets the Projection Type of the Camera.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Projection Type: </SPAN>the type of projection you want to use - orthographic or perspective.<BR>
    <BR>
    Note : The orthographic mode is useful to render simple object when depth is not important since the vertices transformation is much simpler.<BR>
    <BR>
    See Also: 'Orthographic Zoom'.<BR>
    */
    od->SetCategory("Cameras/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xe444e666, 0x4eee6eee));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetProjectionProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateSetProjectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Projection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Projection Type", CKPGUID_PROJECTIONTYPE, "Orthographic");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetProjection);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetProjection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get projection type
    int type = 1;
    beh->GetInputParameterValue(0, &type);

    cam->SetProjectionType(type);

    return CKBR_OK;
}
