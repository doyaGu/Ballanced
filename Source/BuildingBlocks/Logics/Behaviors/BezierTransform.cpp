/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            BezierTransform
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBezierTransformDecl();
CKERROR CreateBezierTransformProto(CKBehaviorPrototype **);
int BezierTransform(const CKBehaviorContext &behcontext);
CKERROR BezierTransformCallBack(const CKBehaviorContext &behcontext); // CallBack Function

CKObjectDeclaration *FillBehaviorBezierTransformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Bezier Transform");
    od->SetDescription("Transform a float value into another float value according to a 2d bezier curve shape, and given boundaries.");
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6d433e1b, 0x191954b4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBezierTransformProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBezierTransformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Bezier Transform");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("X", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Boundaries", CKPGUID_RECT, "(0,1),(1,0)");

    proto->DeclareOutParameter("Y", CKPGUID_FLOAT);

    proto->DeclareSetting("Use 4 Floats As Boundaries", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BezierTransform);
    proto->SetBehaviorCallbackFct(BezierTransformCallBack);

    *pproto = proto;
    return CK_OK;
}

int BezierTransform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // io activation
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    // X
    float X = 0;
    beh->GetInputParameterValue(0, &X);

    // Progression Curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);
    if (!curve)
        return CKBR_OK;

    // use 4 floats as boundaries ?
    CKBOOL use4Floats = FALSE;
    beh->GetLocalParameterValue(0, &use4Floats);

    // Boundaries
    VxRect rect(1.0f, 0.0f, 0.0f, 1.0f);

    if (use4Floats)
    {
        beh->GetInputParameterValue(2, &rect.left);
        beh->GetInputParameterValue(3, &rect.right);
        beh->GetInputParameterValue(4, &rect.bottom);
        beh->GetInputParameterValue(5, &rect.top);
    }
    else
    {
        beh->GetInputParameterValue(2, &rect);
    }

    float Y = rect.bottom + (rect.top - rect.bottom) * curve->GetY((X - rect.left) / (rect.right - rect.left));

    // output result
    beh->SetOutputParameterValue(0, &Y);

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR BezierTransformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL use4Floats = FALSE;
        beh->GetLocalParameterValue(0, &use4Floats);

        CKParameterIn *pin = beh->GetInputParameter(2);
        if (!pin)
            break;

        VxRect rect(1.0f, 0.0f, 0.0f, 1.0f);

        if ((pin->GetGUID() == CKPGUID_RECT) && use4Floats)
        {
            // use 4 floats instead of rect
            pin->SetGUID(CKPGUID_FLOAT, TRUE, "Min X");
            beh->CreateInputParameter("Max X", CKPGUID_FLOAT);
            beh->CreateInputParameter("Min Y", CKPGUID_FLOAT);
            beh->CreateInputParameter("Max Y", CKPGUID_FLOAT);
            break;
        }

        if ((pin->GetGUID() == CKPGUID_FLOAT) && !use4Floats)
        {
            // use rect instead of 4 floats
            pin->SetGUID(CKPGUID_RECT, TRUE, "Boundaries");
            CKDestroyObject(beh->RemoveInputParameter(5));
            CKDestroyObject(beh->RemoveInputParameter(4));
            CKDestroyObject(beh->RemoveInputParameter(3));
            break;
        }
    }
    break;
    }

    return CKBR_OK;
}