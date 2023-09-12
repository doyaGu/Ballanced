#include "DatabaseManager.h"

#include <io.h>
#include <string.h>
#include <sys/stat.h>

#include "CKAll.h"

static inline unsigned char rotl8(unsigned char val, int shift)
{
    shift &= 7;
    return (val << shift) | (val >> (8 - shift));
}

static inline unsigned char rotr8(unsigned char val, int shift)
{
    shift &= 7;
    return (val >> shift) | (val << (8 - shift));
}

struct ArrayHeader
{
    int columnCount;
    int rowCount;
    int keyColumn;
};

DatabaseManager::DatabaseManager(CKContext *context)
    : CKBaseManager(context, TT_DATABASE_MANAGER_GUID, "TT Database Manager"),
      m_Context(context),
      field_2C(false),
      m_ArrayNames(),
      m_Filename(NULL),
      m_Crypted(TRUE)
{
    context->RegisterNewManager(this);
}

DatabaseManager::~DatabaseManager()
{
    Clear();
}

int DatabaseManager::Register(CKSTRING arrayName)
{
    if (!arrayName)
        return 0;

    XArray<CKSTRING>::Iterator it = m_ArrayNames.Begin();
    while (it != m_ArrayNames.End())
        if (!strcmp(*it++, arrayName))
            return 21;

    int nameSize = strlen(arrayName) + 1;
    CKSTRING str = new char[strlen(arrayName) + 1];
    strncpy(str, arrayName, nameSize);
    m_ArrayNames.PushBack(str);
    return 1;
}

int DatabaseManager::Clear()
{
    XArray<CKSTRING>::Iterator it = m_ArrayNames.Begin();
    while (it != m_ArrayNames.End())
        if (*it)
            delete[] * it++;

    m_ArrayNames.Clear();
    return true;
}

