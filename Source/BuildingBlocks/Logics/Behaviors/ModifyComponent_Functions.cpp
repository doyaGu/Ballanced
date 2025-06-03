// Modifying Function use by behavior (see "ModifyComponent.h")

#include "CKAll.h"
#include "ModifyComponent.h"

void Modify2DVector(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    Vx2DVector v;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &v);
    int parameterInputIndex = 1;

    //-----X-----
    if (flags & V2_X)
        beh->GetInputParameterValue(parameterInputIndex++, &v.x);

    //-----Y-----
    if (flags & V2_Y)
        beh->GetInputParameterValue(parameterInputIndex++, &v.y);

    beh->SetOutputParameterValue(POUT_VOUT, &v);
}

void ModifyBox(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    VxBbox b;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &b);
    int parameterInputIndex = 1;

    //-----V1-----
    if (flags & B_V1)
        beh->GetInputParameterValue(parameterInputIndex++, &b.Min);

    //-----V2-----
    if (flags & B_V2)
        beh->GetInputParameterValue(parameterInputIndex++, &b.Max);

    beh->SetOutputParameterValue(POUT_VOUT, &b);
}

void ModifyColor(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    VxColor c;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &c);
    int parameterInputIndex = 1;

    //-----R-----
    if (flags & C_R)
        beh->GetInputParameterValue(parameterInputIndex++, &c.r);

    //-----G-----
    if (flags & C_G)
        beh->GetInputParameterValue(parameterInputIndex++, &c.g);

    //-----B-----
    if (flags & C_B)
        beh->GetInputParameterValue(parameterInputIndex++, &c.b);

    //-----A-----
    if (flags & C_A)
        beh->GetInputParameterValue(parameterInputIndex++, &c.a);

    beh->SetOutputParameterValue(POUT_VOUT, &c);
}

void ModifyEulerAngles(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    VxVector v;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &v);
    int parameterInputIndex = 1;

    //-----A1-----
    if (flags & E_A1)
        beh->GetInputParameterValue(parameterInputIndex++, &v.x);

    //-----A2-----
    if (flags & E_A2)
        beh->GetInputParameterValue(parameterInputIndex++, &v.y);

    //-----A3-----
    if (flags & E_A3)
        beh->GetInputParameterValue(parameterInputIndex++, &v.z);

    beh->SetOutputParameterValue(POUT_VOUT, &v);
}

void ModifyQuaternion(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    VxQuaternion q;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &q);
    int parameterInputIndex = 1;

    //-----X-----
    if (flags & Q_X)
        beh->GetInputParameterValue(parameterInputIndex++, &q.x);

    //-----Y-----
    if (flags & Q_Y)
        beh->GetInputParameterValue(parameterInputIndex++, &q.y);

    //-----Z-----
    if (flags & Q_Z)
        beh->GetInputParameterValue(parameterInputIndex++, &q.z);

    //-----Q-----
    if (flags & Q_W)
        beh->GetInputParameterValue(parameterInputIndex++, &q.w);

    beh->SetOutputParameterValue(POUT_VOUT, &q);
}

void ModifyRect(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    VxRect r;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &r);
    int parameterInputIndex = 1;

    //-----V1-----
    if (flags & R_V1)
        beh->GetInputParameterValue(parameterInputIndex++, &r.GetTopLeft());

    //-----V2-----
    if (flags & R_V2)
        beh->GetInputParameterValue(parameterInputIndex++, &r.GetBottomRight());

    beh->SetOutputParameterValue(POUT_VOUT, &r);
}

void ModifyVector(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    VxVector v;
    CKBehavior *beh = BehContext.Behavior;
    beh->GetInputParameterValue(PIN_VIN, &v);
    int parameterInputIndex = 1;

    //-----X-----
    if (flags & V_X)
        beh->GetInputParameterValue(parameterInputIndex++, &v.x);

    //-----Y-----
    if (flags & V_Y)
        beh->GetInputParameterValue(parameterInputIndex++, &v.y);

    //-----Z-----
    if (flags & V_Z)
        beh->GetInputParameterValue(parameterInputIndex++, &v.z);

    beh->SetOutputParameterValue(POUT_VOUT, &v);
}
