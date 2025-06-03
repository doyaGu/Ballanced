
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Variation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorVariationDecl();
CKERROR CreateVariationProto(CKBehaviorPrototype **);
int Variation(const CKBehaviorContext &behcontext);
CKERROR VariationCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorVariationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Variation");
    od->SetDescription("Computes the difference between old and new value of X.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Reset: </SPAN>resets the memorized old value to 0 (note that no output are activated).<BR>
    <BR>
    <SPAN CLASS=out>Change: </SPAN>activated if old X value differs from new X value.<BR>
    <SPAN CLASS=out>No Change: </SPAN>activated if old X value=new X value.<BR>
    <BR>
    <SPAN CLASS=pin>X: </SPAN>the new value to be compared to the old one.<BR>
    <BR>
    <SPAN CLASS=pout>Delta: </SPAN>(new X value) - (old X value).<BR>
    <BR>
    Useful to compute the variation of a position (ie velocity), or to switch sequential streaming when a change of value occures.<BR>
    <BR>
    */
    /* warning:
    - You can change the type of 'X', so it can be an ANGLE, VECTOR, FLOAT, INTEGER, 2DVECTOR, COLOR, and still get a Delta value (rem: FLOAT and ANGLE are anyway compatible, so does PERCENTAGE).<BR>
    For all other types, the delta won't be calculated (But the variation will still be detected).<BR>
    - Just change the 'X' type, and the 'Delta' type will automatically be changed.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3e7335d9, 0x29687f36));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateVariationProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateVariationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Variation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Reset");

    proto->DeclareOutput("Change");
    proto->DeclareOutput("No Change");

    proto->DeclareInParameter("X", CKPGUID_FLOAT);
    proto->DeclareInParameter("Reset Value", CKPGUID_FLOAT);

    proto->DeclareOutParameter("Delta", CKPGUID_FLOAT);
    proto->DeclareOutParameter("OldX", CKPGUID_FLOAT);

    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT); // "OldX"

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Variation);

    proto->SetBehaviorCallbackFct(VariationCallBack);

    *pproto = proto;
    return CK_OK;
}

int Variation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (beh->IsInputActive(0)) // enter by IN
    {
        beh->ActivateInput(0, FALSE);
    }
    else // enter by RESET
    {
        beh->ActivateInput(1, FALSE);

        CKParameterLocal *l = beh->GetLocalParameter(0);
        void *writePtr = l->GetWriteDataPtr();

        CKParameterIn *in = beh->GetInputParameter(1);
        if (in)
        {
            void *readPtr = in->GetReadDataPtr();
            memcpy(writePtr, readPtr, l->GetDataSize());
        }
        else
        {
            memset(writePtr, 0, l->GetDataSize());
        }

        return CKBR_OK;
    }

    CKGUID type;
    CKParameterOut *pout;

    if (pout = beh->GetOutputParameter(0))
    {
        type = pout->GetGUID();

        if (type == CKPGUID_FLOAT || type == CKPGUID_PERCENTAGE || type == CKPGUID_ANGLE)
        {
            float X, OldX;
            beh->GetInputParameterValue(0, &X);
            beh->GetLocalParameterValue(0, &OldX);
            beh->SetLocalParameterValue(0, &X);
            X -= OldX;
            beh->SetOutputParameterValue(0, &X);
            if (X != 0.0f)
            {
                beh->SetOutputParameterValue(1, &OldX);
                beh->ActivateOutput(0);
            }
            else
                beh->ActivateOutput(1);
            return CKBR_OK;
        }

        if (type == CKPGUID_VECTOR)
        {
            VxVector X, OldX;
            beh->GetInputParameterValue(0, &X);
            beh->GetLocalParameterValue(0, &OldX);
            beh->SetLocalParameterValue(0, &X);
            X.x -= OldX.x;
            X.y -= OldX.y;
            X.z -= OldX.z;
            beh->SetOutputParameterValue(0, &X);
            if ((X.x == 0.0f) && (X.y == 0.0f) && (X.z == 0.0f))
            {
                beh->SetOutputParameterValue(1, &OldX);
                beh->ActivateOutput(1);
            }
            else
                beh->ActivateOutput(0);
            return CKBR_OK;
        }

        if (type == CKPGUID_INT)
        {
            int X, OldX;
            beh->GetInputParameterValue(0, &X);
            beh->GetLocalParameterValue(0, &OldX);
            beh->SetLocalParameterValue(0, &X);
            X -= OldX;
            beh->SetOutputParameterValue(0, &X);
            if (X != 0)
            {
                beh->SetOutputParameterValue(1, &OldX);
                beh->ActivateOutput(0);
            }
            else
                beh->ActivateOutput(1);
            return CKBR_OK;
        }

        if (type == CKPGUID_2DVECTOR)
        {
            Vx2DVector X, OldX;
            beh->GetInputParameterValue(0, &X);
            beh->GetLocalParameterValue(0, &OldX);
            beh->SetLocalParameterValue(0, &X);
            X.x -= OldX.x;
            X.y -= OldX.y;
            beh->SetOutputParameterValue(0, &X);
            if ((X.x == 0.0f) && (X.y == 0.0f))
            {
                beh->SetOutputParameterValue(1, &OldX);
                beh->ActivateOutput(1);
            }
            else
                beh->ActivateOutput(0);
            return CKBR_OK;
        }

        if (type == CKPGUID_COLOR)
        {
            VxColor X, OldX;
            beh->GetInputParameterValue(0, &X);
            beh->GetLocalParameterValue(0, &OldX);
            beh->SetLocalParameterValue(0, &X);
            X.r -= OldX.r;
            X.g -= OldX.g;
            X.b -= OldX.b;
            X.a -= OldX.a;
            beh->SetOutputParameterValue(0, &X);
            if ((X.r == 0.0f) && (X.g == 0.0f) && (X.b == 0.0f) && (X.a == 0.0f))
            {
                beh->SetOutputParameterValue(1, &OldX);
                beh->ActivateOutput(1);
            }
            else
                beh->ActivateOutput(0);
            return CKBR_OK;
        }

        CKParameterIn *pin = beh->GetInputParameter(0);
        CKParameter *pnew = pin->GetRealSource();
        CKParameterLocal *plocal = beh->GetLocalParameter(0);
        void *oldmem = plocal->GetReadDataPtr();
        int minsize = pnew->GetDataSize();
        if (minsize > plocal->GetDataSize())
            minsize = plocal->GetDataSize();
        if (memcmp(pnew->GetReadDataPtr(), oldmem, minsize))
        {
            CKParameterOut *pout = beh->GetOutputParameter(1);
            void *writePtr = pout->GetWriteDataPtr();
            memcpy(writePtr, oldmem, pout->GetDataSize());
            pout->DataChanged();
            beh->ActivateOutput(0);
        }
        else
            beh->ActivateOutput(1);
        plocal->CopyValue(pnew);
        return CKBR_OK;
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR VariationCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        int type_in;
        if (pin = beh->GetInputParameter(0))
            type_in = pin->GetType();

        // we check the type of the 'Output Parameter'
        CKParameterOut *pout;
        if (pout = beh->GetOutputParameter(0)) // output param
            if (pout->GetType() != type_in)
            {
                pout->SetType(type_in);

                CKParameterLocal *p;
                if (p = beh->GetLocalParameter(0)) // local param
                {
                    p->SetType(type_in);
                }

                if (pin = beh->GetInputParameter(1)) // second input param
                {
                    pin->SetType(type_in);
                }

                if (pout = beh->GetOutputParameter(1)) // second output param
                {
                    pout->SetType(type_in);
                }
            }
    }
    }

    return CKBR_OK;
}