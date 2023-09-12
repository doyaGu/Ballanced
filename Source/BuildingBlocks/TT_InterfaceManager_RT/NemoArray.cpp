#include "NemoArray.h"

#include <math.h>
#include <string.h>

#include "CKAll.h"

CNemoElement::CNemoElement()
{
    m_Value.ival = 0;
    m_Type = CKARRAYTYPE_INT;
}

CNemoElement::CNemoElement(void *value, CK_ARRAYTYPE type)
{
    switch (type)
    {
    case CKARRAYTYPE_FLOAT:
        m_Value.fval = *(float *)value;
        break;

    case CKARRAYTYPE_STRING:
        if (value)
        {
            m_Value.str = new char[strlen((char *)value) + 1];
            if (!m_Value.str)
                m_Value.str = NULL;
            else
                strcpy(m_Value.str, (char *)value);
        }
        else
        {
            m_Value.str = new char[2];
            if (!m_Value.str)
                m_Value.str = NULL;
            else
                m_Value.str[0] = '\0';
        }
        break;

    case CKARRAYTYPE_OBJECT:
    case CKARRAYTYPE_PARAMETER:
        m_Value.id = *(CK_ID *)value;
        break;

    case CKARRAYTYPE_INT:
    default:
        m_Value.ival = *(int *)value;
        break;
    }

    m_Type = type;
}

CNemoElement::CNemoElement(const CNemoElement &rhs)
{
    switch (rhs.m_Type)
    {
    case CKARRAYTYPE_FLOAT:
        m_Value.fval = rhs.m_Value.fval;
        break;

    case CKARRAYTYPE_STRING:
        if (rhs.m_Value.str)
        {
            m_Value.str = new char[strlen(rhs.m_Value.str) + 1];
            if (!m_Value.str)
                m_Value.str = NULL;
            else
                strcpy(m_Value.str, rhs.m_Value.str);
        }
        else
        {
            if (!m_Value.str)
                m_Value.str = NULL;
            else
                m_Value.str[0] = '\0';
        }
        break;

    case CKARRAYTYPE_OBJECT:
    case CKARRAYTYPE_PARAMETER:
        m_Value.id = rhs.m_Value.id;
        break;

    case CKARRAYTYPE_INT:
    default:
        m_Value.ival = rhs.m_Value.ival;
        break;
    }

    m_Type = rhs.m_Type;
}

CNemoElement &CNemoElement::operator=(const CNemoElement &rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    switch (rhs.m_Type)
    {
    case CKARRAYTYPE_FLOAT:
        m_Value.fval = rhs.m_Value.fval;
        break;

    case CKARRAYTYPE_STRING:
    {
        char *str = NULL;
        if (rhs.m_Value.str)
        {
            str = new char[strlen(rhs.m_Value.str) + 1];
            if (!str)
                break;
            strcpy(str, rhs.m_Value.str);

            Free();
            m_Value.str = str;
        }
        else
        {
            str = new char[2];
            if (!str)
                break;
            str[0] = '\0';

            Free();
            m_Value.str = str;
        }
    }
    break;

    case CKARRAYTYPE_OBJECT:
    case CKARRAYTYPE_PARAMETER:
        m_Value.id = rhs.m_Value.id;
        break;

    case CKARRAYTYPE_INT:
    default:
        m_Value.ival = rhs.m_Value.ival;
        break;
    }

    m_Type = rhs.m_Type;
	return *this;
}

CNemoElement::~CNemoElement()
{
    Free();
}

bool CNemoElement::operator==(const CNemoElement &rhs)
{
    if (m_Type != rhs.m_Type)
        return false;

    switch (rhs.m_Type)
    {
    case CKARRAYTYPE_FLOAT:
        return fabs(m_Value.fval - rhs.m_Value.fval) < 0.001;

    case CKARRAYTYPE_STRING:
        if (m_Value.str && rhs.m_Value.str)
            return !strcmp(m_Value.str, rhs.m_Value.str);
        else
            return (!m_Value.str && !rhs.m_Value.str);

    case CKARRAYTYPE_OBJECT:
    case CKARRAYTYPE_PARAMETER:
        return m_Value.id == rhs.m_Value.id;

    case CKARRAYTYPE_INT:
    default:
        return m_Value.ival == rhs.m_Value.ival;
    }
}

void CNemoElement::Free()
{
    if (m_Type == CKARRAYTYPE_STRING && m_Value.str)
        delete[] m_Value.str;
}

bool CNemoElement::GetValue(void *value, CK_ARRAYTYPE *type) const
{
    switch (m_Type)
    {
    case CKARRAYTYPE_INT:
        *(int *)value = m_Value.ival;
        *type = CKARRAYTYPE_INT;
        break;

    case CKARRAYTYPE_FLOAT:
        *(float *)value = m_Value.fval;
        *type = CKARRAYTYPE_FLOAT;
        break;

    case CKARRAYTYPE_STRING:
        *(char **)value = m_Value.str;
        *type = CKARRAYTYPE_STRING;
        break;

    case CKARRAYTYPE_OBJECT:
    case CKARRAYTYPE_PARAMETER:
        *(CK_ID *)value = m_Value.id;
        *type = m_Type;
        break;

    default:
        return false;
    }

    return false;
}

char *CNemoElement::GetString() const
{
    if (m_Type != CKARRAYTYPE_STRING || m_Value.str[0] == '\0')
        return NULL;
    return m_Value.str;
}

CK_ARRAYTYPE CNemoElement::GetType() const
{
    return m_Type;
}

