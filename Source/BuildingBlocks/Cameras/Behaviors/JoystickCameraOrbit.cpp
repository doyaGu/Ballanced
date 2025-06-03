/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	            Joystick Camera Orbit
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "GeneralCameraOrbit.h"

CKObjectDeclaration *FillBehaviorJoystickCameraOrbitDecl();
CKERROR CreateJoystickCameraOrbitProto(CKBehaviorPrototype **pproto);
int JoystickCameraOrbit(const CKBehaviorContext &behcontext);
void ProcessJoystickInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping);

CKObjectDeclaration *FillBehaviorJoystickCameraOrbitDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Joystick Camera Orbit");
    od->SetDescription("Makes a Camera orbit round a 3D Entity using Joystick.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Stop: </SPAN>stops the process.<BR>
    <SPAN CLASS=out>Ended: </SPAN>is activated when the process has been stopped and the camera has recovered its original position in case "returns" setting is true.<BR>
    <BR>
    <SPAN CLASS=pin>Target Position: </SPAN>Position we are turning around.<BR>
    <SPAN CLASS=pin>Target Referential: </SPAN>Referential where the position is defined.<BR>
    <SPAN CLASS=pin>Move Speed: </SPAN>Speed in angle per second used when the user moves the camera.<BR>
    <SPAN CLASS=pin>Return Speed: </SPAN>Speed in angle per second used when the camera returns.<BR>
    <SPAN CLASS=pin>Min Horizontal: </SPAN>Minimal angle allowed on the horizontal rotation. Must have a negative value.<BR>
    <SPAN CLASS=pin>Max Horizontal:</SPAN>Maximal angle allowed on the horizontal rotation. Must have a positive value.<BR>
    <SPAN CLASS=pin>Min Vertical: </SPAN>Minimal angle allowed on the vertical rotation. Must have a negative value.<BR>
    <SPAN CLASS=pin>Max Vertical: </SPAN>Maximal angle allowed on the vertical rotation. Must have a positive value.<BR>
    <SPAN CLASS=pin>Zoom Speed: </SPAN>Speed of the zoom in distance per second.<BR>
    <SPAN CLASS=pin>Zoom Min: </SPAN>Minimum zoom value allowed. Must have a negative value.<BR>
    <SPAN CLASS=pin>Zoom Max: </SPAN>Maximum zoom value allowed. Must have a positive value.<BR>
    <BR>
    The following keys are used by default to move the Camera around its target:<BR>
    <BR>
    <FONT COLOR=#FFFFFF>Page Up: </FONT>Zoom in.<BR>
    <FONT COLOR=#FFFFFF>Page Down: </FONT>Zoom out.<BR>
    <FONT COLOR=#FFFFFF>Up and Down Arrows: </FONT>Rotate vertically.<BR>
    <FONT COLOR=#FFFFFF>Left and Right Arrows: </FONT>Rotate horizontally.<BR>
    The arrow keys are the inverted T arrow keys and not the ones of the numeric keypad.<BR>
    <BR>
    <SPAN CLASS=setting>Returns: </SPAN>Does the camera systematically returns to its original position.<BR>
    <SPAN CLASS=setting>Key Rotate Left: </SPAN>Key used to rotate left.<BR>
    <SPAN CLASS=setting>Key Rotate Right: </SPAN>Key used to rotate right.<BR>
    <SPAN CLASS=setting>Key Rotate Up: </SPAN>Key used to rotate up.<BR>
    <SPAN CLASS=setting>Key Rotate Down: </SPAN>Key used to rotate down.<BR>
    <SPAN CLASS=setting>Key Zoom in: </SPAN>Key used to zoom in.<BR>
    <SPAN CLASS=setting>Key Zoom out: </SPAN>Key used to zoom in.<BR>
    <BR>
    */
    od->SetCategory("Cameras/Movement");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x400f0e6f, 0x72822162));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateJoystickCameraOrbitProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Prototype creation
