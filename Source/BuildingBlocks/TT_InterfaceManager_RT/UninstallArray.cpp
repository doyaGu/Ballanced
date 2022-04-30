/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Uninstall Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include <stdio.h>

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorUninstallArrayDecl();
CKERROR CreateUninstallArrayProto(CKBehaviorPrototype **);
int UninstallArray(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorUninstallArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Uninstall Array");
    od->SetDescription("Uninstall Array from Manager");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x77DD56CF, 0x63DC30B4));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateUninstallArrayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateUninstallArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Uninstall Array");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Array to install", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Filename of current CMO", CKPGUID_STRING);
    proto->DeclareInParameter("Show message: UNINSTALLED", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(UninstallArray);

    *pproto = proto;
    return CK_OK;
}

int UninstallArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x1D, 1);
        TT_ERROR("UninstallArray.cpp", "int UninstallArray(...)", " gameInfo not exists");
    }

    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    CKSTRING cmo = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    CNemoArrayList *nemoArrayList = man->GetNemoArrayList();
    if (nemoArrayList->Search(cmo, array))
    {
        nemoArrayList->Remove(cmo, array);

        BOOL bShowMessage = FALSE;
        beh->GetInputParameterValue(2, &bShowMessage);
        if (bShowMessage)
        {
            char *msg = new char[256];
            sprintf(msg, " '%s'  from file '%s' uninstalled from manager", array->GetName(), cmo);
            ::MessageBoxA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), msg, "message...", MB_OK);
            delete[] msg;
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}