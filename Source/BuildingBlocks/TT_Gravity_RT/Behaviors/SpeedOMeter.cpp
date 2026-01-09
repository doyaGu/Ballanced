/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT SpeedOMeter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

CKObjectDeclaration *FillBehaviorSpeedOMeterDecl();
CKERROR CreateSpeedOMeterProto(CKBehaviorPrototype **pproto);
int SpeedOMeter(const CKBehaviorContext &behcontext);
CKERROR SpeedOMeterCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSpeedOMeterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT SpeedOMeter");
    od->SetDescription("Calculates the speed of an 3DEntity.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x51bd5521, 0x672c67bf));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSpeedOMeterProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSpeedOMeterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT SpeedOMeter");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Absolute Speed", CKPGUID_FLOAT, "0.0");

    proto->DeclareLocalParameter("Last Position", CKPGUID_VECTOR);

    proto->DeclareSetting("Calculate relative speed?", CKPGUID_BOOL, "0");
    proto->DeclareSetting("Loop-Framedelay?", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SpeedOMeter);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SpeedOMeterCallBack);

    *pproto = proto;
    return CK_OK;
}

int SpeedOMeter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    CKBOOL calcRelativeSpeed;
    beh->GetLocalParameterValue(1, &calcRelativeSpeed);

    int loopFrameDelay;
    beh->GetLocalParameterValue(2, &loopFrameDelay);

    float delta = behcontext.DeltaTime;
    if (loopFrameDelay > 1)
        delta *= loopFrameDelay;

    VxVector lastPos;
    beh->GetLocalParameterValue(0, &lastPos);

    VxVector pos;
    target->GetPosition(&pos);

    VxVector difference = pos - lastPos;
    float speed = Magnitude(difference) * 1000.0f / delta;
    beh->SetOutputParameterValue(0, &speed);

    if (calcRelativeSpeed)
    {
        float minSpeed = 0.0f;
        beh->GetInputParameterValue(0, &minSpeed);
        float maxSpeed = 0.0f;
        beh->GetInputParameterValue(1, &maxSpeed);

        float range = maxSpeed - minSpeed;
        if (range != 0.0f)
            speed = (speed - minSpeed) / range;
        else
            speed = 0.0f;
        if (speed > 1.0f)
            speed = 1.0f;
        else if (speed < 0.0f)
            speed = 0.0f;

        beh->SetOutputParameterValue(1, &speed);
    }

    beh->SetLocalParameterValue(0, &pos);
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}

CKERROR SpeedOMeterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (behcontext.CallbackMessage == CKM_BEHAVIORSETTINGSEDITED)
    {
        CKBOOL calcRelativeSpeed;
        beh->GetLocalParameterValue(1, &calcRelativeSpeed);
        int inputParamCount = beh->GetInputParameterCount();
        if (calcRelativeSpeed)
        {
            if (inputParamCount == 0)
            {
                beh->CreateInputParameter("Minimum Speed", CKPGUID_FLOAT);
                beh->CreateInputParameter("Maximum Speed", CKPGUID_FLOAT);
                beh->CreateOutputParameter("Relative Speed", CKPGUID_FLOAT);
            }
        }
        else if (inputParamCount != 0)
        {
            CKParameterIn *pin = beh->RemoveInputParameter(0);
            CKDestroyObject(pin);
            pin = beh->RemoveInputParameter(0);
            CKDestroyObject(pin);
            CKParameterOut *pout = beh->RemoveOutputParameter(1);
            CKDestroyObject(pout);
        }
    }

    return CKBR_OK;
}
