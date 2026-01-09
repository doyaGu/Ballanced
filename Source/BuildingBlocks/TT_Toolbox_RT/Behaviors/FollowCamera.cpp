///////////////////////////////////
///////////////////////////////////
//
//        TT FollowCamera
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorFollowCameraDecl();
CKERROR CreateFollowCameraProto(CKBehaviorPrototype **pproto);
int FollowCamera(const CKBehaviorContext &behcontext);
CKERROR FollowCameraCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFollowCameraDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT FollowCamera");
    od->SetDescription("Camera follows an object");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x595b4723, 0x576c6749));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFollowCameraProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateFollowCameraProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT FollowCamera");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Stop");

    proto->DeclareInParameter("Target Entity", CKPGUID_3DENTITY);
    proto->DeclareInParameter("View Offset", CKPGUID_VECTOR);
    proto->DeclareInParameter("View Attenuation", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Camera Offset", CKPGUID_VECTOR);
    proto->DeclareInParameter("Camera Attenuation", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("FOV Angle", CKPGUID_ANGLE, "0:50");
    proto->DeclareInParameter("Back Attenuation", CKPGUID_PERCENTAGE, "25");

    proto->DeclareLocalParameter("ViewPosOld", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("CamPosOld", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("oldDeltaTime", CKPGUID_FLOAT);
    proto->DeclareLocalParameter("init", CKPGUID_BOOL, "true");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Init actPos", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FollowCamera);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(FollowCameraCallBack);

    *pproto = proto;
    return CK_OK;
}

int FollowCamera(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *camera = (CKCamera *)beh->GetTarget();
    if (!camera)
        return CKBR_OWNERERROR;

    CK3dEntity *targetEntity = (CK3dEntity *)beh->GetInputParameterObject(0);

    VxVector viewOffset, cameraOffset;
    float viewAtt, cameraAtt;
    float fovAngle, backAtt;

    viewOffset = VxVector::axis0();
    beh->GetInputParameterValue(1, &viewOffset);
    viewAtt = 1.0f;
    beh->GetInputParameterValue(2, &viewAtt);
    cameraOffset = VxVector::axis0();
    beh->GetInputParameterValue(3, &cameraOffset);
    cameraAtt = 1.0f;
    beh->GetInputParameterValue(4, &cameraAtt);
    beh->GetInputParameterValue(5, &fovAngle);
    backAtt = 0.25f;
    beh->GetInputParameterValue(6, &backAtt);

    VxMatrix mat = camera->GetWorldMatrix();
    if (mat[0][0] < 0.0f)
    {
        mat[0][0] = fabsf(mat[0][0]);
        mat[3][3] = fabsf(mat[3][3]);
        camera->SetWorldMatrix(mat);
    }

    camera->SetFov(fovAngle);

    VxVector viewPosOld = VxVector::axis0();
    VxVector camPosOld = VxVector::axis0();

    VxVector targetViewPos = VxVector::axis0();
    VxVector targetCamPos = VxVector::axis0();

    if (targetEntity)
    {
        targetEntity->Transform(&targetViewPos, &viewOffset);
        targetEntity->Transform(&targetCamPos, &cameraOffset);
    }

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        CKBOOL initActPos = FALSE;
        beh->GetLocalParameterValue(5, &initActPos);

        if (!initActPos)
        {
            viewPosOld = targetViewPos;
            camPosOld = targetCamPos;
        }
        else
        {
            VxVector pos;
            camera->GetPosition(&pos);
            camPosOld = pos;
            viewPosOld = targetViewPos;
        }

        beh->SetLocalParameterValue(0, &viewPosOld);
        beh->SetLocalParameterValue(1, &camPosOld);

        float deltaTime = behcontext.DeltaTime;
        beh->SetLocalParameterValue(2, &deltaTime);
    }

    beh->GetLocalParameterValue(0, &viewPosOld);
    beh->GetLocalParameterValue(1, &camPosOld);

    VxVector viewDelta = targetViewPos - viewPosOld;
    VxVector camDelta = targetCamPos - camPosOld;

    CKBOOL timeBased = FALSE;
    beh->GetLocalParameterValue(3, &timeBased);

    if (timeBased)
    {
        float deltaTime = behcontext.DeltaTime;
        float oldDeltaTime = 0.0f;
        beh->GetLocalParameterValue(2, &oldDeltaTime);

        float maxDelta = deltaTime * 1.1f;
        if (oldDeltaTime > maxDelta)
            deltaTime = maxDelta;

        float minDelta = deltaTime - deltaTime * 0.1f;
        if (oldDeltaTime >= minDelta)
            minDelta = deltaTime;

        float avgDelta = (oldDeltaTime * 4.0f + minDelta) * 0.2f;

        float viewFactor = avgDelta * viewAtt * 0.07f;
        if (viewFactor < 0.0f) viewFactor = 0.0f;
        if (viewFactor > 1.0f) viewFactor = 1.0f;

        float camFactor = avgDelta * cameraAtt * 0.07f;
        if (camFactor < 0.0f) camFactor = 0.0f;
        if (camFactor > 1.0f) camFactor = 1.0f;

        viewAtt = viewFactor;
        cameraAtt = camFactor;

        beh->SetLocalParameterValue(2, &avgDelta);
    }

    viewPosOld += viewDelta * viewAtt;
    camPosOld += camDelta * cameraAtt;

    VxVector camToView = viewPosOld - camPosOld;
    float camDist = Magnitude(cameraOffset);
    float viewDist = Magnitude(camToView);

    if (camDist > viewDist)
    {
        VxVector diff = targetCamPos - camPosOld;
        camPosOld += diff * backAtt;
    }

    camera->SetPosition(&camPosOld);
    camera->LookAt(&viewPosOld);

    beh->SetLocalParameterValue(0, &viewPosOld);
    beh->SetLocalParameterValue(1, &camPosOld);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR FollowCameraCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORPAUSE:
        {
            CKBOOL init = TRUE;
            beh->SetLocalParameterValue(5, &init);
        }
        break;
    default:
        break;
    }

    return CKBR_OK;
}