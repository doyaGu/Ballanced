/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetComponent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetComponentDecl();
CKERROR CreateGetComponentProto(CKBehaviorPrototype **);
int GetComponent(const CKBehaviorContext &behcontext);
CKERROR GetComponentCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorGetComponentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Component");
    od->SetDescription("Extracts the components of a COLOR, VECTOR or 2dVECTOR.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Variable: </SPAN>the COLOR, VECTOR, VECTOR 2D, RECTANGLE or any Parameter Structure you want the component to be extract from.<BR>
    <BR>
    <SPAN CLASS=pout>Component 1: </SPAN>the first component (float).<BR>
    <SPAN CLASS=pout>Component 2: </SPAN>the second component (float).<BR>
    <SPAN CLASS=pout>Component 3: </SPAN>the third component (float).<BR>
    <BR>
    <SPAN CLASS=setting>Rectangle/Box Mode: </SPAN>if the input parameter type is a RECTANGLE or a BOX, then this settings define how the
    informations will be returned (center/half-size ... top,left corner/ bottom/right corner ... or top,left corner/size).<BR>
    <BR>
    This building block retrieves all components of either a COLOR, VECTOR, EULER ANGLES, VECTOR 2D or RECTANGLE.<BR>
    eg: Say 'Variable' is a vector and is equal to [8,3,5].<BR>
    Then, 'Component 1'=8.<BR>
    and , 'Component 2'=3.<BR>
    and , 'Component 3'=5.<BR>
    <BR>
    If you change the type of 'Variable', then (if needed) the number of output parameters will be changed automatically.<BR>
    <BR>
    See Also: SetComponent.<BR>
    */
    /* warning:
    - COLOR componants are to be get from 0.0 to 1.0 (so it'll much easier for you to process calculs).
    Therefore, [1,1,1]=white and [1,0,0]=red.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2689057b, 0x1a1a446a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateGetComponentProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

#define CKPGUID_RECTBOXMODE CKDEFINEGUID(0x5a6a3bd9, 0x7e2797d)

CKERROR CreateGetComponentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Component");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Variable", CKPGUID_VECTOR);

    proto->DeclareOutParameter("Component 1", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Component 2", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Component 3", CKPGUID_FLOAT);

    proto->DeclareSetting("Rectangle/Box Mode", CKPGUID_RECTBOXMODE, "Corners");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFunction(GetComponent);
    proto->SetBehaviorCallbackFct(GetComponentCallBack);

    *pproto = proto;
    return CK_OK;
}

int GetComponent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int count_out = beh->GetOutputParameterCount();
    CKGUID type;
    CKParameterIn *pin;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    if (pin = beh->GetInputParameter(0))
    {
        type = pin->GetGUID();

        if (type == CKPGUID_COLOR)
        {
            VxColor col;
            float r, g, b, a;
            beh->GetInputParameterValue(0, &col);
            r = col.r;
            g = col.g;
            b = col.b;
            a = col.a;
            beh->SetOutputParameterValue(0, &r);
            beh->SetOutputParameterValue(1, &g);
            beh->SetOutputParameterValue(2, &b);
            beh->SetOutputParameterValue(3, &a);
        }

        if (type == CKPGUID_VECTOR || type == CKPGUID_EULERANGLES)
        {
            VxVector vector(0.0f);
            float x, y, z;
            beh->GetInputParameterValue(0, &vector);
            x = vector.x;
            y = vector.y;
            z = vector.z;
            beh->SetOutputParameterValue(0, &x);
            beh->SetOutputParameterValue(1, &y);
            beh->SetOutputParameterValue(2, &z);
        }

        if (type == CKPGUID_2DVECTOR)
        {
            Vx2DVector vector;
            float x, y;
            beh->GetInputParameterValue(0, &vector);
            x = vector.x;
            y = vector.y;
            beh->SetOutputParameterValue(0, &x);
            beh->SetOutputParameterValue(1, &y);
        }

        if (type == CKPGUID_BOX)
        {
            VxBbox box;
            beh->GetInputParameterValue(0, &box);
            beh->SetOutputParameterValue(0, &(box.Min));
            beh->SetOutputParameterValue(1, &(box.Max));
        }

        if (type == CKPGUID_RECT)
        {
            Vx2DVector v1, v2;
            VxRect rect;
            beh->GetInputParameterValue(0, &rect);
            int rmode = 1;
            beh->GetLocalParameterValue(0, &rmode);
            switch (rmode)
            {
            case 1: // corners
                v1 = rect.GetTopLeft();
                v2 = rect.GetBottomRight();
                break;
            case 2: // dimensions
                v1 = rect.GetTopLeft();
                v2 = rect.GetSize();
                break;
            case 3: // center
                v1 = rect.GetCenter();
                v2 = rect.GetHalfSize();
                break;
            }
            beh->SetOutputParameterValue(0, &v1);
            beh->SetOutputParameterValue(1, &v2);
        }

        if (type == CKPGUID_BOX)
        {
            VxVector v1, v2;
            VxBbox box;
            beh->GetInputParameterValue(0, &box);
            int rmode = 1;
            beh->GetLocalParameterValue(0, &rmode);
            switch (rmode)
            {
            case 1: // corners
                v1 = box.Min;
                v2 = box.Max;
                break;
            case 2: // dimensions
                v1 = box.Min;
                v2 = box.GetSize();
                break;
            case 3: // center
                v1 = box.GetCenter();
                v2 = box.GetHalfSize();
                break;
            }
            beh->SetOutputParameterValue(0, &v1);
            beh->SetOutputParameterValue(1, &v2);
        }

        CKParameterManager *pm = behcontext.ParameterManager;
        CKStructStruct *data = pm->GetStructDescByType(pin->GetType());
        if (data)
        {
            CK_ID *ids = (CK_ID *)pin->GetReadDataPtr();
            for (int i = 0; i < data->NbData; ++i)
            {
                CKParameterOut *pout = (CKParameterOut *)behcontext.Context->GetObject(ids[i]);
                CKParameterOut *p = beh->GetOutputParameter(i);
                if (p)
                    p->CopyValue(pout);
            }
        }
    }
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR GetComponentCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORLOAD:
    {
        // create output if its an old version (to avoid building block test version replacement problems with variable inputs)
        if (beh->GetVersion() <= 0x00010000)
        {
            beh->CreateLocalParameter("Rectangle/Box Mode", CKPGUID_RECTBOXMODE);
            int rect_mode = 1;
            beh->SetLocalParameterValue(0, &rect_mode);
        }
    }
    break;

    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        char out_str[20];

        int c = beh->GetOutputParameterCount();
        pin = beh->GetInputParameter(0);
        CKGUID type = pin->GetGUID();

        if ((type == CKPGUID_COLOR))
        {
            int max = (c > 4) ? c : 4;
            for (int i = 0; i < max; i++)
            {
                if (i >= 4)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(4);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_FLOAT);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_FLOAT);
                    }
                }
            }
            return CKBR_OK;
        }

        if ((type == CKPGUID_VECTOR))
        {
            int max = (c > 3) ? c : 3;
            for (int i = 0; i < max; i++)
            {
                if (i >= 3)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(3);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_FLOAT);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_FLOAT);
                    }
                }
            }
            return CKBR_OK;
        }

        if ((type == CKPGUID_EULERANGLES))
        {
            int max = (c > 3) ? c : 3;
            for (int i = 0; i < max; i++)
            {
                if (i >= 3)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(3);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_ANGLE);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_ANGLE);
                    }
                }
            }
            return CKBR_OK;
        }

        if ((type == CKPGUID_2DVECTOR))
        {
            int max = (c > 2) ? c : 2;
            for (int i = 0; i < max; i++)
            {
                if (i >= 2)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_FLOAT);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_FLOAT);
                    }
                }
            }
            return CKBR_OK;
        }

        if ((type == CKPGUID_BOX))
        {
            int max = (c > 2) ? c : 2;
            for (int i = 0; i < max; i++)
            {
                if (i >= 2)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_VECTOR);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_VECTOR);
                    }
                }
            }
            return CKBR_OK;
        }

        if ((type == CKPGUID_RECT))
        {
            int max = (c > 2) ? c : 2;
            for (int i = 0; i < max; i++)
            {
                if (i >= 2)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_2DVECTOR);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_2DVECTOR);
                    }
                }
            }
            return CKBR_OK;
        }

        if ((type == CKPGUID_BOX))
        {
            int max = (c > 2) ? c : 2;
            for (int i = 0; i < max; i++)
            {
                if (i >= 2)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(CKPGUID_VECTOR);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, CKPGUID_VECTOR);
                    }
                }
            }
            return CKBR_OK;
        }

        CKParameterManager *pm = behcontext.ParameterManager;
        CKStructStruct *data = pm->GetStructDescByType(pin->GetType());
        if (data)
        {
            int max = (c > data->NbData) ? c : data->NbData;
            for (int i = 0; i < max; i++)
            {
                if (i >= data->NbData)
                { // we have to delete the extra parameters
                    CKParameterOut *pi = beh->RemoveOutputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c)
                    { // Output already existe
                        CKParameterOut *pin = beh->GetOutputParameter(i);
                        pin->SetGUID(data->Guids[i]);
                    }
                    else
                    { // Output must be created
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateOutputParameter(out_str, data->Guids[i]);
                    }
                }
            }
            return CKBR_OK;
        }

        // No Valid Input, we destroy all outputs
        for (int i = 0; i < c; i++)
        {
            CKParameterOut *pi = beh->RemoveOutputParameter(0);
            CKDestroyObject(pi);
        }
    }
    break;
    }
    return CKBR_OK;
}