/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Random
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#undef min
#undef max

CKObjectDeclaration *FillBehaviorRandomDecl();
CKERROR CreateRandomProto(CKBehaviorPrototype **);
int Random(const CKBehaviorContext &behcontext);
CKERROR RandomCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRandomDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Random");
    od->SetDescription("Retrieves a Random Variable between 'Min' an 'Max'.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Min: </SPAN>minimum value.<BR>
    <SPAN CLASS=pin>Max: </SPAN>maximum value.<BR>
    <BR>
    <SPAN CLASS=pout>Rand: </SPAN>random value between Min and Max</SPAN>.<BR>
    You can change the type of the Output Parameter "Rand", to FLOAT, INT, VECTOR, BOOL (No range need for bool), 2DVECTOR or COLOR, the two Input Parameters will be changed automatically.
    */
    /* warning:
    - you can use as well the "Random" Parameter Operation to perform this calculation.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x0c622386, 0x1c3054f7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRandomProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRandomProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Random");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Min", CKPGUID_FLOAT);
    proto->DeclareInParameter("Max", CKPGUID_FLOAT);

    proto->DeclareOutParameter("Rand", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFunction(Random);
    proto->SetBehaviorCallbackFct(RandomCallBack);

    *pproto = proto;
    return CK_OK;
}

int Random(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IOs States
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKParameterIn *pin;
    CKParameterOut *pout = beh->GetOutputParameter(0);
    CKGUID guid = pout->GetGUID();

#define pincheck(index)                      \
    {                                        \
        pin = beh->GetInputParameter(index); \
        if (!pin)                            \
            return CKBR_OK;                  \
        if (pin->GetGUID() != guid)          \
            return CKBR_OK;                  \
    }

    CKParameterManager *pm = behcontext.ParameterManager;
    if (pm->IsDerivedFrom(guid, CKPGUID_FLOAT)) // case FLOAT
    {
        pincheck(0);
        float min;
        pin->GetValue(&min);

        pincheck(1);
        float max;
        pin->GetValue(&max);

        float res = min + rand() * (max - min) / RAND_MAX;
        pout->SetValue(&res);

        return CKBR_OK;
    }

    if (guid == CKPGUID_INT) // case INT
    {
        pincheck(0);
        int min;
        pin->GetValue(&min);

        pincheck(1);
        int max;
        pin->GetValue(&max);

        int res = min + rand() * (max - min) / RAND_MAX;
        pout->SetValue(&res);

        return CKBR_OK;
    }

    if (guid == CKPGUID_VECTOR) // case Vector
    {
        pincheck(0);
        VxVector min(0.0f);
        pin->GetValue(&min);

        pincheck(1);
        VxVector max(0.0f);
        pin->GetValue(&max);

        VxVector res;
        res.x = min.x + rand() * (max.x - min.x) / RAND_MAX;
        res.y = min.y + rand() * (max.y - min.y) / RAND_MAX;
        res.z = min.z + rand() * (max.z - min.z) / RAND_MAX;

        pout->SetValue(&res);

        return CKBR_OK;
    }

    if (guid == CKPGUID_2DVECTOR) // case Vector
    {
        pincheck(0);
        Vx2DVector min;
        pin->GetValue(&min);

        pincheck(1);
        Vx2DVector max;
        pin->GetValue(&max);

        Vx2DVector res;
        res.x = min.x + rand() * (max.x - min.x) / RAND_MAX;
        res.y = min.y + rand() * (max.y - min.y) / RAND_MAX;

        pout->SetValue(&res);

        return CKBR_OK;
    }

    if (guid == CKPGUID_RECT) // case Vector
    {
        pincheck(0);
        VxRect min;
        pin->GetValue(&min);

        pincheck(1);
        VxRect max;
        pin->GetValue(&max);

        VxRect res;
        res.left = min.left + rand() * (max.left - min.left) / RAND_MAX;
        res.top = min.top + rand() * (max.top - min.top) / RAND_MAX;
        res.right = min.right + rand() * (max.right - min.right) / RAND_MAX;
        res.bottom = min.bottom + rand() * (max.bottom - min.bottom) / RAND_MAX;
        res.Normalize();
        pout->SetValue(&res);

        return CKBR_OK;
    }

    if (guid == CKPGUID_BOOL) // case BOOL
    {
        CKBOOL res = rand() & 1;

        pout->SetValue(&res);

        return CKBR_OK;
    }

    if (guid == CKPGUID_COLOR) // case COLOR
    {
        pincheck(0);
        VxColor min;
        pin->GetValue(&min);

        pincheck(1);
        VxColor max;
        pin->GetValue(&max);

        VxColor res;
        res.r = min.r + rand() * (max.r - min.r) / RAND_MAX;
        res.g = min.g + rand() * (max.g - min.g) / RAND_MAX;
        res.b = min.b + rand() * (max.b - min.b) / RAND_MAX;
        res.a = min.a + rand() * (max.a - min.a) / RAND_MAX;
        pout->SetValue(&res);

        return CKBR_OK;
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR RandomCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        CKParameterOut *pout = beh->GetOutputParameter(0);
        CKGUID guid_pout = pout->GetGUID();

        pin = beh->GetInputParameter(0);
        if (pin->GetGUID() != guid_pout) // Automatic change for MIN
        {
            pin->SetGUID(guid_pout, TRUE);
        }
        pin = beh->GetInputParameter(1);
        if (pin->GetGUID() != guid_pout) // Automatic change for MAX
        {
            pin->SetGUID(guid_pout, TRUE);
        }
    }
    }

    return CKBR_OK;
}