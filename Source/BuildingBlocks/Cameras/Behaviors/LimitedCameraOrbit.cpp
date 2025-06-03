/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	            Keyboard Camera Orbit
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "GeneralCameraOrbit.h"

CKObjectDeclaration *FillBehaviorKeyboardCameraOrbitDecl();
CKERROR CreateKeyboardCameraOrbitProto(CKBehaviorPrototype **pproto);
int KeyboardCameraOrbit(const CKBehaviorContext &behcontext);
void ProcessKeyboardInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping);

CKObjectDeclaration *FillBehaviorKeyboardCameraOrbitDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Keyboard Camera Orbit");
    od->SetDescription("Makes a Camera orbit round a 3D Entity using keyboard with limited angles.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>Starts the behavior.<BR>
    <SPAN CLASS=in>Off: </SPAN>Stops the behavior.<BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when the behavior has been started.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when the behavior has been stop. If in "return" mode, is activated only once the camera is back in place.<BR>
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
    od->SetGuid(CKGUID(0x7610f4d, 0x69747b9d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeyboardCameraOrbitProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Prototype creation
//
//////////////////////////////////////////////////////////////////////////////
CKERROR CreateKeyboardCameraOrbitProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Keyboard Camera Orbit");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // General Description
    CKERROR error = FillGeneralCameraOrbitProto(proto);
    if (error != CK_OK)
        return (error);

    // Additionnal Settings for the Keyboard
    proto->DeclareSetting("Key Rotate Left", CKPGUID_KEY, "Left Arrow");
    proto->DeclareSetting("Key Rotate Right", CKPGUID_KEY, "Right Arrow");
    proto->DeclareSetting("Key Rotate Up", CKPGUID_KEY, "Up Arrow");
    proto->DeclareSetting("Key Rotate Down", CKPGUID_KEY, "Down Arrow");
    proto->DeclareSetting("Key Zoom In", CKPGUID_KEY, "PREVIOUS");
    proto->DeclareSetting("Key Zoom Out", CKPGUID_KEY, "NEXT");

    // Set the execution functions
    proto->SetFunction(KeyboardCameraOrbit);
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
int KeyboardCameraOrbit(const CKBehaviorContext &behcontext)
{
    return (GeneralCameraOrbit(behcontext, ProcessKeyboardInputs));
}

//////////////////////////////////////////////////////////////////////////////
//
//						Function that process the inputs
//
//////////////////////////////////////////////////////////////////////////////
void ProcessKeyboardInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping)
{
    // Is the move limited ?
    CKBOOL Limited = TRUE;
    beh->GetLocalParameterValue(LOCAL_LIMITS, &Limited);

    // Gets the keys settings
    int key_left = 0;
    beh->GetLocalParameterValue(LOCAL_KEY_LEFT, &key_left);
    if (!key_left)
        key_left = CKKEY_LEFT;

    int key_right = 0;
    beh->GetLocalParameterValue(LOCAL_KEY_RIGHT, &key_right);
    if (!key_right)
        key_right = CKKEY_RIGHT;

    int key_up = 0;
    beh->GetLocalParameterValue(LOCAL_KEY_UP, &key_up);
    if (!key_up)
        key_up = CKKEY_UP;

    int key_down = 0;
    beh->GetLocalParameterValue(LOCAL_KEY_DOWN, &key_down);
    if (!key_down)
        key_down = CKKEY_DOWN;

    int key_Zin = 0;
    beh->GetLocalParameterValue(LOCAL_KEY_ZIN, &key_Zin);
    if (!key_Zin)
        key_Zin = CKKEY_PRIOR;

    int key_Zout = 0;
    beh->GetLocalParameterValue(LOCAL_KEY_ZOUT, &key_Zout);
    if (!key_Zout)
        key_Zout = CKKEY_NEXT;

    ////////////////////
    // Position Update
    ////////////////////

    // If the users is moving the camera
    if (((input->IsKeyDown(key_left)) || (input->IsKeyDown(key_right)) || (input->IsKeyDown(key_up)) || (input->IsKeyDown(key_down))) && !stopping)
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

            if (input->IsKeyDown(key_right))
                RotationAngles->x += XMin(SpeedAngle, MaxH - RotationAngles->x);
            if (input->IsKeyDown(key_left))
                RotationAngles->x -= XMin(SpeedAngle, RotationAngles->x - MinH);
            if (input->IsKeyDown(key_down))
                RotationAngles->y += XMin(SpeedAngle, MaxV - RotationAngles->y);
            if (input->IsKeyDown(key_up))
                RotationAngles->y -= XMin(SpeedAngle, RotationAngles->y - MinV);
        }
        else
        {
            if (input->IsKeyDown(key_right))
                RotationAngles->x += SpeedAngle;
            if (input->IsKeyDown(key_left))
                RotationAngles->x -= SpeedAngle;
            if (input->IsKeyDown(key_down))
                RotationAngles->y += SpeedAngle;
            if (input->IsKeyDown(key_up))
                RotationAngles->y -= SpeedAngle;
        }
    }
    else if ((Returns) && ((RotationAngles->x != 0) || (RotationAngles->y != 0)))
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

    if ((input->IsKeyDown(key_Zin)) || (input->IsKeyDown(key_Zout)) && !stopping)
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

            if (input->IsKeyDown(key_Zin))
                RotationAngles->z -= XMin(ZoomSpeed, RotationAngles->z + MaxZoom);
            if (input->IsKeyDown(key_Zout))
                RotationAngles->z += XMin(ZoomSpeed, -MinZoom - RotationAngles->z);
        }
        else
        {
            if (input->IsKeyDown(key_Zin))
                RotationAngles->z -= ZoomSpeed;
            if (input->IsKeyDown(key_Zout))
                RotationAngles->z += ZoomSpeed;
        }
    }
}
