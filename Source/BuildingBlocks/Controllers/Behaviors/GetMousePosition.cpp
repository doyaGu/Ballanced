/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetMousePosition
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

extern CKObjectDeclaration *ODecl;

CKObjectDeclaration *FillBehaviorGetMousePositionDecl();
CKERROR CreateGetMousePositionProto(CKBehaviorPrototype **pproto);
int GetMousePosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetMousePositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Mouse Position");
    od->SetDescription("Gets the position of the mouse.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>X: </SPAN>position of the mouse cursor on the X axis of the screen.<BR>
    <SPAN CLASS=pout>Y: </SPAN>position of the mouse cursor on the Y axis of the screen.<BR>
    <SPAN CLASS=pout>Position: </SPAN>position of the mouse cursor as a 2D Vector.<BR>
    <BR>
    <SPAN CLASS=setting>Relative To Window: </SPAN>if checked, returns the position of the mouse relative to the 3d view window.<BR>
    */
    od->SetCategory("Controllers/Mouse");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5b1f1381, 0x5c640372));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetMousePositionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateGetMousePositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Mouse Position");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("X", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Y", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Position", CKPGUID_2DVECTOR);

    proto->DeclareSetting("Windowed Mode", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetMousePosition);

    *pproto = proto;
    return CK_OK;
}

int GetMousePosition(const CKBehaviorContext &behcontext)
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

    CKBOOL windowed = FALSE;
    beh->GetLocalParameterValue(0, &windowed);

    Vx2DVector mousePos;
    input->GetMousePosition(mousePos, !windowed);

    // Set Output Parameters

    // Set X pos
    int x = (int)mousePos.x;
    beh->SetOutputParameterValue(0, &x);
    // Set Y pos
    int y = (int)mousePos.y;
    beh->SetOutputParameterValue(1, &y);
    // Set 2d Vector
    beh->SetOutputParameterValue(2, &mousePos);

    return CKBR_OK;
}
