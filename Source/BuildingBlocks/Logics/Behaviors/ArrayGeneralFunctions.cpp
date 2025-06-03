/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array General Functions
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKERROR ArrayLineOutputCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIOREDITED:
    {
        CKDataArray *array = (CKDataArray *)beh->GetTarget();
        if (!array)
            return CKBR_OK;

        CreateOutputParameters(array, beh, 1);
    }
    break;
    }

    return CKBR_OK;
}

void WriteOutputParameters(CKDataArray *array, CKDataRow *dr, CKBehavior *beh, int firstoutputindex)
{
    if (!dr)
        return;
    CKDWORD *it = dr->Begin();
    if (!it)
        return;

    // we write the data in the output parameters
    CKParameterOut *pout;
    for (int i = 0; i < array->GetColumnCount(); ++i, ++it)
    {
        pout = beh->GetOutputParameter(firstoutputindex + i);
        if (!pout)
            continue;

        switch (array->GetColumnType(i))
        {
        case CKARRAYTYPE_INT:
        {
            if (pout->GetGUID() == CKPGUID_INT)
            {
                pout->SetValue((int *)it);
            }
        }
        break;
        case CKARRAYTYPE_FLOAT:
        {
            if (pout->GetGUID() == CKPGUID_FLOAT)
            {
                pout->SetValue((float *)it);
            }
        }
        break;
        case CKARRAYTYPE_STRING:
        {
            if (pout->GetGUID() == CKPGUID_STRING)
            {
                char *ns = (char *)(*it);
                if (ns)
                    pout->SetValue(ns, strlen(ns) + 1);
                else
                {
                    char *s = "";
                    pout->SetValue(s, 1);
                }
            }
        }
        break;
        case CKARRAYTYPE_OBJECT:
        {

            CK_CLASSID cid;
            if (cid = pout->GetParameterClassID())
            {
                CKObject *obj = beh->GetCKObject(*it);
                if (CKIsChildClassOf(obj, cid))
                    pout->SetValue((CKDWORD *)it);
                else
                {
                    int dummy = 0;
                    pout->SetValue(&dummy);
                }
            }
        }
        break;
        case CKARRAYTYPE_PARAMETER:
        {
            CKParameterOut *arraypout = (CKParameterOut *)(*it);
            pout->CopyValue(arraypout);
        }
        break;
        }
    }
}

void ReadInputParameters(CKDataArray *array, CKDataRow *dr, CKBehavior *beh, int firstinputindex)
{
    if (!dr)
        return;
    CKDWORD *it = dr->Begin();
    if (!it)
        return;
    int i;
    CKParameterIn *pin;
    for (i = 0; i < array->GetColumnCount(); ++i, ++it)
    {
        pin = beh->GetInputParameter(i + firstinputindex);
        if (!pin)
            continue;
        if (!(pin->GetRealSource()))
            continue;

        switch (array->GetColumnType(i))
        {
        case CKARRAYTYPE_INT:
        {
            if (pin->GetGUID() == CKPGUID_INT)
            {
                int i = 0;
                pin->GetValue(&i);
                (*it) = i;
            }
        }
        break;
        case CKARRAYTYPE_FLOAT:
        {
            if (pin->GetGUID() == CKPGUID_FLOAT)
            {
                float f = 0.0f;
                pin->GetValue(&f);
                (*it) = *(CKDWORD *)&f;
            }
        }
        break;
        case CKARRAYTYPE_STRING:
        {
            if (pin->GetGUID() == CKPGUID_STRING)
            {
                char *s = (char *)pin->GetReadDataPtr();
                char *as = (char *)(*it);
                VxDelete(as);
                if (s)
                {
                    char *ns = CKStrdup(s);
                    (*it) = (CKDWORD)ns;
                }
                else
                    *it = 0;
            }
        }
        break;
        case CKARRAYTYPE_OBJECT:
        {
            if (pin->GetGUID() == CKPGUID_BEOBJECT)
            {
                CK_ID id = 0;
                pin->GetValue(&id);
                (*it) = id;
            }
        }
        break;
        case CKARRAYTYPE_PARAMETER:
        {
            if (pin->GetGUID() == array->GetColumnParameterGuid(i))
            {
                // we get the array parameter
                CKParameterOut *pout = (CKParameterOut *)(*it);
                // we copy the value of the input parameter in the array
                pout->CopyValue(pin->GetRealSource());
            }
        }
        break;
        }
    }
}

