///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//	            Functions for the Camera Orbits behaviors
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "CKAll.h"
#include "GeneralCameraOrbit.h"

//////////////////////////////////////////////////////////////////////////////
//
//							General Creation Function
//
// Note : Creates the common inputs, outputs, settings and local parameters.
//		  And sets the common flags.
//
//////////////////////////////////////////////////////////////////////////////
CKERROR FillGeneralCameraOrbitProto(CKBehaviorPrototype *proto)
{
    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    // Default Inputs
    proto->DeclareInParameter("Target Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Target Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Move Speed", CKPGUID_ANGLE, "0:100");
    proto->DeclareInParameter("Return Speed", CKPGUID_ANGLE, "0:200");
    proto->DeclareInParameter("Min Horizontal", CKPGUID_ANGLE, "0:-180");
    proto->DeclareInParameter("Max Horizontal", CKPGUID_ANGLE, "0:180");
    proto->DeclareInParameter("Min Vertical", CKPGUID_ANGLE, "0:-180");
    proto->DeclareInParameter("Max Vertical", CKPGUID_ANGLE, "0:180");
    proto->DeclareInParameter("Zoom Speed", CKPGUID_FLOAT, "40");
    proto->DeclareInParameter("Zoom Min", CKPGUID_FLOAT, "-40");
    proto->DeclareInParameter("Zoom Max", CKPGUID_FLOAT, "10");

    proto->DeclareLocalParameter("Initial Angles & Radius", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("Rotation Angles & Radius", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("Stopping", CKPGUID_BOOL, "TRUE");

    proto->DeclareSetting("Limits", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Returns", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    return CK_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Main General Function
//
// Note : We use the polar angle of the camera in the target referential to
// turn around it. Therefore, if the target moves, the camera moves with it
// to stay around it.
// Takes a specific input processing function.
//
//////////////////////////////////////////////////////////////////////////////
int GeneralCameraOrbit(const CKBehaviorContext &behcontext, INPUTPROCESSFUNCTION InputFunction)
{
    // Get the behavior
    CKBehavior *beh = behcontext.Behavior;
    float delta = behcontext.DeltaTime;

    // Current state of the behavior
    CKBOOL stopping;

    // Indicates the behavior we are stopping if returns, stop otherwise
    CKBOOL Returns = TRUE;
    beh->GetLocalParameterValue(LOCAL_RETURN, &Returns);

    // Stop Entrance, we stop
    if (beh->IsInputActive(1))
    {
        // Set IO State
        beh->ActivateInput(1, FALSE);

        // A protection in case we stop the behavior before having started it
        VxVector InitialAngles;
        beh->GetLocalParameterValue(LOCAL_INIT, &InitialAngles);

        // Get current position
        VxVector RotationAngles;
        beh->GetLocalParameterValue(LOCAL_ROT, &RotationAngles);

        // Stopping Now
        stopping = TRUE;
        beh->SetLocalParameterValue(LOCAL_STOPPING, &stopping);
        if (!Returns || ((RotationAngles.x == 0) && (RotationAngles.y == 0)))
        {
            beh->ActivateOutput(1, TRUE);
            return CKBR_OK;
        }
    }

    // Gets the current camera
    CKCamera *Camera = (CKCamera *)beh->GetTarget();
    if (!Camera)
        return CKBR_OWNERERROR;

    // Gets the target information
    VxVector TargetPosition;
    beh->GetInputParameterValue(IN_TARGET_POS, &TargetPosition);

    CK3dEntity *TargetRef = (CK3dEntity *)beh->GetInputParameterObject(IN_TARGET_REF);

    //////////////////////////////////////////////
    // Gets the input parameters of the behavior
    //////////////////////////////////////////////
    VxVector InitialAngles(0, 0, 0), RotationAngles(0, 0, 0);

    // First entrance, sets the initial values here only
    if (beh->IsInputActive(0))
    {
        // Sets IO State
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Not Stopping
        stopping = FALSE;
        beh->SetLocalParameterValue(LOCAL_STOPPING, &stopping);

        // Compute the Initial Angles, the radius
        VxVector InitialPosition;
        Camera->GetPosition(&InitialPosition, TargetRef);

        InitialPosition -= TargetPosition;
        InitialAngles.z = Magnitude(InitialPosition);

        // Vertical Polar Angle
        float d;
        InitialPosition.Normalize();
        d = DotProduct(InitialPosition, VxVector::axisY());
        // bound the value of d to avoid errors due to precision.
        if (d < -1)
            d = -1;
        else if (d > 1)
            d = 1;
        InitialAngles.y = acosf(d);

        // Horizontal Polar Angle
        InitialPosition.y = 0;
        InitialPosition.Normalize();
        d = DotProduct(InitialPosition, VxVector::axisX());
        // bound the value of d to avoid errors due to precision.
        if (d < -1)
            d = -1;
        else if (d > 1)
            d = 1;
        InitialAngles.x = acosf(d);
        d = DotProduct(InitialPosition, VxVector::axisZ());
        if (d < 0)
            InitialAngles.x *= -1;

        // In case the camera has the head down, we need to inverse the commands when we go
        // Up otherwise, the user will have the feeling to turn on the wrong direction.
        VxVector CamUp;
        Camera->GetOrientation(NULL, &CamUp, NULL, NULL);
        if (CamUp.y < 0)
        {
            InitialAngles.x += PI;
            InitialAngles.y *= -1;
            InitialAngles.x = (float)fmod(InitialAngles.x, 2 * PI);
        }

        // Reset stopping
        stopping = FALSE;

        // Sets the values in memory
        beh->SetLocalParameterValue(LOCAL_INIT, &InitialAngles);
        beh->SetLocalParameterValue(LOCAL_ROT, &RotationAngles);
        beh->SetLocalParameterValue(LOCAL_STOPPING, &stopping);

        // Only initialization on "On" entrance
        return CKBR_ACTIVATENEXTFRAME;
    }

    // Auto-activation of the behavior
    beh->GetLocalParameterValue(LOCAL_INIT, &InitialAngles);
    beh->GetLocalParameterValue(LOCAL_ROT, &RotationAngles);
    beh->GetLocalParameterValue(LOCAL_STOPPING, &stopping);

    // Get the input manager
    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);

    // Call the input processing function
    InputFunction(&RotationAngles, beh, input, delta, Returns, stopping);

    // Do nothing when initial angle were not initialized.
    // Simply stop the BB. No output activated.
    if ((InitialAngles.x == INF) || (RotationAngles.x == INF))
        return CKBR_OK;

    // When we are exactly on the top or the bottom of the target, +/-90�,
    // The LookAt BB won't rotate the camera when we turn right or left as
    // it already looks at the target. Therefore, when we are at +/-90�, we
    // add or remove a little something.
    if ((RotationAngles.y == PI / 2) || (RotationAngles.y == -PI / 2))
    {
        // Get Min, Max
        // If equals, nothing to change, it is the given value.
        float MinH = -PI / 2;
        float MaxH = PI / 2;
        beh->GetInputParameterValue(IN_MIN_H, &MinH);
        beh->GetInputParameterValue(IN_MAX_H, &MaxH);
        if (MaxH - MinH > 2 * STEP)
        {
            float sign = (RotationAngles.y > 0) ? 1.0f : -1.0f;
            if (MaxH >= sign * PI / 2 + STEP)
                RotationAngles.y += STEP;
            else
                RotationAngles.y -= STEP;
        }
    }

    // We memorize the new state with modulos
    RotationAngles.x = (float)fmod(RotationAngles.x, 2 * PI);
    RotationAngles.y = (float)fmod(RotationAngles.y, 2 * PI);
    beh->SetLocalParameterValue(LOCAL_ROT, &RotationAngles);

    // Computes the coordinates of the camera in the target referential thanks to the
    // current polar angles and radius information. And moves the camera
    VxVector Position;
    Position = InitialAngles + RotationAngles;
    Position = Position.z * (cosf(Position.x) * sinf(Position.y) * VxVector::axisX() + sinf(Position.x) * sinf(Position.y) * VxVector::axisZ() + cosf(Position.y) * VxVector::axisY());
    Position += TargetPosition;
    Camera->SetPosition(&Position, TargetRef, FALSE);

    // Does the camera has a Target ?
    CK3dEntity *CameraTarget;
    CKBOOL CameraHasTarget = CKIsChildClassOf(Camera, CKCID_TARGETCAMERA) && (CameraTarget = ((CKTargetCamera *)Camera)->GetTarget());

    // Orients the Camera. The LookAt implies that when the camera reach
    // the 90 degree, it may be flipped and suddenly have the head up and down.
    // Therefore, we use the target for target cameras as we have to use the
    // target. But for free cameras, we do our own look at using our rotation
    // angle to avoid this problem.
    if (CameraHasTarget)
        CameraTarget->SetPosition(&TargetPosition, TargetRef, TRUE);
    else
    {
        // New direction
        VxVector Dir;
        Dir = TargetPosition - Position;

        // Temp Right Value
        VxMatrix mat;
        VxVector R(0, 0, -1); // Up for (0,0) angle
        Vx3DMatrixFromRotationAndOrigin(mat, VxVector::axisY(), VxVector(0, 0, 0), InitialAngles.x + RotationAngles.x);
        R.Rotate(mat);

        // Get Up
        VxVector Up;
        Up = CrossProduct(R, Dir);

        Camera->SetOrientation(&Dir, &Up, NULL, TargetRef);
        // Camera->LookAt(&TargetPosition,TargetRef);
    }

    // Stop is finished, reset values.
    if (stopping && ((RotationAngles.x == 0) && (RotationAngles.y == 0)))
    {
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }
    else
        // Come back next frame
        return CKBR_ACTIVATENEXTFRAME;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Callback Function
//
//////////////////////////////////////////////////////////////////////////////
CKERROR GeneralCameraOrbitCallback(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    switch (context.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        VxVector InitialAngles(INF, INF, INF);
        beh->SetLocalParameterValue(LOCAL_INIT, &InitialAngles);

        VxVector RotationAngles(0, 0, 0);
        beh->SetLocalParameterValue(LOCAL_ROT, &RotationAngles);
    }
    break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // Update the needed input for "returns"
        CKBOOL Returns = TRUE;
        beh->GetLocalParameterValue(LOCAL_RETURN, &Returns);
        beh->EnableInputParameter(IN_SPEED_RETURN, Returns);

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