bool CNemoElement::SetValue(void *value, CK_ARRAYTYPE type, bool force)
{
    if (!force && type != m_Type)
        return false;

    switch (type)
    {
    case CKARRAYTYPE_INT:
        m_Value.ival = *(int *)value;
        break;

    case CKARRAYTYPE_FLOAT:
        m_Value.fval = *(float *)value;
        break;

    case CKARRAYTYPE_STRING:
    {
        if (!value)
            return false;

        char *str = new char[strlen((char *)value) + 1];
        if (!str)
            return false;
        strcpy(str, (char *)value);

        Free();
        m_Value.str = str;
    }

    break;

    case CKARRAYTYPE_OBJECT:
    case CKARRAYTYPE_PARAMETER:
        m_Value.id = *(CK_ID *)value;
        break;

    default:
        return false;
    }

    m_Type = type;
    return true;
}

bool CNemoElement::SetString(const char *str)
{
    if (!str || m_Type != CKARRAYTYPE_STRING)
        return false;

    char *tstr = new char[strlen(str) + 1];
    if (!tstr)
        return false;
    strcpy(tstr, str);

    Free();
    m_Value.str = tstr;

    return true;
}

bool CNemoElement::operator!=(const CNemoElement &rhs)
{
    return !(*this == rhs);
}

CNemoArray::CNemoArray() : m_Flag(false), m_Elements()
{
    memset(m_CmoName, 0, sizeof(m_CmoName));
    memset(m_ArrayName, 0, sizeof(m_ArrayName));
}

CNemoArray::CNemoArray(const char *cmoName, CKDataArray *array) : m_Flag(true), m_Elements()
{
    char str[512];
    void *value;

    strcpy(m_CmoName, cmoName);
    strcpy(m_ArrayName, array->GetName());

    const int columns = array->GetColumnCount();
    if (columns > 0)
    {
        for (int c = 0; c < columns; ++c)
        {
            CK_ARRAYTYPE type = array->GetColumnType(c);
            if (type == CKARRAYTYPE_STRING)
            {
                array->GetElementStringValue(0, c, str);
                value = (void *)str;
            }
            else
            {
                value = array->GetElement(0, c);
            }
            m_Elements.PushBack(CNemoElement(value, type));
        }
    }
}

CNemoArray::CNemoArray(const CNemoArray &rhs)
{
    strcpy(m_CmoName, rhs.m_CmoName);
    strcpy(m_ArrayName, rhs.m_ArrayName);
    m_Flag = rhs.m_Flag;
    m_Elements = rhs.m_Elements;
}

CNemoArray &CNemoArray::operator=(const CNemoArray &rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    strcpy(m_CmoName, rhs.m_CmoName);
    strcpy(m_ArrayName, rhs.m_ArrayName);
    m_Flag = rhs.m_Flag;
    m_Elements = rhs.m_Elements;

	return *this;
}

CNemoArray::~CNemoArray()
{
}

bool CNemoArray::operator==(const CNemoArray &rhs)
{
    return !strcmp(m_CmoName, rhs.m_CmoName) &&
           !strcmp(m_ArrayName, rhs.m_ArrayName);
}

bool CNemoArray::operator!=(const CNemoArray &rhs)
{
    return !(*this == rhs);
}

void CNemoArray::Read(CKDataArray *array)
{
    const int size = Size();
    if (array->GetRowCount() > 0 && array->GetColumnCount() == size)
    {
        XArray<CNemoElement>::Iterator it = m_Elements.Begin();
        for (int c = 0; c < size; ++c, ++it)
        {
            CK_ARRAYTYPE type = array->GetColumnType(c);
            if (type != 0 && it->GetType() == type)
            {
                if (type == CKARRAYTYPE_STRING)
                {
                    array->SetElementStringValue(0, c, it->GetString());
                }
                else
                {
                    CKDWORD value = 0;
                    it->GetValue(&value, &type);
                    array->SetElementValue(0, c, &value, sizeof(CKDWORD));
                }
            }
        }
    }
}

void CNemoArray::Write(CKDataArray *array)
{
    const int size = Size();
    if (array->GetRowCount() > 0 && array->GetColumnCount() == size)
    {
        XArray<CNemoElement>::Iterator it = m_Elements.Begin();
        for (int c = 0; c < size; ++c, ++it)
        {
            CK_ARRAYTYPE type = array->GetColumnType(c);
            if (type != 0 && it->GetType() == type)
            {
                if (type == CKARRAYTYPE_STRING)
                {
                    char str[128];
                    array->GetElementStringValue(0, c, str);
                    it->SetString(str);
                }
                else
                {
                    it->SetValue(array->GetElement(0, c), type);
                }
            }
        }
    }
}

CNemoArrayList::CNemoArrayList() : m_Flag(false), m_Arrays() {}

CNemoArrayList::~CNemoArrayList() {}

void CNemoArrayList::Add(const char *cmoName, CKDataArray *array)
{
    m_Arrays.PushBack(CNemoArray(cmoName, array));
}

void CNemoArrayList::Remove(const char *cmoName, CKDataArray *array)
{
    m_Arrays.Remove(Find(cmoName, array));
}

CNemoArray *CNemoArrayList::Search(const char *cmoName, CKDataArray *array)
{
    if (m_Arrays.Size() == 0)
        return NULL;
    return Find(cmoName, array);
}

CNemoArray *CNemoArrayList::Find(const char *cmoName, CKDataArray *array)
{
    return m_Arrays.Find(CNemoArray(cmoName, array));
}