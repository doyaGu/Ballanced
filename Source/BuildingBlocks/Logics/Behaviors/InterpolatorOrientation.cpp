/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Interpolator Orientation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorInterpolatorOrientationDecl();
CKERROR CreateInterpolatorOrientationProto(CKBehaviorPrototype **);
int InterpolatorOrientation(const CKBehaviorContext &behcontext);
CKERROR InterpolatorOrientationCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorInterpolatorOrientationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator Object Orientation");
    od->SetDescription("Performs an interpolation between orientations of two given 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first 3D Entity which matrix will be taken.<BR>
    <SPAN CLASS=pin>B: </SPAN>second 3D Entity which matrix will be taken.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <SPAN CLASS=pin>Scale: </SPAN>the resulted normalized matrix is multiply with this scale value (useful if the object receiving this matrix has a non-unitary scale). This parameter is used and visible only if the setting 'Objects are all 1x1x1 scaled' is set to TRUE.<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated matrix.<BR>
    <BR>
    <SPAN CLASS=setting>Objects are all 1x1x1 scaled: </SPAN>if set to TRUE, then no scale calculation is performed, and process time is saved.
    You should set this parameter to TRUE if you know all your objects to be 1x1x1 scaled.<BR>
    <BR>
    If Value=0% then C=A matrix, if Value=100% then C=B matrix.<BR>
    */
    od->SetCategory("Logics/Interpolator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17171704, 0x17171704));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorOrientationProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateInterpolatorOrientationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator Object Orientation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_3DENTITY);
    proto->DeclareInParameter("B", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE);
    proto->DeclareInParameter("Scale", CKPGUID_VECTOR, "1,1,1");

    proto->DeclareOutParameter("C", CKPGUID_MATRIX);

    proto->DeclareSetting("Objects are all 1x1x1 scaled", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(InterpolatorOrientation);
    proto->SetBehaviorCallbackFct(InterpolatorOrientationCallBack);

    *pproto = proto;
    return CK_OK;
}

int InterpolatorOrientation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxQuaternion quatA, quatB, quatC;
    float value;

    CKBOOL unit_scale = FALSE;
    beh->GetLocalParameterValue(0, &unit_scale);

    CK3dEntity *ckA = (CK3dEntity *)beh->GetInputParameterObject(0);
    quatA.FromMatrix(ckA->GetWorldMatrix(), unit_scale); // get unitary quatA

    CK3dEntity *ckB = (CK3dEntity *)beh->GetInputParameterObject(1);
    quatB.FromMatrix(ckB->GetWorldMatrix(), unit_scale); // get unitary quatB

    beh->GetInputParameterValue(2, &value);

    quatC = Slerp(value, quatA, quatB); // interpolates unitary quaternions
    VxMatrix matC;
    quatC.ToMatrix(matC); // retrieve a unitary matrix

    if (!unit_scale)
    {
        VxVector scale(1, 1, 1);
        beh->GetInputParameterValue(3, &scale);
        if (scale != VxVector::axis1())
        {
            // set a scale if and only if:
            // unit_scale is not check
            // and the given scale isn't (1,1,1)
            matC[0][0] *= scale.x;
            matC[0][1] *= scale.x;
            matC[0][2] *= scale.x;
            matC[1][0] *= scale.y;
            matC[1][1] *= scale.y;
            matC[1][2] *= scale.y;
            matC[2][0] *= scale.z;
            matC[2][1] *= scale.z;
            matC[2][2] *= scale.z;
        }
    }
    beh->SetOutputParameterValue(0, &matC);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR InterpolatorOrientationCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL unit_scale = TRUE;
        beh->GetLocalParameterValue(0, &unit_scale);

        if (!unit_scale && !beh->GetInputParameter(3)) // add scale param
        {
            beh->CreateInputParameter("Scale", CKPGUID_VECTOR);
        }

        if (unit_scale && beh->GetInputParameter(3)) // remove scale param
        {
            CKDestroyObject(beh->RemoveInputParameter(3));
        }
    }
    break;
    }

    return CKBR_OK;
}
