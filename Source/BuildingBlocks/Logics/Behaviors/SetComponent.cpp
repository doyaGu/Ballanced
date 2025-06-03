/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetComponent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetComponentDecl();
CKERROR CreateSetComponentProto(CKBehaviorPrototype **);
int SetComponent(const CKBehaviorContext &behcontext);
CKERROR SetComponentCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorSetComponentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Component");
    od->SetDescription("Composes a COLOR, VECTOR or 2dVECTOR, by specifying each components.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Component 1: </SPAN>the first component (float).<BR>
    <SPAN CLASS=pin>Component 2: </SPAN>the second component (float).<BR>
    <SPAN CLASS=pin>Component 3: </SPAN>the third component (float).<BR>
    <BR>
    <SPAN CLASS=pout>Variable : </SPAN>the composed COLOR, VECTOR or VECTOR 2D.<BR>
    <BR>
    <SPAN CLASS=setting>Rectangle/Box Mode: </SPAN>if the output parameter type is a RECTANGLE or a BOUNDING BOX, then this settings define how
    informations should be given (center/half-size ... top,left(min) corner/ bottom/right(max) corner ... or top,left(min) corner/size).<BR>
    <BR>
    This building block composes either a COLOR, VECTOR, EULER ANGLES, VECTOR 2D, RECTANGLE or a BOUNDING BOX from their components (floats or vector 2d for rectangles).<BR>
    eg: Say 'Variable' is a vector and <BR>
    'Component 1'=8.<BR>
    'Component 2'=3.<BR>
    'Component 3'=5.<BR>
    Then 'Variable'=[8,3,5].<BR>
    <BR>
    If you change the type of 'Variable', then (if needed) the number of input parameters will be changed automatically.<BR>
    <BR>
    See Also: Get Component.<BR>
    */
    /* warning:
    - COLOR componants are to be get from 0.0 to 1.0 (so it'll much easier for you to process calculs).
    Therefore, [1,1,1]=white and [1,0,0]=red.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6e800755, 0x57b64acb));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSetComponentProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

#define CKPGUID_RECTBOXMODE CKDEFINEGUID(0x5a6a3bd9, 0x7e2797d)

CKERROR CreateSetComponentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Component");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Component 1", CKPGUID_FLOAT);
    proto->DeclareInParameter("Component 2", CKPGUID_FLOAT);
    proto->DeclareInParameter("Component 3", CKPGUID_FLOAT);

    proto->DeclareOutParameter("Variable", CKPGUID_VECTOR);

    proto->DeclareSetting("Rectangle/Box Mode", CKPGUID_RECTBOXMODE, "Corners");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(SetComponent);
    proto->SetBehaviorCallbackFct(SetComponentCallBack);

    *pproto = proto;
    return CK_OK;
}

int SetComponent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int count_out = beh->GetOutputParameterCount();
    CKGUID type;
    CKParameterOut *pout;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    if (pout = beh->GetOutputParameter(0))
    {
        type = pout->GetGUID();

        if (type == CKPGUID_COLOR)
        {
            VxColor col;
            float r, g, b, a;
            beh->GetInputParameterValue(0, &r);
            beh->GetInputParameterValue(1, &g);
            beh->GetInputParameterValue(2, &b);
            beh->GetInputParameterValue(3, &a);
            col.r = r;
            col.g = g;
            col.b = b;
            col.a = a;
            beh->SetOutputParameterValue(0, &col);
        }

        if (type == CKPGUID_VECTOR || type == CKPGUID_EULERANGLES)
        {
            VxVector vector;
            float x, y, z;
            beh->GetInputParameterValue(0, &x);
            beh->GetInputParameterValue(1, &y);
            beh->GetInputParameterValue(2, &z);
            vector.x = x;
            vector.y = y;
            vector.z = z;
            beh->SetOutputParameterValue(0, &vector);
        }

        if (type == CKPGUID_2DVECTOR)
        {
            Vx2DVector vector;
            float x, y;
            beh->GetInputParameterValue(0, &x);
            beh->GetInputParameterValue(1, &y);
            vector.x = x;
            vector.y = y;
            beh->SetOutputParameterValue(0, &vector);
        }

        if (type == CKPGUID_RECT)
        {
            Vx2DVector v1, v2;
            beh->GetInputParameterValue(0, &v1);
            beh->GetInputParameterValue(1, &v2);
            VxRect rect;
            int rmode = 1;
            beh->GetLocalParameterValue(0, &rmode);
            switch (rmode)
            {
            case 1: // corners
                rect.SetCorners(v1, v2);
                break;
            case 2: // dimensions
                rect.SetDimension(v1, v2);
                break;
            case 3: // center
                rect.SetCenter(v1, v2);
                break;
            }
            beh->SetOutputParameterValue(0, &rect);
        }

        if (type == CKPGUID_BOX)
        {
            VxVector v1(0.0f), v2(0.0f);
            beh->GetInputParameterValue(0, &v1);
            beh->GetInputParameterValue(1, &v2);
            VxBbox box;
            int rmode = 1;
            beh->GetLocalParameterValue(0, &rmode);
            switch (rmode)
            {
            case 1: // corners
                box.SetCorners(v1, v2);
                break;
            case 2: // dimensions
                box.SetDimension(v1, v2);
                break;
            case 3: // center
                box.SetCenter(v1, v2);
                break;
            }
            beh->SetOutputParameterValue(0, &box);
        }

        CKStructHelper sh(pout);
        for (int i = 0; i < sh.GetMemberCount(); ++i)
        {
            CKParameterIn *pin = beh->GetInputParameter(i);
            if (!pin)
                continue;

            CKParameter *p = pin->GetRealSource();
            if (!p)
                continue;

            sh[i]->CopyValue(p);
        }
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SetComponentCallBack(const CKBehaviorContext &behcontext)
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

        // patch for intermediary versions
        int localcount = beh->GetLocalParameterCount();
        for (int i = 1; i < localcount; ++i)
        {
            CKParameterLocal *pl = beh->GetLocalParameter(1);
            CKDestroyObject(pl, CK_DESTROY_NONOTIFY, NULL);
        }
    }
    break;

    case CKM_BEHAVIOREDITED:
    {
        CKParameterOut *pout;
        char out_str[20];

        int c = beh->GetInputParameterCount();
        pout = beh->GetOutputParameter(0);
        CKGUID type = pout->GetGUID();

        if ((type == CKPGUID_COLOR))
        {
            int max = (c > 4) ? c : 4;
            for (int i = 0; i < max; i++)
            {
                if (i >= 4) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(4);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(CKPGUID_FLOAT, TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, CKPGUID_FLOAT);
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
                if (i >= 3) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(3);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(CKPGUID_FLOAT, TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, CKPGUID_FLOAT);
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
                if (i >= 3) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(3);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(CKPGUID_ANGLE, TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, CKPGUID_ANGLE);
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
                if (i >= 2) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(CKPGUID_FLOAT, TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, CKPGUID_FLOAT);
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
                if (i >= 2) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(CKPGUID_2DVECTOR, TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, CKPGUID_2DVECTOR);
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
                if (i >= 2) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(CKPGUID_VECTOR, TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, CKPGUID_VECTOR);
                    }
                }
            }
            return CKBR_OK;
        }

        CKStructHelper sh(pout);
        if (sh.GetMemberCount())
        {
            int max = XMax(c, sh.GetMemberCount());
            for (int i = 0; i < max; i++)
            {
                if (i >= sh.GetMemberCount()) // we have to delete the extra parameters
                {
                    CKParameterIn *pi = beh->RemoveInputParameter(2);
                    CKDestroyObject(pi);
                }
                else
                {
                    if (i < c) // input already exist
                    {
                        CKParameterIn *pin = beh->GetInputParameter(i);
                        pin->SetGUID(sh.GetMemberGUID(i), TRUE);
                    }
                    else // input must be created
                    {
                        sprintf(out_str, "Component %d", i + 1);
                        beh->CreateInputParameter(out_str, sh.GetMemberGUID(i));
                    }
                }
            }
            return CKBR_OK;
        }
    }
    }
    return CKBR_OK;
}