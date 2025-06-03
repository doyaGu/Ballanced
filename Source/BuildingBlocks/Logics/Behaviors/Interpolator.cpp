/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Interpolator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Interpoltor Functions
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#include "hsv_rgb.h"
int ColorHSVInterpolator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxColor c1;
    VxColor c2;

    VxColor res;

    float mix;

    beh->GetInputParameterValue(0, &c1);
    beh->GetInputParameterValue(1, &c2);
    beh->GetInputParameterValue(2, &mix);

    if (!(c1.r || c1.g || c1.b) || !(c2.r || c2.g || c2.b)) // C1 or c2 is black
    {
        res.r = (1.0f - mix) * c1.r + mix * c2.r;
        res.g = (1.0f - mix) * c1.g + mix * c2.g;
        res.b = (1.0f - mix) * c1.g + mix * c2.b;
    }
    else
    {

        float h1, s1, v1;
        float h2, s2, v2;

        rgb_to_hsv(c1.r, c1.g, c1.b, &h1, &s1, &v1);
        rgb_to_hsv(c2.r, c2.g, c2.b, &h2, &s2, &v2);

        if (h1 > 180.0f)
            h1 = h1 - 360.0f;

        if (h2 > 180.0f)
            h2 = h2 - 360.0f;

        float x1 = (float)cos(h1 * PI / 180.0f) * s1;
        float y1 = (float)sin(h1 * PI / 180.0f) * s1;

        float x2 = (float)cos(h2 * PI / 180.0f) * s2;
        float y2 = (float)sin(h2 * PI / 180.0f) * s2;

        float vx = x2 - x1;
        float vy = y2 - y1;

        float x = x1 + vx * mix;
        float y = y1 + vy * mix;

        s1 = (float)sqrt(x * x + y * y);

        h1 = (float)asinf(y / s1) * 180.0f / PI;

        if (x < 0.0f)
        {
            if (y > 0.0f)
                h1 = 180 - h1;
            else
                h1 = -180.0f - h1;
        }

        v1 = (1.0f - mix) * v1 + mix * v2;

        if (h1 < 0.0f)
            h1 += 360.0f;

        hsv_to_rgb(h1, s1, v1, &res.r, &res.g, &res.b);
    }

    res.a = (1.0f - mix) * c1.a + mix * c2.a;

    beh->SetOutputParameterValue(0, &res);
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorColor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxColor A, B, C;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    C.r = A.r + (B.r - A.r) * value;
    C.g = A.g + (B.g - A.g) * value;
    C.b = A.b + (B.b - A.b) * value;
    C.a = A.a + (B.a - A.a) * value;

    beh->SetOutputParameterValue(0, &C);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorFloat(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float A, B, C;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    C = A + (B - A) * value;

    beh->SetOutputParameterValue(0, &C);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorInt(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int A, B, C;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    C = (int)(A + (B - A) * value);

    beh->SetOutputParameterValue(0, &C);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorMatrix(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxMatrix matA, matB, matC;
    float value;

    beh->GetInputParameterValue(0, &matA);
    beh->GetInputParameterValue(1, &matB);
    beh->GetInputParameterValue(2, &value);

    Vx3DInterpolateMatrix(value, matC, matA, matB);

    beh->SetOutputParameterValue(0, &matC);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorVector(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxVector A(0.0f), B(0.0f), C;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    C.x = A.x + (B.x - A.x) * value;
    C.y = A.y + (B.y - A.y) * value;
    C.z = A.z + (B.z - A.z) * value;

    beh->SetOutputParameterValue(0, &C);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int Interpolator2DVector(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    Vx2DVector A, B, C;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    C.x = A.x + (B.x - A.x) * value;
    C.y = A.y + (B.y - A.y) * value;

    beh->SetOutputParameterValue(0, &C);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorQuaternion(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxQuaternion A, B, C;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    C = Slerp(value, A, B); // interpolates unitary quaternions

    beh->SetOutputParameterValue(0, &C);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

int InterpolatorRectangle(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    VxRect A, B;
    float value;

    beh->GetInputParameterValue(0, &A);
    beh->GetInputParameterValue(1, &B);
    beh->GetInputParameterValue(2, &value);

    A.Interpolate(value, B);
    beh->SetOutputParameterValue(0, &A);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CKObjectDeclaration *FillBehaviorInterpolatorDecl();
CKERROR CreateInterpolatorProto(CKBehaviorPrototype **);
CKERROR InterpolatorCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorInterpolatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator");
    od->SetDescription("Performs an interpolation between 2 parameters");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first value.<BR>
    <SPAN CLASS=pin>B: </SPAN>second value.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated value.<BR>
    <BR>
    This building block performs an interpolation between A and B, and put the result into C.<BR>
    If Value=0% then C=A, if Value=100% then C=B.<BR>
    This building block is a generic building block which can perform several kind of interpolation:<BR>
    - FLOAT interpolation<BR>
    - VECTOR2D interpolation<BR>
    - VECTOR interpolation<BR>
    - INTEGER interpolation<BR>
    - MATRIX interpolation<BR>
    - QUATERNION interpolation<BR>
    - EULER ANGLES interpolation<BR>
    - COLOR interpolation<BR>
    - HSV COLOR interpolation<BR>
    - RECTANGLE interpolation<BR>
    To use this building block as a Vector Interpolator, just change the type of the output parameter "C" (the type of the input parameters will be changed automatically).<BR>
    */
    /* warning:
    - If you want to perform an HSV Color interpolation, set the type of "C" output parameter to COLOR, and set the boolean value "Use HSV Interpolation for Color" to TRUE (edit settings).<BR>
    */
    od->SetCategory("Logics/Interpolator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x35503950, 0x2dde7a65));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateInterpolatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_FLOAT);
    proto->DeclareInParameter("B", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE, "50");
    proto->DeclareOutParameter("C", CKPGUID_FLOAT);

    proto->DeclareSetting("Use HSV Interpolation for Color", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(InterpolatorFloat);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(InterpolatorCallBack);

    *pproto = proto;
    return CK_OK;
}

CKERROR InterpolatorCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIOREDITED:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKParameterOut *pout = beh->GetOutputParameter(0);
        CKGUID guid_pout = pout->GetGUID();

        // if( guid_pout==CKPGUID_FLOAT || guid_pout==CKPGUID_PERCENTAGE || guid_pout==CKPGUID_ANGLE ){
        if (behcontext.ParameterManager->IsDerivedFrom(guid_pout, CKPGUID_FLOAT))
        {
            beh->SetFunction(InterpolatorFloat);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_VECTOR || guid_pout == CKPGUID_EULERANGLES)
        {
            beh->SetFunction(InterpolatorVector);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_2DVECTOR)
        {
            beh->SetFunction(Interpolator2DVector);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_INT)
        {
            beh->SetFunction(InterpolatorInt);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_COLOR)
        {
            CKBOOL useHSV = FALSE;
            beh->GetLocalParameterValue(0, &useHSV);
            beh->SetFunction(useHSV ? ColorHSVInterpolator : InterpolatorColor);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_MATRIX)
        {
            beh->SetFunction(InterpolatorMatrix);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_QUATERNION)
        {
            beh->SetFunction(InterpolatorQuaternion);
            goto COMMON;
        }
        if (guid_pout == CKPGUID_RECT)
        {
            beh->SetFunction(InterpolatorRectangle);
            goto COMMON;
        }

        return CKBR_OK;

    COMMON:
        // change type of input params
        CKParameterIn *pin = beh->GetInputParameter(0);
        if (pin->GetGUID() != guid_pout)
            pin->SetGUID(guid_pout, TRUE);
        pin = beh->GetInputParameter(1);
        if (pin->GetGUID() != guid_pout)
            pin->SetGUID(guid_pout, TRUE);
    }
    break;
    }

    return CKBR_OK;
}
