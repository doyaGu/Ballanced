#ifndef TT_INTERFACEMANAGER_NEMOARRAY_H
#define TT_INTERFACEMANAGER_NEMOARRAY_H

#include "XClassArray.h"
#include "CKDataArray.h"

class CNemoElement
{
public:
    CNemoElement();
    CNemoElement(void *value, CK_ARRAYTYPE type);
    CNemoElement(const CNemoElement &rhs);
    CNemoElement &operator=(const CNemoElement &rhs);

    virtual ~CNemoElement();

    bool operator==(const CNemoElement &rhs);
    bool operator!=(const CNemoElement &rhs);

    void Free();

    bool GetValue(void *value, CK_ARRAYTYPE *type) const;
    char *GetString() const;
    CK_ARRAYTYPE GetType() const;

    bool SetValue(void *value, CK_ARRAYTYPE type, bool force = true);
    bool SetString(const char *str);

private:
    union
    {
        char *str;
        int ival;
        float fval;
        CK_ID id;
    } m_Value;
    CK_ARRAYTYPE m_Type;
};

class CNemoArray
{
public:
    CNemoArray();
    CNemoArray(const char *cmoName, CKDataArray *array);
    CNemoArray(const CNemoArray &rhs);
    CNemoArray &operator=(const CNemoArray &rhs);

    virtual ~CNemoArray();

    bool operator==(const CNemoArray &rhs);
    bool operator!=(const CNemoArray &rhs);

    void Read(CKDataArray *array);
    void Write(CKDataArray *array);

    int Size() const
    {
        return m_Elements.Size();
	}

private:
    char m_CmoName[125];
    char m_ArrayName[125];
    bool m_Flag;
    XClassArray<CNemoElement> m_Elements;
};

class CNemoArrayList
{
public:
    CNemoArrayList();

    virtual ~CNemoArrayList();

    void Add(const char *cmoName, CKDataArray *array);
    void Remove(const char *cmoName, CKDataArray *array);

    CNemoArray *Search(const char *cmoName, CKDataArray *array);
    CNemoArray *Find(const char *cmoName, CKDataArray *array);

    int Size() const
    {
        return m_Arrays.Size();
    }

    bool IsEmpty() const
    {
        return m_Arrays.Size() == 0;
    }

private:
    CNemoArrayList(const CNemoArrayList &rhs);
    CNemoArrayList &operator=(const CNemoArrayList &rhs);

    bool m_Flag;
    XClassArray<CNemoArray> m_Arrays;
};

#endif // TT_INTERFACEMANAGER_NEMOARRAY_H