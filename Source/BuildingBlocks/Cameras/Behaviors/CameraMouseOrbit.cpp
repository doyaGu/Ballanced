/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	            Mouse Camera Orbit
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "GeneralCameraOrbit.h"

#define CKPGUID_MOUSEBUTTON CKDEFINEGUID(0x1ff24d5a, 0x122f2c1f)

CKObjectDeclaration *FillBehaviorMouseCameraOrbitDecl();
CKERROR CreateMouseCameraOrbitProto(CKBehaviorPrototype **pproto);
int MouseCameraOrbit(const CKBehaviorContext &behcontext);
void ProcessMouseInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping);
CKERROR MouseCameraOrbitCallback(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorMouseCameraOrbitDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mouse Camera Orbit");
    od->SetDescription("Makes a Camera orbit round a 3D Entity using Mouse.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
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
    od->SetGuid(CKGUID(0x2356342f, 0x9542674f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMouseCameraOrbitProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Prototype creation
//
//////////////////////////////////////////////////////////////////////////////

CKERROR CreateMouseCameraOrbitProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Mouse Camera Orbit");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // General Description
    CKERROR error = FillGeneralCameraOrbitProto(proto);
    if (error != CK_OK)
        return (error);

    // Additional Input and Output
    proto->DeclareInput("Resume");
    proto->DeclareOutput("Exit Resume");

    // Set the execution functions
    proto->SetFunction(MouseCameraOrbit);
    proto->SetBehaviorCallbackFct(MouseCameraOrbitCallback, CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORLOAD | CKCB_BEHAVIORCREATE);

    // return OK
    *pproto = proto;
    return CK_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Main Function
//
//////////////////////////////////////////////////////////////////////////////
int MouseCameraOrbit(const CKBehaviorContext &behcontext)
{
    return (GeneralCameraOrbit(behcontext, ProcessMouseInputs));
}

//////////////////////////////////////////////////////////////////////////////
//
//						Function that process the inputs
//
//////////////////////////////////////////////////////////////////////////////
void ProcessMouseInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping)
{
    // Is the move limited ?
    CKBOOL Limited = TRUE;
    beh->GetLocalParameterValue(LOCAL_LIMITS, &Limited);

    // Gets the mouse displacement informations
    VxVector mouseDisplacement;
    input->GetMouseRelativePosition(mouseDisplacement);

    ////////////////////
    // Resume Input
    ////////////////////
    if (Returns && beh->IsInputActive(2))
    {
        // Input / Output Status
        beh->ActivateInput(2, FALSE);
        beh->ActivateOutput(2);

        // We're not stopping anymore
        stopping = FALSE;
        beh->SetLocalParameterValue(LOCAL_STOPPING, &stopping);
    }

    ////////////////////
    // Position Update
    ////////////////////
    // If the users is moving the camera
    if (!stopping)
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

            RotationAngles->x -= mouseDisplacement.x * SpeedAngle;
            RotationAngles->x = XMin(RotationAngles->x, MaxH);
            RotationAngles->x = XMax(RotationAngles->x, MinH);

            RotationAngles->y -= mouseDisplacement.y * SpeedAngle;
            RotationAngles->y = XMin(RotationAngles->y, MaxV);
            RotationAngles->y = XMax(RotationAngles->y, MinV);
        }
        else
        {
            RotationAngles->x -= mouseDisplacement.x * SpeedAngle;
            RotationAngles->y -= mouseDisplacement.y * SpeedAngle;
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
    if ((mouseDisplacement.z != 0) && !stopping)
    {
        float ZoomSpeed = 40.0f;
        beh->GetInputParameterValue(IN_SPEED_ZOOM, &ZoomSpeed);
        ZoomSpeed *= delta / 10000;

        if (Limited)
        {
            float MinZoom = -40.0f;
            beh->GetInputParameterValue(IN_MIN_ZOOM, &MinZoom);

            float MaxZoom = 10.0f;
            beh->GetInputParameterValue(IN_MAX_ZOOM, &MaxZoom);

            RotationAngles->z -= mouseDisplacement.z * ZoomSpeed;
            RotationAngles->z = XMin(RotationAngles->z, -MinZoom);
            RotationAngles->z = XMax(RotationAngles->z, -MaxZoom);
        }
        else
            RotationAngles->z -= mouseDisplacement.z * ZoomSpeed;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//						Mouse Specific Callback Function
//
//////////////////////////////////////////////////////////////////////////////
CKERROR MouseCameraOrbitCallback(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    switch (context.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        VxVector InitialAngles(INF, INF, INF);
        beh->SetLocalParameterValue(LOCAL_INIT, &InitialAngles);
    }
    break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // Update the needed input for "returns"
        CKBOOL Returns = TRUE;
        beh->GetLocalParameterValue(LOCAL_RETURN, &Returns);
        beh->EnableInputParameter(IN_SPEED_RETURN, Returns);
        if (Returns && (beh->GetInputCount() == 2))
        {
            beh->AddInput("Resume");
            beh->AddOutput("Exit Resume");
        }
        else if (!Returns && (beh->GetOutputCount() == 3))
        {
            beh->DeleteInput(2);
            beh->DeleteOutput(2);
        }

        // Updates the needed inputs for limiting the move
        CKBOOL Limited = TRUE;
        beh->GetLocalParameterValue(LOCAL_LIMITS, &Limited);
        beh->EnableInputParameter(IN_MIN_H, Limited);
        beh->EnableInputParameter(IN_MAX_H, Limited);
        beh->EnableInputParameter(IN_MIN_V, Limited);
        beh->EnableInputParameter(IN_MAX_V, Limited);
        beh->EnableInputParameter(IN_MIN_ZOOM, Limited);
        beh->EnableInputParameter(IN_MAX_ZOOM, Limited);
    }
    break;

    default:
        break;
    }

    return CKBR_OK;
}