void CreateInputParameter(CKDataArray *array, CKBehavior *beh, int c, int pos, char *pname)
{
    if (c >= array->GetColumnCount())
        return;

    CKGUID AttrType = CKPGUID_NONE;
    CKParameterIn *pin;

    switch (array->GetColumnType(c))
    {
    case CKARRAYTYPE_INT:
        AttrType = CKPGUID_INT;
        break;
    case CKARRAYTYPE_FLOAT:
        AttrType = CKPGUID_FLOAT;
        break;
    case CKARRAYTYPE_STRING:
        AttrType = CKPGUID_STRING;
        break;
    case CKARRAYTYPE_OBJECT:
        AttrType = CKPGUID_BEOBJECT;
        break;
    case CKARRAYTYPE_PARAMETER:
        AttrType = array->GetColumnParameterGuid(c);
        break;
    }
    pin = beh->GetInputParameter(pos);
    if (pin)
    { // we need to change the type
        if (pin->GetGUID() != AttrType)
        {
            pin->SetGUID(AttrType);
        }
        if (pname)
            pin->SetName(pname);
        else
            pin->SetName(array->GetColumnName(c));
    }
    else
    { // we need to create the parameter
        if (pname)
            beh->CreateInputParameter(pname, AttrType);
        else
            beh->CreateInputParameter(array->GetColumnName(c), AttrType);
    }
}

void CreateOutputParameter(CKDataArray *array, CKBehavior *beh, int c, int pos, char *pname)
{
    if (c >= array->GetColumnCount())
        return;

    CKGUID AttrType = CKPGUID_NONE;
    CKParameterOut *pout;

    switch (array->GetColumnType(c))
    {
    case CKARRAYTYPE_INT:
        AttrType = CKPGUID_INT;
        break;
    case CKARRAYTYPE_FLOAT:
        AttrType = CKPGUID_FLOAT;
        break;
    case CKARRAYTYPE_STRING:
        AttrType = CKPGUID_STRING;
        break;
    case CKARRAYTYPE_OBJECT:
        AttrType = CKPGUID_BEOBJECT;
        break;
    case CKARRAYTYPE_PARAMETER:
        AttrType = array->GetColumnParameterGuid(c);
        break;
    }
    pout = beh->GetOutputParameter(pos);
    if (pout)
    { // we need to change the type
        if (pout->GetGUID() != AttrType)
        {
            pout->SetGUID(AttrType);
        }
        if (pname)
            pout->SetName(pname);
        else
            pout->SetName(array->GetColumnName(c));
    }
    else
    { // we need to create the parameter
        if (pname)
            beh->CreateOutputParameter(pname, AttrType);
        else
            beh->CreateOutputParameter(array->GetColumnName(c), AttrType);
    }
}

CKBOOL
IsCompatible(CKDataArray *array, int c, CKSTRING paramname, CKGUID paramguid, CKGUID &columnguid)
{

    // Not the same name
    CKSTRING columnname = array->GetColumnName(c);

    // Name Checking
    if (!paramname)
    {
        if (columnname && (*columnname != '\0'))
            return FALSE;
    }
    else
    {
        if (!columnname && (*paramname != '\0'))
            return FALSE;
        else if (strcmp(columnname, paramname))
            return FALSE;
    }

    switch (array->GetColumnType(c))
    {
    case CKARRAYTYPE_INT:
        columnguid = CKPGUID_INT;
        break;
    case CKARRAYTYPE_FLOAT:
        columnguid = CKPGUID_FLOAT;
        break;
    case CKARRAYTYPE_STRING:
        columnguid = CKPGUID_STRING;
        break;
    case CKARRAYTYPE_OBJECT:
        columnguid = CKPGUID_BEOBJECT;
        break;
    case CKARRAYTYPE_PARAMETER:
        columnguid = array->GetColumnParameterGuid(c);
        break;
    default:
        columnguid = CKPGUID_NONE;
    }

    if (columnguid != paramguid)
        return FALSE;

    return TRUE;
}