//
//////////////////////////////////////////////////////////////////////////////
CKERROR CreateJoystickCameraOrbitProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Joystick Camera Orbit");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // General Description
    CKERROR error = FillGeneralCameraOrbitProto(proto);
    if (error != CK_OK)
        return (error);

    // Setting to choose the mouse button
    proto->DeclareSetting("Joystick Number", CKPGUID_INT, "0");
    proto->DeclareSetting("Zoom In Button", CKPGUID_INT, "1");
    proto->DeclareSetting("Zoom Out Button", CKPGUID_INT, "2");
    proto->DeclareSetting("Inverse Up / Down", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Threshold", CKPGUID_FLOAT, "0.1");

    // Set the execution functions
    proto->SetFunction(JoystickCameraOrbit);
    proto->SetBehaviorCallbackFct(GeneralCameraOrbitCallback, CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORLOAD | CKCB_BEHAVIORCREATE);

    // return OK
    *pproto = proto;
    return CK_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Main Function
//
//////////////////////////////////////////////////////////////////////////////
int JoystickCameraOrbit(const CKBehaviorContext &behcontext)
{
    return (GeneralCameraOrbit(behcontext, ProcessJoystickInputs));
}

//////////////////////////////////////////////////////////////////////////////
//
//						Function that process the inputs
//
//////////////////////////////////////////////////////////////////////////////
void ProcessJoystickInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping)
{
    // Is the move limited ?
    CKBOOL Limited = TRUE;
    beh->GetLocalParameterValue(LOCAL_LIMITS, &Limited);

    // Gets the Joystick informations
    int JoystickNumber = 0;
    VxVector JoystickPosition(0, 0, 0);
    beh->GetLocalParameterValue(LOCAL_JOY_NB, &JoystickNumber);
    input->GetJoystickPosition(JoystickNumber, &JoystickPosition);

    // Is the move vertically inverted ?
    CKBOOL inverse = FALSE;
    float InverseFactor = 1.0f;
    beh->GetLocalParameterValue(LOCAL_JOY_INVERSE, &inverse);
    if (inverse)
        InverseFactor = -1.0f;

    // Gets the joystick threshold
    float epsilon = 0.1f;
    beh->GetLocalParameterValue(LOCAL_JOY_THRESHOLD, &epsilon);

    ////////////////////
    // Position Update
    ////////////////////

    // If the users is moving the camera
    if (((fabs(JoystickPosition.x) > epsilon) || (fabs(JoystickPosition.y) > epsilon)) && !stopping)
    {
        float SpeedAngle = 0.87f;
        beh->GetInputParameterValue(IN_SPEED_MOVE, &SpeedAngle);
        SpeedAngle *= delta / 1000;

        if (Limited)
        {
            float MinH = -PI / 2;
            beh->GetInputParameterValue(IN_MIN_H, &MinH);

            float MaxH = PI / 2;
            beh->GetInputParameterValue(IN_MAX_H, &MaxH);

            float MinV = -PI / 2;
            beh->GetInputParameterValue(IN_MIN_V, &MinV);

            float MaxV = PI / 2;
            beh->GetInputParameterValue(IN_MAX_V, &MaxV);

            RotationAngles->x += JoystickPosition.x * SpeedAngle;
            RotationAngles->x = XMin(RotationAngles->x, MaxH);
            RotationAngles->x = XMax(RotationAngles->x, MinH);

            RotationAngles->y += InverseFactor * JoystickPosition.y * SpeedAngle;
            RotationAngles->y = XMin(RotationAngles->y, MaxV);
            RotationAngles->y = XMax(RotationAngles->y, MinV);
        }
        else
        {
            RotationAngles->x += JoystickPosition.x * SpeedAngle;
            RotationAngles->y += InverseFactor * JoystickPosition.y * SpeedAngle;
        }
    }
    else if (Returns && ((RotationAngles->x != 0) || (RotationAngles->y != 0)))
    {
        float ReturnSpeedAngle = 1.75f;
        beh->GetInputParameterValue(IN_SPEED_RETURN, &ReturnSpeedAngle);
        ReturnSpeedAngle *= delta / 1000;

        if (RotationAngles->x < 0)
            RotationAngles->x += XMin(ReturnSpeedAngle, -RotationAngles->x);
        if (RotationAngles->x > 0)
            RotationAngles->x -= XMin(ReturnSpeedAngle, RotationAngles->x);
        if (RotationAngles->y < 0)
            RotationAngles->y += XMin(ReturnSpeedAngle, -RotationAngles->y);
        if (RotationAngles->y > 0)
            RotationAngles->y -= XMin(ReturnSpeedAngle, RotationAngles->y);
    }

    ////////////////
    // Zoom Update
    ////////////////

    int ZinButton;
    beh->GetLocalParameterValue(LOCAL_JOY_ZIN, &ZinButton);

    int ZoutButton;
    beh->GetLocalParameterValue(LOCAL_JOY_ZOUT, &ZoutButton);

    if ((input->IsJoystickButtonDown(JoystickNumber, ZinButton)) || (input->IsJoystickButtonDown(JoystickNumber, ZoutButton)) && !stopping)
    {
        float ZoomSpeed = 40.0f;
        beh->GetInputParameterValue(IN_SPEED_ZOOM, &ZoomSpeed);
        ZoomSpeed *= delta / 1000;

        if (Limited)
        {
            float MinZoom = -40.0f;
            beh->GetInputParameterValue(IN_MIN_ZOOM, &MinZoom);

            float MaxZoom = 10.0f;
            beh->GetInputParameterValue(IN_MAX_ZOOM, &MaxZoom);

            if (input->IsJoystickButtonDown(JoystickNumber, ZinButton))
                RotationAngles->z -= XMin(ZoomSpeed, RotationAngles->z + MaxZoom);
            if (input->IsJoystickButtonDown(JoystickNumber, ZoutButton))
                RotationAngles->z += XMin(ZoomSpeed, -MinZoom - RotationAngles->z);
        }
        else
        {
            if (input->IsJoystickButtonDown(JoystickNumber, ZinButton))
                RotationAngles->z -= ZoomSpeed;
            if (input->IsJoystickButtonDown(JoystickNumber, ZoutButton))
                RotationAngles->z += ZoomSpeed;
        }
    }
}
