/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            JoystickWaiter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_JOYAXIS CKGUID(0x1ed64a6a, 0x5a24059a)

CKObjectDeclaration *FillBehaviorJoystickWaiterDecl();
CKERROR CreateJoystickWaiterProto(CKBehaviorPrototype **pproto);
int JoystickWaiter(const CKBehaviorContext &behcontext);
int JoystickWaiterV2(const CKBehaviorContext &behcontext);
CKERROR JoystickWaiterCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorJoystickWaiterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Joystick Waiter");
    od->SetDescription("Activates different outputs according to Joystick commands.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the behavior.<BR>
    <SPAN CLASS=in>Off: </SPAN>disables the behavior.<BR>
    <BR>
    <SPAN CLASS=out>Joystick Idle: </SPAN>is activated when the joystick is idle (no direction, no button pressed.)<BR>
    <SPAN CLASS=out>xxx Moved: </SPAN>is activated when the joystick is moving in its xxx axis. See the setings for the possible axis list.<BR>
    <SPAN CLASS=out>Button xxx Moved: </SPAN>is activated when the xxxth joystick button is pressed, with xxx ranging from 1 to 32.<BR>
    <BR>
    <SPAN CLASS=pout>Analog Position: </SPAN>vector containing joystick's X-axis, Y-axis and Z-axis values respectively.<BR>
    <SPAN CLASS=pout>Rotation: </SPAN>vector containing joystick's X-axis, Y-axis and Z-axis rotation values respectively.<BR>
    <SPAN CLASS=pout>Sliders: </SPAN>vector2D containing joystick's Slider1 (U Axis) and Slider2 (V Axis) values respectively.<BR>
    <SPAN CLASS=pout>Point Of View: </SPAN>angle containing joystick's point of view (a negative value means the POV is centered).<BR>
    <BR>
    <SPAN CLASS=setting>Axis Sensitivity: </SPAN>minimum move needed to activate an output, values betwwen 0 and 1.
    Use X-axis for absolute moves, Y-axis for relative moves. Z-axis is obsolete and uses for backward compatibility.<BR>
    <SPAN CLASS=setting>Axis Outputs: </SPAN>The possible Axis are : Axis X, Axis Y, Axis Z, Rotation X, Rotation Y,
    Rotation Z, Slider 1, Slider 2, Point Of View (it can be moved or centered, actiavting two different outputs.).<BR>
    <SPAN CLASS=setting>Button Outputs: </SPAN>Choose the buttons among 32 you want to see states.<BR>
    <SPAN CLASS=setting>Relative Moves: </SPAN>Which axis's output should be activated by relative moves only.<BR>
    <BR>
    Each ouput is activated according to the joystick's state while the building block remains active.<BR>
    <BR>
    See also : Joystick Mapper.<BR>
    */
    /* warning:
    The activated output is highly dependent of the joystick you are using. For instance, an analogic
    stick can activate the Z-rotation and the slider 1 outputs. It directly depends of the hardware.
    Use the "Relative Moves" flags depending on the relation between the physical move and the output activated.
    There is no garanty to have the same results with different joysticks.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetCategory("Controllers/Joystick");
    od->SetGuid(CKGUID(0x012d010b, 0x013d010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateJoystickWaiterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateJoystickWaiterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Joystick Waiter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Joystick Number", CKPGUID_INT, "0");

    proto->DeclareOutput("Joystick Idle");
    proto->DeclareOutput("Axis X Moved");
    proto->DeclareOutput("Axis Y Moved");

    proto->DeclareOutParameter("Analog Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareOutParameter("Rotation", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareOutParameter("Sliders", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareOutParameter("Point Of View", CKPGUID_ANGLE);

    proto->DeclareSetting("Axis Sensitivity", CKPGUID_VECTOR, "0.5,0.1,0.5");
    proto->DeclareSetting("Axis Outputs", CKPGUID_JOYAXIS, "Axis X,Axis Y");
    proto->DeclareSetting("Button Outputs", CKPGUID_FILTER, "1");
    proto->DeclareSetting("Relative Moves", CKPGUID_JOYAXIS, "Slider 1,Slider 2,POV");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(JoystickWaiterV2);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetBehaviorCallbackFct(JoystickWaiterCallBack);

    *pproto = proto;
    return CK_OK;
}

int JoystickWaiter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1)) // The Off Input is Active
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    // we get the 3dEntity entity
    CKBeObject *ent = (CKBeObject *)beh->GetOwner();
    if (!ent)
        return CKBR_OWNERERROR;

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    VxVector sensitivity(0.5f, 0.5f, 0.5f);
    beh->GetLocalParameterValue(0, &sensitivity);

    int joy = 0;
    beh->GetInputParameterValue(0, &joy);

    if (!input->IsJoystickAttached(joy))
    {
        behcontext.Context->OutputToConsoleEx("Joystick not attached");
        return CKBR_GENERICERROR;
    }

    VxVector pos;
    input->GetJoystickPosition(joy, &pos);
    beh->SetOutputParameterValue(0, &pos);

    VxVector rot;
    input->GetJoystickRotation(joy, &rot);
    beh->SetOutputParameterValue(1, &rot);

    Vx2DVector slid;
    input->GetJoystickSliders(joy, &slid);
    beh->SetOutputParameterValue(2, &slid);

    float pov = -1.0f;
    input->GetJoystickPointOfViewAngle(joy, &pov);
    beh->SetOutputParameterValue(3, &pov);

    CKDWORD joybut = input->GetJoystickButtonsState(joy);

    if (joybut & 1)
        beh->ActivateOutput(4, TRUE);
    if (joybut & 2)
        beh->ActivateOutput(5, TRUE);
    if (joybut & 4)
        beh->ActivateOutput(6, TRUE);
    if (joybut & 8)
        beh->ActivateOutput(7, TRUE);
    if (joybut & 16)
        beh->ActivateOutput(8, TRUE);
    if (joybut & 32)
        beh->ActivateOutput(9, TRUE);
    if (pos.y < -sensitivity.y)
        beh->ActivateOutput(0, TRUE);
    if (pos.y > sensitivity.y)
        beh->ActivateOutput(1, TRUE);
    if (pos.x < -sensitivity.x)
        beh->ActivateOutput(2, TRUE);
    if (pos.x > sensitivity.x)
        beh->ActivateOutput(3, TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

#define F_X 1
#define F_Y 2
#define F_Z 4
#define F_XYZ 7
#define F_RX 8
#define F_RY 16
#define F_RZ 32
#define F_RXYZ 56
#define F_S1 64
#define F_S2 128
#define F_POV 256

int JoystickWaiterV2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1)) // The Off Input is Active
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CKBOOL oneactive = FALSE;

    // we get the 3dEntity entity
    CKBeObject *ent = (CKBeObject *)beh->GetOwner();
    if (!ent)
        return CKBR_OWNERERROR;

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    VxVector sensitivity(0.5f, 0.5f, 0.5f);
    beh->GetLocalParameterValue(0, &sensitivity);

    int joy = 0;
    beh->GetInputParameterValue(0, &joy);

    if (!input->IsJoystickAttached(joy))
    {
        behcontext.Context->OutputToConsoleEx("Joystick not attached");
        return CKBR_GENERICERROR;
    }

    int f = 3;
    beh->GetLocalParameterValue(1, &f);

    int relative;
    beh->GetLocalParameterValue(3, &relative);

    int outpos = 1;

    ///
    // Positions
    VxVector pos;
    VxVector oldpos;
    input->GetJoystickPosition(joy, &pos);
    beh->GetOutputParameterValue(0, &oldpos);
    beh->SetOutputParameterValue(0, &pos);

    if (f & F_XYZ)
    {
        // Axis X
        if (f & F_X)
        {
            if (relative & F_X)
            {
                if ((oldpos.x - pos.x < -sensitivity.y) || (oldpos.x - pos.x > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((pos.x < -sensitivity.x) || (pos.x > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }

        // Axis Y
        if (f & F_Y)
        {
            if (relative & F_Y)
            {
                if ((oldpos.y - pos.y < -sensitivity.y) || (oldpos.y - pos.y > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((pos.y < -sensitivity.x) || (pos.y > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }

        // Axis Z
        if (f & F_Z)
        {
            if (relative & F_Z)
            {
                if ((oldpos.z - pos.z < -sensitivity.y) || (oldpos.z - pos.z > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((pos.z < -sensitivity.x) || (pos.z > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }
    }

    ///
    // Rotations
    VxVector rot;
    VxVector oldrot;
    input->GetJoystickRotation(joy, &rot);
    beh->GetOutputParameterValue(1, &oldrot);
    beh->SetOutputParameterValue(1, &rot);

    if (f & F_RXYZ)
    {
        // Axis X
        if (f & F_RX)
        {
            if (relative & F_RX)
            {
                if ((rot.x - oldrot.x < -sensitivity.y) || (rot.x - oldrot.x > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((rot.x < -sensitivity.x) || (rot.x > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }

        // AXis Y
        if (f & F_RY)
        {
            if (relative & F_RY)
            {
                if ((rot.y - oldrot.y < -sensitivity.y) || (rot.y - oldrot.y > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((rot.y < -sensitivity.x) || (rot.y > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }

        // AXis Z
        if (f & F_RZ)
        {
            if (relative & F_RZ)
            {
                if ((rot.z - oldrot.z < -sensitivity.y) || (rot.z - oldrot.z > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((rot.z < -sensitivity.x) || (rot.z > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }
    }

    ///
    // Sliders
    Vx2DVector slid;
    Vx2DVector oldslid;
    input->GetJoystickSliders(joy, &slid);
    beh->GetOutputParameterValue(2, &oldslid);
    beh->SetOutputParameterValue(2, &slid);

    if (f & (F_S1 | F_S2))
    {
        // Slider1
        if (f & F_S1)
        {
            if (relative & F_S1)
            {
                if ((slid.x - oldslid.x < -sensitivity.y) || (slid.x - oldslid.x > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((slid.x < -sensitivity.x) || (slid.x > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }

        // Slider 2
        if (f & F_S2)
        {
            if (relative & F_S2)
            {
                if ((slid.y - oldslid.y < -sensitivity.y) || (slid.y - oldslid.y > sensitivity.y))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            else
            {
                if ((slid.y < -sensitivity.x) || (slid.y > sensitivity.x))
                {
                    beh->ActivateOutput(outpos, TRUE);
                    oneactive = TRUE;
                }
            }
            outpos++;
        }
    }

    // Point Of View
    float oldpov = -1.0f, pov = -1.0f;
    input->GetJoystickPointOfViewAngle(joy, &pov);
    beh->GetOutputParameterValue(3, &oldpov);
    beh->SetOutputParameterValue(3, &pov);

    if (f & F_POV)
    {
        if (relative & F_POV)
        {
            if (pov != oldpov)
            { // Has It Moved
                if (pov >= 0.0f)
                { // Not Centered
                    beh->ActivateOutput(outpos, TRUE);
                }
                else
                    beh->ActivateOutput(outpos + 1, TRUE); // Centered
                oneactive = TRUE;
            }
        }
        else
        {
            if (pov >= 0.0f)
            { // Not Centered
                beh->ActivateOutput(outpos, TRUE);
            }
            else
                beh->ActivateOutput(outpos + 1, TRUE); // Centered
            oneactive = TRUE;
        }
        outpos += 2;
    }

    ///
    // Buttons
    CKDWORD joybut = input->GetJoystickButtonsState(joy);

    // Creation of Buttons Output
    beh->GetLocalParameterValue(2, &f);

    int mask = 1;
    for (int i = 0; i < 32; ++i, mask *= 2)
    {
        if (f & mask) // the button is required
        {
            if (joybut & mask)
            {
                beh->ActivateOutput(outpos);
                oneactive = TRUE;
            }
            ++outpos;
        }
    }

    // Joystick Idle ?
    if (!oneactive)
        beh->ActivateOutput(0, TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR JoystickWaiterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
        if (beh->GetVersion() < 0x00020000)
            beh->SetFunction(JoystickWaiter);
        break;
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        if (beh->GetVersion() < 0x00020000)
            return CK_OK;

        CKDWORD f = 3;
        // Axis
        beh->GetLocalParameterValue(1, &f);

        int pos = 1; // the first output stay there always
        CKBehaviorIO *out;

        // Outputs Creation/Deletion Of Axis Events
        if (f & F_X)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Axis X Moved");
            }
            else
                beh->AddOutput("Axis X Moved");
            ++pos;
        }
        if (f & F_Y)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Axis Y Moved");
            }
            else
                beh->AddOutput("Axis Y Moved");
            ++pos;
        }
        if (f & F_Z)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Axis Z Moved");
            }
            else
                beh->AddOutput("Axis Z Moved");
            ++pos;
        }
        if (f & F_RX)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Rotation X Moved");
            }
            else
                beh->AddOutput("Rotation X Moved");
            ++pos;
        }
        if (f & F_RY)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Rotation Y Moved");
            }
            else
                beh->AddOutput("Rotation Y Moved");
            ++pos;
        }
        if (f & F_RZ)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Rotation Z Moved");
            }
            else
                beh->AddOutput("Rotation Z Moved");
            ++pos;
        }
        if (f & F_S1)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Slider 1 Moved");
            }
            else
                beh->AddOutput("Slider 1 Moved");
            ++pos;
        }
        if (f & F_S2)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Slider 2 Moved");
            }
            else
                beh->AddOutput("Slider 2 Moved");
            ++pos;
        }
        if (f & F_POV)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Point Of View Moved");
            }
            else
                beh->AddOutput("Point Of View Moved");
            ++pos;
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Point Of View Centered");
            }
            else
                beh->AddOutput("Point Of View Centered");
            ++pos;
        }

        // Creation of Buttons Output
        beh->GetLocalParameterValue(2, &f);

        int mask = 1;
        char buffer[32];
        for (int i = 0; i < 32; ++i, mask *= 2)
        {
            if (f & mask)
            { // the button is required
                sprintf(buffer, "Button %d Pressed", i + 1);
                if (out = beh->GetOutput(pos))
                {
                    out->SetName(buffer);
                }
                else
                    beh->AddOutput(buffer);
                ++pos;
            }
        }

        // we delete the unusefull outputs
        while (pos < beh->GetOutputCount())
            beh->DeleteOutput(pos);
    }
    break;
    }

    return CK_OK;
}
