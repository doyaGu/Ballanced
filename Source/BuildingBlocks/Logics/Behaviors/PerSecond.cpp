/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PerSecond
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPerSecondDecl();
CKERROR CreatePerSecondProto(CKBehaviorPrototype **);
int PerSecond(const CKBehaviorContext &behcontext);
CKERROR PerSecondCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorPerSecondDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Per Second");
    od->SetDescription("Calculates a progression (Y) according to a given speed (X) : Y=X*Elapsed Time During One Frame. X can be either an Angle, a Float, a Percentage, Vector or a 2dVector.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>X: </SPAN>Input value to be concidered in 1 second.<BR>
    <BR>
    <SPAN CLASS=pout>Y: </SPAN>Output value, Y = X * Ellapsed Time (in s.). The elapsed time represents the duration of one rendering process.<BR>
    <BR>
    Examples of its use ...
    - Assume you need to make an object rotate at the speed of 5 degrees per second, just change 'Y' parameter type to an angle, and give 'X' the value 5. Then link 'Y' to the input parameter of the 'Rotate' building block, and loop.<BR>
    - Or assume you need to make an object translate in depth at the speed of 2 units per second, just change 'Y' parameter type to a vector, and give 'X' the value 5. Then link 'Y' to the input parameter of the 'Translate' building block, and loop.<BR>
    */
    /* warning:
    - You can change the type of 'Y', so it can be a 'Angle', 'Vector', '2dVector' or 'Float' (rem: float and angle are anyway compatible).<BR>
    Just change the 'Y' type, and the 'X' type will automatically be canged.
    -
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x448e54ce, 0x75a655c5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePerSecondProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreatePerSecondProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Per Second");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("pIn 0", CKPGUID_FLOAT);
    proto->DeclareOutParameter("pOut 0", CKPGUID_FLOAT);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PerSecond);

    proto->SetBehaviorCallbackFct(PerSecondCallBack);

    *pproto = proto;
    return CK_OK;
}

int PerSecond(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    //--------- manage in/outputs
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    const float delta = behcontext.DeltaTime * 0.001f;

    CKParameterOut *pout;
    CKParameter *param;
    const int pOutCount = beh->GetOutputParameterCount();
    float *fpout, *fpin;

    int a, b;

    for (b = 0; b < pOutCount; ++b)
    {

        param = beh->GetInputParameter(b)->GetRealSource();
        if (!param)
            continue;
        fpin = (float *)param->GetReadDataPtr();

        pout = beh->GetOutputParameter(b);
        fpout = (float *)pout->GetWriteDataPtr();

        const int fsize = pout->GetDataSize() >> 2;

        for (a = 0; a < fsize; ++a)
        {
            *fpout = *fpin * delta;
            ++fpin;
            ++fpout;
        }

        pout->DataChanged();
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR PerSecondCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterOut *pout;
        CKParameterIn *pin;
        CKGUID guid_out;
        char str[16];

        const int pOutCount = beh->GetOutputParameterCount();

        for (int a = 0; a < pOutCount; ++a)
        {

            if (pout = beh->GetOutputParameter(a))
            {
                guid_out = pout->GetGUID();

                // change type if it's an Integer	(classic case)
                if (guid_out == CKPGUID_INT)
                    pout->SetGUID(CKPGUID_FLOAT);

                pin = beh->GetInputParameter(a);

                if (pin)
                {
                    // we check the type of the 'Input Parameter'
                    if (pin->GetGUID() != guid_out)
                    {
                        pin->SetGUID(guid_out, TRUE);
                    }
                }
                else
                {
                    // create input if not here
                    sprintf(str, "pIn %d", a);
                    beh->CreateInputParameter(str, guid_out);
                }
            }
        }

        // remove input if needed
        int pInCount = beh->GetInputParameterCount();
        while (pInCount > pOutCount)
        {
            --pInCount;
            behcontext.Context->DestroyObject(beh->RemoveInputParameter(pInCount));
        }
    }
    break;
    }

    return CKBR_OK;
}