void CreateInputParameters(CKDataArray *array, CKBehavior *beh, int firstinputindex)
{
    int i = 0;
    CKGUID desiredguid;

    CKParameterManager *pman = beh->GetCKContext()->GetParameterManager();

    // WARNING: the swap of columns isn t working wiht this code

    // First we clean the outputs of the behavior
    int count = beh->GetInputParameterCount();
    int arraycount = array->GetColumnCount();

    // First we want to check it it could be a renaming
    if ((count - firstinputindex) == arraycount)
    {
        int candidate = -1;

        for (int i = 0; i < arraycount; ++i)
        {
            CKParameterIn *pin = beh->GetInputParameter(firstinputindex + i);

            if (pin && !IsCompatible(array, i, pin->GetName(), pin->GetGUID(), desiredguid))
            { // we found one candidate
                if (candidate == -1)
                    candidate = i;
                else
                    candidate = -2;
            }
        }

        if (candidate >= 0)
        { // seems to be a simple renaming or type change
            CKParameterIn *pin = beh->GetInputParameter(firstinputindex + candidate);

            // Name updating
            CKSTRING columnname = array->GetColumnName(candidate);
            pin->SetName(columnname);

            // type updating
            CKGUID columnguid;
            switch (array->GetColumnType(candidate))
            {
            case CKARRAYTYPE_INT:
                columnguid = CKPGUID_INT;
                break;
            case CKARRAYTYPE_FLOAT:
                columnguid = CKPGUID_FLOAT;
                break;
            case CKARRAYTYPE_STRING:
                columnguid = CKPGUID_STRING;
                break;
            case CKARRAYTYPE_OBJECT:
                columnguid = CKPGUID_BEOBJECT;
                break;
            case CKARRAYTYPE_PARAMETER:
                columnguid = array->GetColumnParameterGuid(candidate);
                break;
            default:
                columnguid = CKPGUID_NONE;
            }
            pin->SetGUID(columnguid);
        }
        else
        {
            if (candidate == -1)
                return; // Nothing has changed, we exit from here
        }
    }

    for (i = firstinputindex; i < count;)
    {
        CKParameterIn *pin = beh->GetInputParameter(i);

        if (pin)
        {
            int j;
            for (j = 0; j < arraycount; ++j)
            {
                if (IsCompatible(array, j, pin->GetName(), pin->GetGUID(), desiredguid))
                {
                    break;
                }
            }
            if (j != arraycount)
                ++i;
            else
                CKDestroyObject(beh->RemoveInputParameter(i));
        }
        else
        {
            ++i;
        }
    }

    // then we add the missing ones
    int behindex = firstinputindex;
    for (i = 0; i < arraycount; ++i, ++behindex)
    {
        CKParameterIn *pin = beh->GetInputParameter(behindex);

        if (pin)
        {
            if (!IsCompatible(array, i, pin->GetName(), pin->GetGUID(), desiredguid))
            {
                int j;
                for (j = behindex + 1; j < beh->GetInputParameterCount(); ++j)
                {
                    CKParameterIn *pin = beh->GetInputParameter(j);
                    if (IsCompatible(array, i, pin->GetName(), pin->GetGUID(), desiredguid))
                    {
                        break;
                    }
                }
                if (j != beh->GetInputParameterCount())
                { // The parameter is present elsewhere, we swap it
                    CKParameterIn *pin = beh->RemoveInputParameter(j);
                    pin = beh->ReplaceInputParameter(behindex, pin);
                    beh->AddInputParameter(pin); // we reinsert the other at the end
                }
                else
                { // we have to insert a new input param
                    IsCompatible(array, i, array->GetColumnName(i), desiredguid, desiredguid);
                    beh->InsertInputParameter(behindex, array->GetColumnName(i), pman->ParameterGuidToType(desiredguid));
                }
            }
            else
                ; // the parameter is compatible and well placed
        }
        else
        { // No more input parameter, we create it
            IsCompatible(array, i, array->GetColumnName(i), desiredguid, desiredguid);
            beh->CreateInputParameter(array->GetColumnName(i), desiredguid);
        }
    }

    count = beh->GetInputParameterCount();
    for (i = behindex; i < count; ++i)
    {
        CKDestroyObject(beh->RemoveInputParameter(behindex));
    }

    /*
    // we remove all the extra input parameter
    int i;
    int count = beh->GetInputParameterCount();
    int firstindex = array->GetColumnCount()+firstoutputindex;
    for(i=firstindex; i<count; ++i){
        CKDestroyObject( beh->RemoveInputParameter(firstindex) );
    }

    CKGUID AttrType;
    CKParameterIn* pin;
    for(i=0;i<array->GetColumnCount();++i) {
        pin = beh->GetInputParameter(i+firstoutputindex);
        switch(array->GetColumnType(i)) {
        case CKARRAYTYPE_INT:AttrType = CKPGUID_INT;break;
        case CKARRAYTYPE_FLOAT:AttrType = CKPGUID_FLOAT;break;
        case CKARRAYTYPE_STRING:AttrType = CKPGUID_STRING;break;
        case CKARRAYTYPE_OBJECT:AttrType = CKPGUID_BEOBJECT;break;
        case CKARRAYTYPE_PARAMETER:AttrType = array->GetColumnParameterGuid(i);break;
        default:	AttrType = CKPGUID_NONE;
        }
        if(pin) { // we need to change the type
            if (pin->GetGUID()!=AttrType) {
                pin->SetGUID( AttrType );
            }
            pin->SetName(array->GetColumnName(i));
        } else { // we need to create the parameter
            beh->CreateInputParameter(array->GetColumnName(i),AttrType);
        }
    }
    */
}

