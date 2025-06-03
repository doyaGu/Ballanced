/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetMouseRelativePosition
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

extern CKObjectDeclaration *ODecl;

CKObjectDeclaration *FillBehaviorGetMouseRelativePositionDecl();
CKERROR CreateGetMouseRelativePositionProto(CKBehaviorPrototype **pproto);
int GetMouseRelativePosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetMouseRelativePositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Mouse Displacement");
    od->SetDescription("Gets the mouse relative movement.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>X: </SPAN>number of pixels the mouse has moved along the X axis of the screen.<BR>
    <SPAN CLASS=pout>Y: </SPAN>number of pixels the mouse has moved along the Y axis of the screen.<BR>
    <SPAN CLASS=pout>Displacement Vector:  </SPAN>number of pixels the mouse has been moved along the screen in a 2D Vector.<BR>
    <BR>
    The movement returned is the number of pixel the mouse moved since the previous frame.
  Positive X movement is from left to right. Positive Y movement is from top to bottom.<BR>
    */
    od->SetCategory("Controllers/Mouse");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x32d26c84, 0x3c741f89));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetMouseRelativePositionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateGetMouseRelativePositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Mouse Displacement");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("X", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Y", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Displacement Vector", CKPGUID_2DVECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetMouseRelativePosition);

    *pproto = proto;
    return CK_OK;
}

int GetMouseRelativePosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    VxVector mousePos;
    input->GetMouseRelativePosition(mousePos);

    // Set Output Parameters
    // Set X pos
    int x = (int)mousePos.x;
    beh->SetOutputParameterValue(0, &x);
    // Set Y pos
    int y = (int)mousePos.y;
    beh->SetOutputParameterValue(1, &y);

    // Set 2d Vector
    Vx2DVector v(mousePos.x, mousePos.y);
    beh->SetOutputParameterValue(2, &v);

    return CKBR_OK;
}