int DatabaseManager::Load(CKContext *context, bool autoRegister, CKSTRING arrayName)
{
    int i, c;

    if (!arrayName)
        return 33;

    FILE *fp = fopen(m_Filename, "rb");
    if (!fp)
        return 31;

    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp);
    rewind(fp);

    char *fileData = new char[fileSize];

    fread(fileData, fileSize, sizeof(char), fp);
    fclose(fp);

    if (fileSize <= 0)
    {
        delete[] fileData;
        return 33;
    }

    if (m_Crypted)
    {
        for (i = 0; i < fileSize; ++i)
            fileData[i] = -(rotl8(fileData[i], 3) ^ 0xAF);
    }

    int nameSize = 0;
    int arraySize = 0;
    int chunkSize = 0;
    int pos = 0;
    char *chunk;
    for (chunk = fileData; strcmp(chunk, arrayName) != 0; chunk += chunkSize)
    {
        if (pos >= fileSize)
        {
            delete[] fileData;
            return 33;
        }

        nameSize = strlen(chunk) + 1;
        arraySize = *(int *)&chunk[nameSize];
        chunkSize = nameSize + sizeof(int) + arraySize;
        pos += chunkSize;
    }

    CKDataArray *array = (CKDataArray *)context->GetObjectByNameAndClass(chunk, CKCID_DATAARRAY);
    if (!array)
    {
        CK_CREATIONMODE res = CKLOAD_REPLACE;
        array = (CKDataArray *)context->CreateObject(CKCID_DATAARRAY, chunk, CK_OBJECTCREATION_REPLACE, &res);
        context->GetCurrentScene()->AddObjectToScene(array, FALSE);
    }

    array->Clear();
    for (c = array->GetColumnCount(); c >= 0; --c)
        array->RemoveColumn(c);

    if (autoRegister)
        Register(arrayName);

    nameSize = strlen(chunk) + 1;
    arraySize = *(int *)&chunk[nameSize];
    char *arrayData = new char[arraySize];
    memcpy(arrayData, &chunk[nameSize + sizeof(int)], arraySize);

    ArrayHeader header = *(ArrayHeader *)&arrayData[0];
    if (header.columnCount < 0 ||
        header.rowCount < 0 ||
        header.keyColumn < -1 ||
        header.keyColumn >= header.columnCount)
    {
        delete[] arrayData;
        delete[] fileData;
        return 32;
    }

    int offset = sizeof(ArrayHeader);
    for (c = 0; c < header.columnCount; ++c)
    {
        CKSTRING colName = (CKSTRING)&arrayData[offset];
        int colNameSize = strlen(colName) + 1;
        CK_ARRAYTYPE type = *(CK_ARRAYTYPE *)&arrayData[offset + colNameSize];
        switch (type)
        {
        case CKARRAYTYPE_INT:
            array->InsertColumn(-1, CKARRAYTYPE_INT, colName);
            break;
        case CKARRAYTYPE_FLOAT:
            array->InsertColumn(-1, CKARRAYTYPE_FLOAT, colName);
            break;
        case CKARRAYTYPE_STRING:
            array->InsertColumn(-1, CKARRAYTYPE_STRING, colName);
            break;
        default:
            break;
        }
        offset += colNameSize + sizeof(CK_ARRAYTYPE);
    }

    array->SetKeyColumn(header.keyColumn);

    for (i = 0; i < header.rowCount; ++i)
        array->InsertRow(-1);

    for (c = 0; c < header.columnCount; ++c)
    {
        CK_ARRAYTYPE type = array->GetColumnType(c);
        for (i = 0; i < header.rowCount; ++i)
        {
            switch (type)
            {
            case CKARRAYTYPE_INT:
            {
                int value = *(int *)&arrayData[offset];
                offset += sizeof(int);
                array->SetElementValue(i, c, &value, sizeof(int));
            }
            break;
            case CKARRAYTYPE_FLOAT:
            {
                float value = *(float *)&arrayData[offset];
                offset += sizeof(float);
                array->SetElementValue(i, c, &value, sizeof(float));
            }
            break;
            case CKARRAYTYPE_STRING:
            {
                char *str = (char *)&arrayData[offset];
                int size = strlen(str) + 1;
                char *nstr = new char[size];
                strncpy(nstr, str, size);
                offset += size;
                array->SetElementValue(i, c, nstr, size);
                delete[] nstr;
            }
            break;
            default:
                break;
            }
        }
    }

    delete[] arrayData;
    delete[] fileData;
    return 1;
}

