//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_ListDir
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

CKObjectDeclaration *FillBehaviorListDirDecl();
CKERROR CreateListDirProto(CKBehaviorPrototype **pproto);
int ListDir(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListDirDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ListDir");
    od->SetDescription("Checks for existing of a File");
    od->SetCategory("TT Toolbox/File");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x496f429e, 0x7d602e9d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateListDirProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateListDirProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ListDir");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("OUT");

    proto->DeclareInParameter("SearchPath", CKPGUID_STRING);
    proto->DeclareInParameter("Array", CKPGUID_DATAARRAY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ListDir);

    *pproto = proto;
    return CK_OK;
}

int ListDir(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKSTRING searchPath = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(1);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        ctx->OutputToConsole("Invalid file handle", TRUE);
        return CKBR_GENERICERROR;
    }

    // Skip "." and ".." entries
    if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
    {
        array->InsertRow(-1);
        int row = array->GetRowCount() - 1;
        if (!array->SetElementStringValue(row, 0, findData.cFileName))
            array->RemoveRow(row);
    }

    while (FindNextFileA(hFind, &findData))
    {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
        {
            array->InsertRow(-1);
            int row = array->GetRowCount() - 1;
            if (!array->SetElementStringValue(row, 0, findData.cFileName))
                array->RemoveRow(row);
        }
    }

    DWORD err = GetLastError();
    if (err != ERROR_NO_MORE_FILES)
    {
        ctx->OutputToConsole("Invalid file handle", TRUE);
        FindClose(hFind);
        return CKBR_GENERICERROR;
    }

    if (!FindClose(hFind))
    {
        ctx->OutputToConsole("Can't close find handle", TRUE);
        return CKBR_GENERICERROR;
    }

    return CKBR_OK;
}