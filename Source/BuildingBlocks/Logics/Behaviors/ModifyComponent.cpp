/////////////////////////////////////////////////////
//		            Modify Vector
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ModifyComponent.h"

CKObjectDeclaration *FillBehaviorModifyComponentDecl();
CKERROR CreateModifyComponentProto(CKBehaviorPrototype **);
int ModifyComponent(const CKBehaviorContext &BehContext);
CKERROR ModifyComponentCB(const CKBehaviorContext &BehContext);

// to call after edition
// update parameters
// removes user created pins
CKBOOL ComponentChange(const CKBehaviorContext &BehContext, int componentIndex);

// to call after settings edition
// update parameters
CKBOOL ComponentUpdate(const CKBehaviorContext &BehContext, CKDWORD flags);

// get main inputparameter "component index", that is the index of the guid in g_Components
int GetComponentIndex(const CKBehaviorContext &BehContext);

void UpdateSettingsType(const CKBehaviorContext &BehContext, CKDWORD type);

//
// function declaration
//

CKObjectDeclaration *FillBehaviorModifyComponentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Modify Component");
    od->SetDescription("Modify a vector");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Vin: </SPAN>the original input (Vector by default). You can modify its type with those available (2dVector, Box, Color, Quaternion, Rectangle or Vector). The settings change according to the type of the original input, as well as the number of components and their types.
    Created.<BR>

    <SPAN CLASS=pin>Created: </SPAN>one of the original input's component (float for a Vector). Edit the settings to choose which component you want edited.<BR>
    <BR>
    <SPAN CLASS=pout>Vout: </SPAN>the composed 2dVector, Box, Color, Quaternion, Rectangle or Vector.<BR>
    <BR>
    <SPAN CLASS=setting>Rectangle/Box Mode: </SPAN>if the output parameter type is a RECTANGLE or a BOUNDING BOX, then this settings define how
    informations should be given (center/half-size ... top,left(min) corner/ bottom/right(max) corner ... or top,left(min) corner/size).<BR>
    <BR>
    Composes a 2dVector, Box, Color, Quaternion, Rectangle or Vector, by specifying none, one or several of its components.<BR>
    <BR>
    This building block composes either a 2dVector, Box, Color, Quaternion, Rectangle or Vector from none/some of their components.
    eg: Say 'Variable' is a Vector (2,2,2).
    In the settings, chek "Modify X"
    Set X value with (0)
    Then 'Vout'=[0,2,2].
    
    If you change the type of 'Vin', all settings are reseted and all automatically added parameters input are destroyed.
    
    See Also: Get Component, Set Component.
    <BR>
    See Also: Get Component, Set Component.<BR>
    */
    /* warning:
    - COLOR componants are to be get from 0.0 to 1.0 (so it'll much easier for you to process calculs).
    Therefore, [1,1,1]=white and [1,0,0]=red.<BR>
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateModifyComponentProto);
    od->SetCategory("Logics/Calculator");
    od->SetGuid(CKGUID(0xE5B234F3, 0x35FBACF7));
    od->SetAuthorGuid(CKGUID(0x56495254, 0x4f4f4c53));
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_BEOBJECT);

    return od;
}

CKERROR CreateModifyComponentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Modify Component");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    //---	Inputs declaration
    proto->DeclareInput("In");

    //---	Outputs declaration
    proto->DeclareOutput("Out");

    //----- Input Parameters Declaration
    proto->DeclareInParameter("Vin", CKPGUID_VECTOR);

    //---	Output Parameters Declaration
    proto->DeclareOutParameter("Vout", CKPGUID_VECTOR);

    //----	Settings Declaration
    proto->DeclareSetting("Settings", CKPGUID_MODIFYCOMPONENT_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(ModifyComponentCB, CKCB_BEHAVIORBASE | CKCB_BEHAVIOREDITIONS | CKCB_BEHAVIORCREATE, NULL);
    proto->SetFunction(ModifyComponent);

    *pproto = proto;
    return CK_OK;
}

int ModifyComponent(const CKBehaviorContext &BehContext)
{
    CKBehavior *beh = BehContext.Behavior;

    // retrieve ComponentData
    CKDWORD flags;
    beh->GetLocalParameterValue(SETTINGS_FLAGS, &flags);
    ModifyParameter function = NULL;
    CKDWORD componentType = flags & TYPE_ALL;
    int componentIndex = ComponentTypeToIndex(componentType);
    ComponentData *c = g_Components.At(componentIndex);
    function = c->m_modifyFunction;
    if (function)
        function(BehContext, flags & ~TYPE_ALL);

    beh->ActivateInput(IN_IN, FALSE);
    beh->ActivateOutput(OUT_OUT);
    return CKBR_OK;
}

CKERROR ModifyComponentCB(const CKBehaviorContext &BehContext)
{
    CKBehavior *beh = BehContext.Behavior;

    switch (BehContext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // get settings
        CKDWORD flags;
        beh->GetLocalParameterValue(SETTINGS_FLAGS, &flags);
        ComponentUpdate(BehContext, flags);
    }
    break;

    case CKM_BEHAVIORCREATE:
    {
        CKDWORD flags;
        beh->GetLocalParameterValue(SETTINGS_FLAGS, &flags);

        CKDWORD oldtype = flags & TYPE_ALL; // old type of data
        if (oldtype == 0)
        {
            oldtype = TYPE_VECTOR;
            UpdateSettingsType(BehContext, oldtype);
        }
    }
    break;
    case CKM_BEHAVIOREDITED:
        // settings update
        {
            // get settings
            CKDWORD flags;
            beh->GetLocalParameterValue(SETTINGS_FLAGS, &flags);

            CKDWORD oldtype = flags & TYPE_ALL; // old type of data
            if (oldtype == 0)
            {
                oldtype = TYPE_VECTOR;
                UpdateSettingsType(BehContext, oldtype);
            }

            int componentIndex = GetComponentIndex(BehContext);

            CKDWORD newtype = ComponentIndexToType(componentIndex);
            if (newtype == oldtype)
                ComponentUpdate(BehContext, flags);
            else
                ComponentChange(BehContext, componentIndex);
        }
        break;
    default:
        return CKBR_OK;
    }
    return CKBR_OK;
}

CKBOOL ComponentChange(const CKBehaviorContext &BehContext, int componentIndex)
{
    // remove all parameters except main p�n & pout
    //+set pout's type to pin's
    ClearParameters(BehContext);

    // reset settings
    CKParameterLocal *plocal = BehContext.Behavior->GetLocalParameter(SETTINGS_FLAGS);
    plocal->SetGUID(ComponentIndexToGuid(componentIndex));
    CKDWORD value = ComponentIndexToType(componentIndex);
    plocal->SetValue(&value);

    return TRUE;
}

CKBOOL ComponentUpdate(const CKBehaviorContext &BehContext, CKDWORD flags)
{
    int i;

    // retrieve context
    CKContext *context = BehContext.Context;
    // get parameter manager
    CKParameterManager *pm = BehContext.Context->GetParameterManager();

    // retrieve ComponentData
    CKBehavior *beh = BehContext.Behavior;
    CKDWORD componentType = flags & TYPE_ALL;
    int componentIndex = ComponentTypeToIndex(componentType);
    ComponentData *c = g_Components.At(componentIndex);

    // update pins
    int inputParameterCount = 1;  // main pin at least
    int outputParameterCount = 1; // main pout at least

    // update pouts
    for (i = 0; i < c->m_count; i++)
    {
        CKDWORD parameterFlags = c->m_flags[i];

        //**********output parameter**********
        if (parameterFlags & FLAG_POUT)
        {
            parameterFlags &= ~FLAG_POUT; // remove that from the local parameter flag

            int index = GetPoutIndex(beh, c->m_names[i].Str()); // index of parameter, -1 if does not exist
            if (flags & parameterFlags)                         // parameter should be here
            {
                if (index < 0) // dont exist, create it
                {
                    beh->InsertOutputParameter(outputParameterCount, c->m_names[i].Str(), c->m_types[i]);
                }
                else if (index != outputParameterCount) // already exists but misplaced
                {
                    // SHOULD NOT HAPPEN, Plus no func to insert an existing parameter in ck
                    context->OutputToConsole("Modify Component : Update Error !");
                }
                else // already exists, verify type
                {
                    CKParameterOut *pout = beh->GetOutputParameter(index);
                    int type = pout->GetType();
                    if (type != c->m_types[i])
                        pout->SetType(c->m_types[i]);
                }

                outputParameterCount++;
            }
            else // parameter shouldn't be here
            {
                if (index >= 0) // exists, delete it
                {
                    CKParameterOut *pout = beh->GetOutputParameter(index);
                    context->DestroyObject(pout);
                }
            }
        }
        //**********input parameter**********
        else
        {
            int index = GetPinIndex(beh, c->m_names[i].Str());
            if (flags & parameterFlags) // parameter should be here
            {
                if (index < 0) // dont exist, create it
                {
                    beh->InsertInputParameter(inputParameterCount, c->m_names[i].Str(), c->m_types[i]);
                }
                else if (index != inputParameterCount) // already exists but misplaced
                {
                    // SHOULD NOT HAPPEN, Plus no func to insert an existing parameter in ck
                    context->OutputToConsole("Modify Component : Update Error !");
                }
                else // already exists, verify type
                {
                    CKParameterIn *pin = beh->GetInputParameter(index);
                    int type = pin->GetType();
                    if (type != c->m_types[i])
                        pin->SetType(c->m_types[i]);
                }

                inputParameterCount++;
            }
            else // parameter shouldnt be here
            {
                if (index >= 0) // exists, delete it
                {
                    CKParameterIn *pin = beh->GetInputParameter(index);
                    context->DestroyObject(pin);
                }
            }
        }
    }

    // delete remaining pin & pout
    XArray<CK_ID> parametersToDestroy;
    int pinCount = beh->GetInputParameterCount();
    for (i = inputParameterCount; i < pinCount; i++)
    {
        CKParameterIn *pin = beh->GetInputParameter(i);
        if (pin)
            parametersToDestroy.PushBack(pin->GetID());
    }
    int poutCount = beh->GetOutputParameterCount();
    for (i = outputParameterCount; i < poutCount; i++)
    {
        CKParameterOut *pout = beh->GetOutputParameter(i);
        if (pout)
            parametersToDestroy.PushBack(pout->GetID());
    }

    if (parametersToDestroy.Size() > 0)
        context->DestroyObjects(parametersToDestroy.Begin(), parametersToDestroy.Size());

    return TRUE;
}

int GetComponentIndex(const CKBehaviorContext &BehContext)
{
    // accepted type of 1st Pin
    // use of xsarray to get quickly in term of code line, the index of the type of the main pin
    XArray<CKGUID> guids;
    guids.Reserve(COMPONENT_COUNT);
    guids.PushBack(CKPGUID_2DVECTOR);
    guids.PushBack(CKPGUID_BOX);
    guids.PushBack(CKPGUID_COLOR);
    guids.PushBack(CKPGUID_EULERANGLES);
    guids.PushBack(CKPGUID_QUATERNION);
    guids.PushBack(CKPGUID_RECT);
    guids.PushBack(CKPGUID_VECTOR);

    CKParameterIn *pin = BehContext.Behavior->GetInputParameter(PIN_VIN);
    if (!pin)
        return -1; // big error

    CKGUID guid = pin->GetGUID();
    int componentIndex = guids.GetPosition(guid);
    if (componentIndex < 0) // not available guid
    {
        pin->SetGUID(CKPGUID_VECTOR); // force vector
        componentIndex = INDEX_VECTOR;
        BehContext.Context->OutputToConsole("Modify Component : type set to parameter input not allowed");
    }

    return componentIndex;
}

void UpdateSettingsType(const CKBehaviorContext &BehContext, CKDWORD type)
{
    CKDWORD value = 0;
    BehContext.Behavior->GetLocalParameterValue(SETTINGS_FLAGS, &value);
    value &= ~TYPE_ALL;
    value |= type;
    BehContext.Behavior->SetLocalParameterValue(SETTINGS_FLAGS, &value);
}