void CreateOutputParameters(CKDataArray *array, CKBehavior *beh, int firstoutputindex)
{
    int i = 0;
    CKGUID desiredguid;

    CKParameterManager *pman = beh->GetCKContext()->GetParameterManager();

    int count = beh->GetOutputParameterCount();
    int arraycount = array->GetColumnCount();

    // First we want to check it it could be a renaming
    if ((count - firstoutputindex) == arraycount)
    {
        int candidate = -1;

        for (int i = 0; i < arraycount; ++i)
        {
            CKParameterOut *pout = beh->GetOutputParameter(firstoutputindex + i);

            if (pout && !IsCompatible(array, i, pout->GetName(), pout->GetGUID(), desiredguid))
            { // we found one candidate
                if (candidate == -1)
                    candidate = i;
                else
                    candidate = -2;
            }
        }

        if (candidate >= 0)
        { // seems to be a simple renaming or type change
            CKParameterOut *pout = beh->GetOutputParameter(firstoutputindex + candidate);

            // Name updating
            CKSTRING columnname = array->GetColumnName(candidate);
            pout->SetName(columnname);

            // type updating
            CKGUID columnguid;
            switch (array->GetColumnType(candidate))
            {
            case CKARRAYTYPE_INT:
                columnguid = CKPGUID_INT;
                break;
            case CKARRAYTYPE_FLOAT:
                columnguid = CKPGUID_FLOAT;
                break;
            case CKARRAYTYPE_STRING:
                columnguid = CKPGUID_STRING;
                break;
            case CKARRAYTYPE_OBJECT:
                columnguid = CKPGUID_BEOBJECT;
                break;
            case CKARRAYTYPE_PARAMETER:
                columnguid = array->GetColumnParameterGuid(candidate);
                break;
            default:
                columnguid = CKPGUID_NONE;
            }
            pout->SetGUID(columnguid);
        }
        else
        {
            if (candidate == -1)
                return; // Nothing has changed, we exit from here
        }
    }

    // WARNING: the swap of columns isn t working wiht this code

    // First we clean the outputs of the behavior
    for (i = firstoutputindex; i < count;)
    {
        CKParameterOut *pout = beh->GetOutputParameter(i);

        if (pout)
        {
            int j;
            for (j = 0; j < arraycount; ++j)
            {
                if (IsCompatible(array, j, pout->GetName(), pout->GetGUID(), desiredguid))
                {
                    break;
                }
            }
            if (j != arraycount)
                ++i;
            else
                CKDestroyObject(beh->RemoveOutputParameter(i));
        }
        else
        {
            ++i;
        }
    }

    // then we add the missing ones
    int behindex = firstoutputindex;
    for (i = 0; i < arraycount; ++i, ++behindex)
    {
        CKParameterOut *pout = beh->GetOutputParameter(behindex);

        if (pout)
        {
            if (!IsCompatible(array, i, pout->GetName(), pout->GetGUID(), desiredguid))
            {
                int j;
                for (j = behindex + 1; j < beh->GetOutputParameterCount(); ++j)
                {
                    CKParameterOut *pout = beh->GetOutputParameter(j);
                    if (IsCompatible(array, i, pout->GetName(), pout->GetGUID(), desiredguid))
                    {
                        break;
                    }
                }
                if (j != beh->GetOutputParameterCount())
                { // The parameter is present elsewhere, we swap it
                    CKParameterOut *pout = beh->RemoveOutputParameter(j);
                    pout = beh->ReplaceOutputParameter(behindex, pout);
                    beh->AddOutputParameter(pout); // we reinsert the other at the end
                }
                else
                { // we have to insert a new output param
                    IsCompatible(array, i, array->GetColumnName(i), desiredguid, desiredguid);
                    beh->InsertOutputParameter(behindex, array->GetColumnName(i), pman->ParameterGuidToType(desiredguid));
                }
            }
            else
                ; // the parameter is compatible and well placed
        }
        else
        { // No more output parameter, we create it
            IsCompatible(array, i, array->GetColumnName(i), desiredguid, desiredguid);
            beh->CreateOutputParameter(array->GetColumnName(i), desiredguid);
        }
    }

    count = beh->GetOutputParameterCount();
    for (i = behindex; i < count; ++i)
    {
        CKDestroyObject(beh->RemoveOutputParameter(behindex));
    }
}

