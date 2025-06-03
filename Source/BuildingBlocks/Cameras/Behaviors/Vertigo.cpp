/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Vertigo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorVertigoDecl();
CKERROR CreateVertigoProto(CKBehaviorPrototype **pproto);
int Vertigo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorVertigoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Vertigo");
    od->SetDescription("Creates a 'Vertigo' effect on the Camera.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out:</SPAN>is activated when the process needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Focal Point: </SPAN>3D Entity that determines the constant focal plane for the camera during the effect.<BR>
    <SPAN CLASS=pin>Number of Frames: </SPAN>number of frames in which the effect will be performed.<BR>
    <SPAN CLASS=pin>Forward: </SPAN>'True' moves the camera forward, 'False' moves it backward.<BR>
    <SPAN CLASS=pin>Distortion Amount: </SPAN>amount of distorsion expressed in percentage.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>progression curve along the camera Z axis.<BR>
    <BR>
    This behavior creates a 'Vertigo' effect by moving the camera forward (or backward) along its Z axis while adjusting the field of view so that the objects in its focal plan remain the same size on the screen (while the others are deformed).
    Typically, this is used to produce dizziness or to increase tension before a scary event...
    */
    /* warning:
    - As a old stuff, this building block is not time based at all. Therefore it is frame rate dependant.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xb00d010a, 0xc00d010a));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateVertigoProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    od->SetCategory("Cameras/FX");
    return od;
}

CKERROR CreateVertigoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Vertigo");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, 0); //"stepsRemaining"
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);  //"projectionPlaneSize"
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);  //"curveLastX"
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);  //"curveDeltaX"
    proto->DeclareLocalParameter(NULL, CKPGUID_VECTOR); //"goingVector"

    proto->DeclareInParameter("Focal Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Number of Frames", CKPGUID_INT, "100");
    proto->DeclareInParameter("Forward", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Distortion Amount", CKPGUID_PERCENTAGE, "50");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Vertigo);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Vertigo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the camera entity
    CKCamera *ent = (CKCamera *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    VxVector goingVector;

    // we get the steps remaining (local parameter)
    int stepsRemaining;
    beh->GetLocalParameterValue(0, &stepsRemaining);

    if (beh->IsInputActive(0))
    { // first time in a loop vertigo is launched
        // we get the velocity (input parameter)
        int nb_frame = 100;
        beh->GetInputParameterValue(1, &nb_frame);

        // we calculate the steps remaining
        stepsRemaining = nb_frame;

        // we get the object position
        CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(0);
        if (!e)
            return CKBR_PARAMETERERROR;

        VxVector objectPosition;
        e->GetPosition(&objectPosition, ent);

        // we get the effect value
        float effectValue = 0.5f;
        beh->GetInputParameterValue(3, &effectValue);

        // we get the direction
        CKBOOL direction = TRUE;
        beh->GetInputParameterValue(2, &direction);

        // we calculate the goingVector
        if (direction)
        { // camera -> object
            goingVector.x = 0;
            goingVector.y = 0;
            goingVector.z = objectPosition.z;
        }
        else
        { // <- camera object
            goingVector.x = 0;
            goingVector.y = 0;
            goingVector.z = -objectPosition.z;
            effectValue = effectValue / (1.0f - effectValue);
        }
        float D = Magnitude(goingVector);
        goingVector *= effectValue;

        // we calculate the planeProjectionSize
        float fov = ent->GetFov();
        float planeProjectionSize;
        planeProjectionSize = (float)(D * tan(fov / 2.0f));

        // we calculate the curveDeltaX
        float curveDeltaX;
        curveDeltaX = 1.0f / stepsRemaining;

        ///////////////////////////////
        // writing the local parameters

        // writing the stepsRemaining
        beh->SetLocalParameterValue(0, &stepsRemaining);

        // writing the planeProjectionSize
        beh->SetLocalParameterValue(1, &planeProjectionSize);

        // writing the curveLastX
        float clx = 0.0;
        beh->SetLocalParameterValue(2, &clx);

        // writing the curveDeltaX
        beh->SetLocalParameterValue(3, &curveDeltaX);

        // writing the goingVector
        beh->SetLocalParameterValue(4, &goingVector);

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }
    else
    { // we are in the loop
        beh->ActivateInput(1, FALSE);
        ///////////////////////////////
        // getting the input parameters

        // getting the 2d curve
        CK2dCurve *accelerationCurve = NULL;
        beh->GetInputParameterValue(4, &accelerationCurve);

        ///////////////////////////////
        // getting the local parameters

        // getting the planeProjectionSize
        float planeProjectionSize;
        beh->GetLocalParameterValue(1, &planeProjectionSize);

        // getting the curveLastX
        float curveLastX;
        beh->GetLocalParameterValue(2, &curveLastX);

        // getting the curveDeltaX
        float curveDeltaX;
        beh->GetLocalParameterValue(3, &curveDeltaX);

        // getting the goingVector
        VxVector goingVector;
        beh->GetLocalParameterValue(4, &goingVector);

        // calculating the deltaY of the curve
        float deltaY = accelerationCurve->GetY(curveLastX + curveDeltaX) -
                       accelerationCurve->GetY(curveLastX);

        // translating the camera
        VxVector trans = goingVector * deltaY;
        ent->Translate(&trans, ent);

        // calculating the new D
        // we get the object position
        CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(0);
        VxVector objectPosition;
        e->GetPosition(&objectPosition, ent);
        float D = (float)fabs(objectPosition.z);

        // calculating the fov
        float fov = 2.0f * atanf(planeProjectionSize / D);

        // changing the fov
        ent->SetFov(fov);

        ///////////////////////////////
        // writing the local parameters

        // writing the stepsRemaining
        stepsRemaining--;
        beh->SetLocalParameterValue(0, &stepsRemaining);

        // writing the curveLastX
        curveLastX += curveDeltaX;
        beh->SetLocalParameterValue(2, &curveLastX);

        if (stepsRemaining)
        { // vertigo not finished
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
        else
        { // loop accomplished
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }
    return CKBR_OK;
}
