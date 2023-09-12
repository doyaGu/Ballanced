/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT Write Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorWriteArrayDecl();
CKERROR CreateWriteArrayProto(CKBehaviorPrototype **pproto);
int WriteArray(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWriteArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Write Array");
    od->SetDescription("Writes Arrays to manager");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7414af4, 0x505e52ee));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWriteArrayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWriteArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Write Array");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Name of Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Filename of CMO", CKPGUID_STRING);
    proto->DeclareInParameter("Show message: 'wrote array'", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WriteArray);

    *pproto = proto;
    return CK_OK;
}

int WriteArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x1E, 1);
        context->OutputToConsoleExBeep("WriteArray: gameInfo not exists");
    }

    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    CKSTRING cmo = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    CNemoArrayList *nemoArraylist = man->GetNemoArrayList();
    CNemoArray *nemoArray = nemoArraylist->Search(cmo, array);
    if (nemoArray)
    {
        nemoArray->Write(array);

        BOOL bShowMessage = FALSE;
        beh->GetInputParameterValue(2, &bShowMessage);
        if (bShowMessage)
        {
            char *msg = new char[256];
            sprintf(msg, " '%s'  from file '%s' written to manager", array->GetName(), cmo);
            ::MessageBoxA((HWND)context->GetMainWindow(), msg, "message...", MB_OK);
            delete[] msg;
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}