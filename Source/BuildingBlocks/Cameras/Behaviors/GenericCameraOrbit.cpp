/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	            Generic Camera Orbit
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "GeneralCameraOrbit.h"

CKObjectDeclaration *FillBehaviorGenericCameraOrbitDecl();
CKERROR CreateGenericCameraOrbitProto(CKBehaviorPrototype **pproto);
int GenericCameraOrbit(const CKBehaviorContext &behcontext);
void ProcessGenericInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping);

CKObjectDeclaration *FillBehaviorGenericCameraOrbitDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Generic Camera Orbit");
    od->SetDescription("Makes a Camera orbit round a 3D Entity using input activation.");
    od->SetCategory("Cameras/Movement");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x104d0913, 0x766563dd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGenericCameraOrbitProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

//////////////////////////////////////////////////////////////////////////////
//
//							Prototype creation
//
//////////////////////////////////////////////////////////////////////////////
CKERROR CreateGenericCameraOrbitProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Generic Camera Orbit");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // General Description
    CKERROR error = FillGeneralCameraOrbitProto(proto);
    if (error != CK_OK)
        return (error);

    // Additionnal Inputs
    proto->DeclareInput("Move Left");
    proto->DeclareInput("Move Right");
    proto->DeclareInput("Move Up");
    proto->DeclareInput("Move Down");
    proto->DeclareInput("Zoom In");
    proto->DeclareInput("Zoom Out");

    // Additionnal Local
    proto->DeclareLocalParameter("Returning", CKPGUID_BOOL, "FALSE");

    // Set the execution functions
    proto->SetFunction(GenericCameraOrbit);
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
int GenericCameraOrbit(const CKBehaviorContext &behcontext)
{
    return (GeneralCameraOrbit(behcontext, ProcessGenericInputs));
}

//////////////////////////////////////////////////////////////////////////////
//
//						Function that process the inputs
//
//////////////////////////////////////////////////////////////////////////////
void ProcessGenericInputs(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &stopping)
{
    // Is the move limited ?
    CKBOOL Limited = TRUE;
    beh->GetLocalParameterValue(LOCAL_LIMITS, &Limited);

    // In the behavioral engine, when the BB return CKBR_ACTIVATENEXTFRAME, and
    // is also called by another BB, it may happpen that it is called two times.
    // We could use priorities to avoid this problem, but we implement a more
    // automatic solution with this parameter.
    // The return will therefore happen after at least one frame without action.
    CKBOOL Returning = FALSE;
    beh->GetLocalParameterValue(LOCAL_RETURNING, &Returning);

    ////////////////////
    // Position Update
    ////////////////////
    // If the users is moving the camera
    if ((beh->IsInputActive(INPUT_LEFT) || beh->IsInputActive(INPUT_RIGHT) || beh->IsInputActive(INPUT_UP) || beh->IsInputActive(INPUT_DOWN)) && !stopping)
    {
        // Not Returning
        if (Returning)
        {
            Returning = FALSE;
            beh->SetLocalParameterValue(LOCAL_RETURNING, &Returning);
        }

        // Get current time dependent speed angle.
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

            if (beh->IsInputActive(INPUT_LEFT))
                RotationAngles->x += XMin(SpeedAngle, MaxH - RotationAngles->x);
            if (beh->IsInputActive(INPUT_RIGHT))
                RotationAngles->x -= XMin(SpeedAngle, RotationAngles->x - MinH);
            if (beh->IsInputActive(INPUT_UP))
                RotationAngles->y += XMin(SpeedAngle, MaxV - RotationAngles->y);
            if (beh->IsInputActive(INPUT_DOWN))
                RotationAngles->y -= XMin(SpeedAngle, RotationAngles->y - MinV);
        }
        else
        {
            if (beh->IsInputActive(INPUT_LEFT))
                RotationAngles->x += SpeedAngle;
            if (beh->IsInputActive(INPUT_RIGHT))
                RotationAngles->x -= SpeedAngle;
            if (beh->IsInputActive(INPUT_UP))
                RotationAngles->y += SpeedAngle;
            if (beh->IsInputActive(INPUT_DOWN))
                RotationAngles->y -= SpeedAngle;
        }

        // Deactivates Inputs
        beh->ActivateInput(INPUT_LEFT, FALSE);
        beh->ActivateInput(INPUT_RIGHT, FALSE);
        beh->ActivateInput(INPUT_UP, FALSE);
        beh->ActivateInput(INPUT_DOWN, FALSE);
    }
    else if (Returns && ((RotationAngles->x != 0) || (RotationAngles->y != 0)))
    {
        if (!Returning)
        {
            Returning = TRUE;
            beh->SetLocalParameterValue(LOCAL_RETURNING, &Returning);
            return;
        }

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
    // We are stopping and already stopped !
    else if (stopping)
        RotationAngles->x = INF;

    ////////////////
    // Zoom Update
    ////////////////
    if ((beh->IsInputActive(INPUT_ZOOMIN) || beh->IsInputActive(INPUT_ZOOMOUT)) && !stopping)
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

            if (beh->IsInputActive(INPUT_ZOOMIN))
                RotationAngles->z -= XMin(ZoomSpeed, RotationAngles->z + MaxZoom);
            if (beh->IsInputActive(INPUT_ZOOMOUT))
                RotationAngles->z += XMin(ZoomSpeed, -MinZoom - RotationAngles->z);
        }
        else
        {
            if (beh->IsInputActive(INPUT_ZOOMIN))
                RotationAngles->z -= ZoomSpeed;
            if (beh->IsInputActive(INPUT_ZOOMOUT))
                RotationAngles->z += ZoomSpeed;
        }

        // Deactivates Inputs
        beh->ActivateInput(INPUT_ZOOMIN, FALSE);
        beh->ActivateInput(INPUT_ZOOMOUT, FALSE);
    }
}
