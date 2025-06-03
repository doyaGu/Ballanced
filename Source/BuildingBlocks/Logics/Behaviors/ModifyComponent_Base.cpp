//Global & Utils & Initialisation functions
//declared in "ModifyComponent.h"

#include "CKAll.h"
#include "ModifyComponent.h"

//--------------------------------------------------
//global
//--------------------------------------------------

void Set2DVectorComponent	(CKParameterManager * pm,ComponentData * c);
void SetBoxComponent		(CKParameterManager * pm,ComponentData * c);
void SetColorComponent		(CKParameterManager * pm,ComponentData * c);
void SetEulerAnglesComponent(CKParameterManager * pm,ComponentData * c);
void SetQuaternionComponent	(CKParameterManager * pm,ComponentData * c);
void SetRectComponent		(CKParameterManager * pm,ComponentData * c);
void SetVectorComponent		(CKParameterManager * pm,ComponentData * c);

//database
XClassArray<ComponentData> g_Components;

ComponentData::ComponentData()
{
    m_count=0;
    m_componentGuid.d1=0;
    m_componentGuid.d2=0;
    m_componentType=0;
    m_modifyFunction=NULL;
}

ComponentData::~ComponentData()
{
    m_flags.Clear();
    m_names.Clear();
    m_types.Clear();
}

void InitializeModifyComponent(CKContext* context)
{
    CKParameterManager* pm = context->GetParameterManager();

    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_2DVECTOR	,SETTINGS_2DVECTOR_NAME		,SETTINGS_2DVECTOR);
    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_BOX		,SETTINGS_BOX_NAME			,SETTINGS_BOX);
    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_COLOR		,SETTINGS_COLOR_NAME		,SETTINGS_COLOR);
    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_EULERANGLES,SETTINGS_EULERANGLES_NAME	,SETTINGS_EULERANGLES);
    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_QUATERNION	,SETTINGS_QUATERNION_NAME	,SETTINGS_QUATERNION);
    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_RECT		,SETTINGS_RECT_NAME			,SETTINGS_RECT);
    pm->RegisterNewFlags(CKPGUID_MODIFYCOMPONENT_VECTOR		,SETTINGS_VECTOR_NAME		,SETTINGS_VECTOR);

    CKParameterTypeDesc* param_type=NULL;
    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_2DVECTOR);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_BOX);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_COLOR);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_EULERANGLES);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_QUATERNION);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_RECT);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    param_type=pm->GetParameterTypeDescription(CKPGUID_MODIFYCOMPONENT_VECTOR);
    if (param_type) param_type->dwFlags|=CKPARAMETERTYPE_HIDDEN;

    g_Components.Resize		(COMPONENT_COUNT);

    Set2DVectorComponent	(pm,g_Components.At(INDEX_2DVECTOR));
    SetBoxComponent			(pm,g_Components.At(INDEX_BOX));
    SetColorComponent		(pm,g_Components.At(INDEX_COLOR));
    SetEulerAnglesComponent	(pm,g_Components.At(INDEX_EULERANGLES));
    SetQuaternionComponent	(pm,g_Components.At(INDEX_QUATERNION));
    SetRectComponent		(pm,g_Components.At(INDEX_RECT));
    SetVectorComponent		(pm,g_Components.At(INDEX_VECTOR));
}

void DestroyModifyComponent(CKContext* context)
{
    CKParameterManager* pm = context->GetParameterManager();
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_2DVECTOR);
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_BOX);
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_COLOR);
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_EULERANGLES);
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_QUATERNION);
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_RECT);
    pm->UnRegisterParameterType(CKPGUID_MODIFYCOMPONENT_VECTOR);

    g_Components.Clear();
}

void Set2DVectorComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=Modify2DVector;

    int ptype = pm->ParameterGuidToType(CKPGUID_FLOAT);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_2DVECTOR;
    c->m_componentType=TYPE_2DVECTOR;

    int index;
    c->m_count=2;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=V2_X;
    c->m_flags[index++]=V2_Y;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_V2_X;
    c->m_names[index++]=STR_V2_Y;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

void SetBoxComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=ModifyBox;

    int ptype = pm->ParameterGuidToType(CKPGUID_VECTOR);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_BOX;
    c->m_componentType=TYPE_BOX;

    int index;
    c->m_count=2;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=B_V1;
    c->m_flags[index++]=B_V2;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_B_V1;
    c->m_names[index++]=STR_B_V2;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

void SetColorComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=ModifyColor;
    int ptype = pm->ParameterGuidToType(CKPGUID_FLOAT);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_COLOR;
    c->m_componentType=TYPE_COLOR;

    int index;
    c->m_count=4;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=C_R;
    c->m_flags[index++]=C_G;
    c->m_flags[index++]=C_B;
    c->m_flags[index++]=C_A;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_C_R;
    c->m_names[index++]=STR_C_G;
    c->m_names[index++]=STR_C_B;
    c->m_names[index++]=STR_C_A;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

void SetEulerAnglesComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=ModifyEulerAngles;

    int ptype = pm->ParameterGuidToType(CKPGUID_ANGLE);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_EULERANGLES;
    c->m_componentType=TYPE_EULERANGLES;

    int index;
    c->m_count=3;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=E_A1;
    c->m_flags[index++]=E_A2;
    c->m_flags[index++]=E_A3;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_E_A1;
    c->m_names[index++]=STR_E_A2;
    c->m_names[index++]=STR_E_A3;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

void SetQuaternionComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=ModifyQuaternion;

    int ptype = pm->ParameterGuidToType(CKPGUID_FLOAT);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_QUATERNION;
    c->m_componentType=TYPE_QUATERNION;

    int index;
    c->m_count=4;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=Q_X;
    c->m_flags[index++]=Q_Y;
    c->m_flags[index++]=Q_Z;
    c->m_flags[index++]=Q_W;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_Q_X;
    c->m_names[index++]=STR_Q_Y;
    c->m_names[index++]=STR_Q_Z;
    c->m_names[index++]=STR_Q_W;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

void SetRectComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=ModifyRect;

    int ptype = pm->ParameterGuidToType(CKPGUID_2DVECTOR);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_RECT;
    c->m_componentType=TYPE_RECT;

    int index;
    c->m_count=2;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=R_V1;
    c->m_flags[index++]=R_V2;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_R_V1;
    c->m_names[index++]=STR_R_V2;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

void SetVectorComponent(CKParameterManager* pm,ComponentData * c)
{
    c->m_modifyFunction=ModifyVector;

    int ptype = pm->ParameterGuidToType(CKPGUID_FLOAT);

    c->m_componentGuid=CKPGUID_MODIFYCOMPONENT_VECTOR;
    c->m_componentType=TYPE_VECTOR;

    int index;
    c->m_count=3;

    index=0;
    c->m_flags.Resize(c->m_count);
    c->m_flags[index++]=V_X;
    c->m_flags[index++]=V_Y;
    c->m_flags[index++]=V_Z;

    index=0;
    c->m_names.Resize(c->m_count);
    c->m_names[index++]=STR_V_X;
    c->m_names[index++]=STR_V_Y;
    c->m_names[index++]=STR_V_Z;

    c->m_types.Resize(c->m_count);
    for (index=0;index<c->m_count;index++)
        c->m_types[index]=ptype;
}

//--------------------------------------------------
//code
//--------------------------------------------------
int GetPinIndex(CKBehavior * beh,char * name)
{
    if (!name)
        return -1;
    int count=beh->GetInputParameterCount();
    for (int i=0;i<count;i++)
    {
        CKParameterIn * pin = beh->GetInputParameter(i);
        CKSTRING str = pin->GetName();
        if (str)
        {
            if (strcmp(str,name)==0)
                return i;
        }
    }
    return -1;
}

int GetPoutIndex(CKBehavior * beh,char * name)
{
    if (!name)
        return -1;
    int count=beh->GetOutputParameterCount();
    for (int i=0;i<count;i++)
    {
        CKParameterOut * pout = beh->GetOutputParameter(i);
        CKSTRING str = pout->GetName();
        if (str)
        {
            if (strcmp(str,name)==0)
                return i;
        }
    }
    return -1;
}

void ClearParameters(const CKBehaviorContext& BehContext)
{
    CKBehavior * beh = BehContext.Behavior;

    //delete remaining pin & pout
    XArray<CK_ID> parametersToDestroy;
    int pinCount=beh->GetInputParameterCount();
    int i;
    for (i=1;i<pinCount;i++)
    {
        CKParameterIn * pin = beh->GetInputParameter(i);
        if (pin)
            parametersToDestroy.PushBack(pin->GetID());
    }
    int poutCount=beh->GetOutputParameterCount();
    for (i=1;i<poutCount;i++)
    {
        CKParameterOut * pout = beh->GetOutputParameter(i);
        if (pout)
            parametersToDestroy.PushBack(pout->GetID());
    }

    if (parametersToDestroy.Size()>0)
        BehContext.Context->DestroyObjects(parametersToDestroy.Begin(),parametersToDestroy.Size());

    CKParameterIn * pin=beh->GetInputParameter(PIN_VIN);
    CKParameterOut * pout=beh->GetOutputParameter(POUT_VOUT);
    pout->SetType(pin->GetType());
}

CKGUID ComponentIndexToGuid(int index)
{
    return g_Components[index].m_componentGuid;
}

int ComponentGuidToIndex(CKGUID guid)
{
    for (int i=0;i<COMPONENT_COUNT;i++)
    {
        if (g_Components[i].m_componentGuid==guid)
            return i;
    }
    return -1;
}

int ComponentIndexToType(int index)
{
    return g_Components[index].m_componentType;
}

int ComponentTypeToIndex(int type)
{
    if (type==0)
        return INDEX_VECTOR;
    for (int i=0;i<COMPONENT_COUNT;i++)
    {
        if (g_Components[i].m_componentType==type)
            return i;
    }
    return -1;
}
