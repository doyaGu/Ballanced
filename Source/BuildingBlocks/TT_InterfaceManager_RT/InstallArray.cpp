/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Install Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include <stdio.h>

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorInstallArrayDecl();
CKERROR CreateInstallArrayProto(CKBehaviorPrototype **);
int InstallArray(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInstallArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Install Array");
    od->SetDescription("Install Array to Manager");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x41F47EC4, 0x69470BBD));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInstallArrayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateInstallArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Install Array");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Array to install", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Filename of current CMO", CKPGUID_STRING);
    proto->DeclareInParameter("Show message: INSTALLED", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(InstallArray);

    *pproto = proto;
    return CK_OK;
}

int InstallArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x1A, 1);
        TT_ERROR("InstallArray.cpp", "int InstallArray(...)", " gameInfo not exists");
    }

    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    CKSTRING cmo = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    CNemoArrayList *nemoArrayList = man->GetNemoArrayList();
    if (!nemoArrayList->Search(cmo, array))
    {
		nemoArrayList->Add(cmo, array);

        BOOL bShowMessage = FALSE;
        beh->GetInputParameterValue(2, &bShowMessage);
        if (bShowMessage)
        {
            char *msg = new char[256];
            sprintf(msg, " '%s'  from file '%s' installed to manager", array->GetName(), cmo);
            ::MessageBoxA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), msg, "message...", MB_OK);
            delete[] msg;
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}