/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MouseWaiter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR MouseWaiterCallBack(const CKBehaviorContext &behcontext);

#define MEMOVE				1
#define MELDRECEIVED		2
#define MELURECEIVED		4
#define MERDRECEIVED		8
#define MERURECEIVED		16
#define MEMDRECEIVED		32
#define MEMURECEIVED		64
#define MEROLL				128

CKObjectDeclaration *FillBehaviorMouseWaiterDecl();
CKERROR CreateMouseWaiterProto(CKBehaviorPrototype **pproto);
int MouseWaiter(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMouseWaiterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mouse Waiter");
    od->SetDescription("Activates different outputs according to mouse actions.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Move: </SPAN>activated if the mouse has been moved.<BR>
    <SPAN CLASS=out>Left Button Down: </SPAN>activated if the left mouse button has been pressed.<BR>
    <SPAN CLASS=out>Left Button Up: </SPAN>activated if the left mouse button has been released.<BR>
    <SPAN CLASS=out>Right Button Down: </SPAN>activated if the right mouse button has been pressed.<BR>
    <SPAN CLASS=out>Right Button Up: </SPAN>activated if the right mouse button has been released.<BR>
    <SPAN CLASS=out>Middle Button Down: </SPAN>activated if the middle mouse button has been pressed.<BR>
    <SPAN CLASS=out>Middle Button Up: </SPAN>activated if the middle mouse button has been released.<BR>
    <SPAN CLASS=out>Roll: </SPAN>activated if the mouse wheel has been moved.<BR>
    <BR>
    <SPAN CLASS=pout>Mouse Position: </SPAN>position of the mouse cursor on the screen (NB: this position is NOT expressed in the render window but in the screen coordinates system).<BR>
    <SPAN CLASS=pout>Wheel Direction: </SPAN>direction of the mouse wheel, if applicable.<BR>
    <SPAN CLASS=pout>Left Button: </SPAN>TRUE if the left mouse button is down, FALSE otherwise.<BR>
    <SPAN CLASS=pout>Middle Button: </SPAN>TRUE if the middle mouse button is down, FALSE otherwise.<BR>
    <SPAN CLASS=pout>Right Button: </SPAN>TRUE if the right mouse button is down, FALSE otherwise.<BR>
    <BR>
    <SPAN CLASS=setting>Stay Active: </SPANif TRUE, the building block will remain active after the first event, i.e.after the first mouse action has been recieved and the correponding output has been activated.<BR>
  if FALSE, the 'On' and 'Off' input will be replaced by a 'In' input, and the building block will be deactivated after the first event.<BR>
  <SPAN CLASS=setting>Outputs: </SPAN>only checked outputs will be activated.<BR>
    */
    od->SetCategory("Controllers/Mouse");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x014d010b, 0x015d010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMouseWaiterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateMouseWaiterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Mouse Waiter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Move");
    proto->DeclareOutput("Left Button Down");
    proto->DeclareOutput("Left Button Up");
    proto->DeclareOutput("Right Button Down");
    proto->DeclareOutput("Right Button Up");
    proto->DeclareOutput("Middle Button Down");
    proto->DeclareOutput("Middle Button Up");
    proto->DeclareOutput("Roll Received");

    proto->DeclareOutParameter("Mouse Position", CKPGUID_2DVECTOR);
    proto->DeclareOutParameter("Wheel Direction", CKPGUID_FLOAT, "0");
    proto->DeclareOutParameter("Left Button", CKPGUID_BOOL, "FALSE");
    proto->DeclareOutParameter("Middle Button", CKPGUID_BOOL, "FALSE");
    proto->DeclareOutParameter("Right Button", CKPGUID_BOOL, "FALSE");

    proto->DeclareSetting("Stay Active", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Outputs", CKGUID(0x7abe0373, 0x1c366a1b), "Move,Left Button Down,Left Button Up,Right Button Down,Right Button Up,Middle Button Down,Middle Button Up,Roll Received");
    proto->DeclareSetting("Notify Only if Action Inside 3D Window", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MouseWaiter);

    proto->SetBehaviorCallbackFct(MouseWaiterCallBack);

    *pproto = proto;
    return CK_OK;
}

int MouseWaiter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int sa = 1;
    beh->GetLocalParameterValue(0, &sa);

    if (beh->IsInputActive(0))
    { // The Off Input is Active
        beh->ActivateInput(0, FALSE);
    }

    if (sa)
    {
        if (beh->IsInputActive(1))
        { // The Off Input is Active
            beh->ActivateInput(1, FALSE);
            return CKBR_OK;
        }
    }

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    Vx2DVector oldMousePos;
    beh->GetOutputParameterValue(0, &oldMousePos);

    Vx2DVector mousePos;
    input->GetMousePosition(mousePos, TRUE);
    beh->SetOutputParameterValue(0, &mousePos);

    int mef = 0xffff;
    beh->GetLocalParameterValue(1, &mef);

    CKBOOL constraint = 0;
    beh->GetLocalParameterValue(2, &constraint);

    if (constraint)
    {
        CKRenderContext *rc = behcontext.Context->GetPlayerRenderContext();
        VxRect rect;
        rc->GetWindowRect(rect, TRUE);
        if (rect.IsInside(mousePos))
            constraint = FALSE;
    }

    int pos = 0;
    CKBehaviorIO *out;
    CKBOOL oneactive = FALSE;

    VxVector v3;
    input->GetMouseRelativePosition(v3);

    float wheelDir = v3.z;
    beh->SetOutputParameterValue(1, &wheelDir);

    CKBOOL left_old, left_new; // we handle the left mouse button
    beh->GetOutputParameterValue(2, &left_old);
    left_new = input->IsMouseButtonDown(CK_MOUSEBUTTON_LEFT);

    if (left_new)
    {
        int a = 0;
    }

    beh->SetOutputParameterValue(2, &left_new);

    CKBOOL middle_old, middle_new; // we handle the middle mouse button
    beh->GetOutputParameterValue(3, &middle_old);
    middle_new = input->IsMouseButtonDown(CK_MOUSEBUTTON_MIDDLE);
    beh->SetOutputParameterValue(3, &middle_new);

    CKBOOL right_old, right_new; // we handle the right mouse button
    beh->GetOutputParameterValue(4, &right_old);
    right_new = input->IsMouseButtonDown(CK_MOUSEBUTTON_RIGHT);
    beh->SetOutputParameterValue(4, &right_new);

    // Mouse Move
    if (mef & MEMOVE)
    {
        if (out = beh->GetOutput(pos))
        {
            if (mousePos.x != oldMousePos.x || mousePos.y != oldMousePos.y)
            {
                out->Activate(TRUE);
                oneactive = TRUE;
            }
        }
        ++pos;
    }

    // Left Button
    if (mef & MELDRECEIVED)
    {
        if (out = beh->GetOutput(pos))
        {
            if (!left_old && left_new)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }
    if (mef & MELURECEIVED)
    {
        if (out = beh->GetOutput(pos))
        {
            if (left_old && !left_new)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }

    // Right Button
    if (mef & MERDRECEIVED)
    {
        if (out = beh->GetOutput(pos))
        {
            if (!right_old && right_new)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }
    if (mef & MERURECEIVED)
    {
        if (out = beh->GetOutput(pos))
        {
            if (right_old && !right_new)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }

    // Middle Button
    if (mef & MEMDRECEIVED)
    {
        if (out = beh->GetOutput(pos))
        {
            if (!middle_old && middle_new)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }
    if (mef & MEMURECEIVED)
    {
        if (out = beh->GetOutput(pos))
        {
            if (middle_old && !middle_new)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }

    // Roll
    if (mef & MEROLL)
    {
        if (out = beh->GetOutput(pos))
        {
            if (wheelDir)
            {
                if (!constraint)
                {
                    out->Activate(TRUE);
                    oneactive = TRUE;
                }
            }
        }
        ++pos;
    }

    if (sa)
        return CKBR_ACTIVATENEXTFRAME;
    else if (oneactive)
        return CKBR_OK;
    else
        return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR MouseWaiterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORRESUME:
    {
        CKBOOL f = FALSE;
        beh->SetOutputParameterValue(2, &f);
        beh->SetOutputParameterValue(3, &f);
        beh->SetOutputParameterValue(4, &f);
    };
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL sa = TRUE;
        beh->GetLocalParameterValue(0, &sa);

        // Inputs Creation/Deletion
        CKBehaviorIO *in;
        if (sa)
        {
            in = beh->GetInput(0);
            in->SetName("On");
            in = beh->GetInput(1);
            if (!in)
            {
                in = beh->GetInput(beh->AddInput("Off"));
            }
            in->SetName("Off");
        }
        else
        {
            in = beh->GetInput(0);
            in->SetName("In");
            in = beh->GetInput(1);
            if (in)
                beh->DeleteInput(1);
        }

        int mef = 0xffff;
        beh->GetLocalParameterValue(1, &mef);

        int pos = 0;
        CKBehaviorIO *out;

        // Outputs Creation/Deletion
        if (mef & MEMOVE)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Move Received");
            }
            else
                beh->AddOutput("Move Received");
            ++pos;
        }
        if (mef & MELDRECEIVED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Left Button Down Received");
            }
            else
                beh->AddOutput("Left Button Down Received");
            ++pos;
        }
        if (mef & MELURECEIVED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Left Button Up Received");
            }
            else
                beh->AddOutput("Left Button Up Received");
            ++pos;
        }
        if (mef & MERDRECEIVED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Right Button Down Received");
            }
            else
                beh->AddOutput("Right Button Down Received");
            ++pos;
        }
        if (mef & MERURECEIVED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Right Button Up Received");
            }
            else
                beh->AddOutput("Right Button Up Received");
            ++pos;
        }
        if (mef & MEMDRECEIVED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Middle Button Down Received");
            }
            else
                beh->AddOutput("Middle Button Down Received");
            ++pos;
        }
        if (mef & MEMURECEIVED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Middle Button Up Received");
            }
            else
                beh->AddOutput("Middle Button Up Received");
            ++pos;
        }
        if (mef & MEROLL)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Roll Received");
            }
            else
                beh->AddOutput("Roll Received");
            ++pos;
        }

        // we delete the unusefull outputs
        while (pos < beh->GetOutputCount())
            beh->DeleteOutput(pos);
    }
    break;
    }
    return CKBR_OK;
}