int DatabaseManager::Save(CKContext *context)
{
    int n, i, c;
    int fileSize = 0;
    char *fileData = NULL;

    const int arrayNameCount = m_ArrayNames.Size();
    for (n = 0; n < arrayNameCount; ++n)
    {
        CKDataArray *array = (CKDataArray *)context->GetObjectByNameAndClass(m_ArrayNames[n], CKCID_DATAARRAY);
        if (!array)
            return 42;

        int chunkSize = 0;
        CKSTRING arrayName = array->GetName();
        chunkSize += (arrayName) ? strlen(arrayName) : 0;
        chunkSize += 1 + sizeof(int) + sizeof(ArrayHeader);

        const int columnCount = array->GetColumnCount();
        const int rowCount = array->GetRowCount();

        for (c = 0; c < columnCount; ++c)
        {
            CKSTRING colName = array->GetColumnName(c);
            chunkSize += (colName) ? strlen(colName) : 0;
            chunkSize += 1 + sizeof(int);
        }

        for (c = 0; c < columnCount; ++c)
        {
            CK_ARRAYTYPE type = array->GetColumnType(c);
            switch (type)
            {
            case CKARRAYTYPE_INT:
                chunkSize += rowCount * sizeof(int);
                break;
            case CKARRAYTYPE_FLOAT:
                chunkSize += rowCount * sizeof(float);
                break;
            case CKARRAYTYPE_STRING:
            {
                CKSTRING str = NULL;
                for (i = 0; i < rowCount; ++i)
                {
                    array->GetElementValue(i, c, &str);
                    if (str)
                        chunkSize += strlen(str);
                    chunkSize += 1;
                }
            }
            break;
            default:
                break;
            }
        }

        int offset = 0;
        int nameLength = 0;
        char *chunk = new char[chunkSize];

        arrayName = array->GetName();
        if (arrayName)
        {
            nameLength = strlen(arrayName);
            strncpy(&chunk[offset], arrayName, nameLength + 1);
        }
        else
        {
            nameLength = 0;
        }
        offset += nameLength + 1;

        *(int *)&chunk[offset] = chunkSize - (nameLength + 1 + sizeof(int));
        offset += sizeof(int);

        *(int *)&chunk[offset] = columnCount;
        offset += sizeof(int);

        *(int *)&chunk[offset] = rowCount;
        offset += sizeof(int);

        *(int *)&chunk[offset] = array->GetKeyColumn();
        offset += sizeof(int);

        for (c = 0; c < columnCount; ++c)
        {
            CKSTRING colName = array->GetColumnName(c);
            if (colName)
            {
                nameLength = strlen(colName);
                strncpy(&chunk[offset], colName, nameLength + 1);
            }
            else
            {
                nameLength = 0;
            }
            offset += nameLength + 1;

            *(CK_ARRAYTYPE *)&chunk[offset] = array->GetColumnType(c);
            offset += sizeof(CK_ARRAYTYPE);
        }

        for (c = 0; c < columnCount; ++c)
        {
            CK_ARRAYTYPE type = array->GetColumnType(c);
            for (i = 0; i < rowCount; ++i)
            {
                switch (type)
                {
                case CKARRAYTYPE_INT:
                {
                    int value = 0;
                    array->GetElementValue(i, c, &value);
                    *(int *)&chunk[offset] = value;
                    offset += sizeof(int);
                }
                break;
                case CKARRAYTYPE_FLOAT:
                {
                    float value = 0;
                    array->GetElementValue(i, c, &value);
                    *(float *)&chunk[offset] = value;
                    offset += sizeof(float);
                }
                break;
                case CKARRAYTYPE_STRING:
                {
                    char *str = NULL;
                    array->GetElementValue(i, c, &str);
                    if (str)
                    {
                        nameLength = strlen(str);
                        strncpy(&chunk[offset], str, nameLength + 1);
                    }
                    else
                    {
                        nameLength = 0;
                    }
                    offset += nameLength + 1;
                }
                break;
                default:
                    break;
                }
            }
        }

        if (fileSize == 0)
        {
            fileData = chunk;
        }
        else
        {
            char *NewFileData = new char[fileSize + chunkSize];
            memcpy(NewFileData, fileData, fileSize);
            memcpy(&NewFileData[fileSize], chunk, chunkSize);
            delete[] fileData;
            delete[] chunk;
            fileData = NewFileData;
        }

        fileSize += chunkSize;
    }

    if (!fileData)
        return 41;

    if (m_Crypted)
    {
        for (i = 0; i < fileSize; ++i)
            fileData[i] = rotr8(-fileData[i] ^ 0xAF, 3);
    }

    struct _stat buf;
    _stat(m_Filename, &buf);

    bool readable = (buf.st_mode & _S_IREAD) != 0;
    bool writable = (buf.st_mode & _S_IWRITE) != 0;

    _chmod(m_Filename, _S_IREAD | _S_IWRITE);
    FILE *fp = fopen(m_Filename, "wb");
    if (!fp)
        return 41;

    fwrite(fileData, fileSize, sizeof(char), fp);
    fclose(fp);

    if (readable)
        _chmod(m_Filename, _S_IREAD);
    else if (writable)
        _chmod(m_Filename, _S_IWRITE);
    else if (readable && writable)
        _chmod(m_Filename, _S_IREAD | _S_IWRITE);

    delete[] fileData;
    return 1;
}

bool DatabaseManager::SetProperty(CKSTRING filename, CKBOOL crypted)
{
    if (!filename || filename[0] == '\0')
        return false;

    m_Filename = filename;
    m_Crypted = crypted;

    return true;
}