int GetInputValue(CKDataArray *array, CKBehavior *beh, int column, int pindex, CKDWORD &key)
{
    CKParameterIn *pin = beh->GetInputParameter(pindex);
    if (!pin)
        return 0;

    key = 0;

    switch (array->GetColumnType(column))
    {
    case CKARRAYTYPE_INT:
    {
        if (pin->GetGUID() == CKPGUID_INT)
        {
            pin->GetValue(&key);
        }
    }
    break;
    case CKARRAYTYPE_FLOAT:
    {
        if (pin->GetGUID() == CKPGUID_FLOAT)
        {
            pin->GetValue(&key);
        }
    }
    break;
    case CKARRAYTYPE_STRING:
    {
        if (pin->GetGUID() == CKPGUID_STRING)
        {
            char *s = (char *)pin->GetReadDataPtr();
            if (s)
            {
                key = (CKDWORD)s;
                return strlen(s) + 1;
            }
        }
    }
    break;
    case CKARRAYTYPE_OBJECT:
    {
        if (pin->GetGUID() == CKPGUID_BEOBJECT)
        {
            pin->GetValue(&key);
        }
    }
    break;
    case CKARRAYTYPE_PARAMETER:
    {
        if (pin->GetGUID() == array->GetColumnParameterGuid(column))
        {
            key = (CKDWORD)pin->GetReadDataPtr();
            return (pin->GetRealSource())->GetDataSize();
        }
    }
    break;
    }

    return -1;
}