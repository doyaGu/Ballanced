/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT Read Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include <stdio.h>

#include "InterfaceManager.h"
#include "ErrorProtocol.h"

CKObjectDeclaration *FillBehaviorReadArrayDecl();
CKERROR CreateReadArrayProto(CKBehaviorPrototype **pproto);
int ReadArray(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReadArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Read Array");
    od->SetDescription("Reads arrays from manager");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5c825017, 0x6d367973));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReadArrayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReadArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Read Array");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Name of Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Filename of CMO", CKPGUID_STRING);
    proto->DeclareInParameter("Show message: 'red array'", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReadArray);

    *pproto = proto;
    return CK_OK;
}

int ReadArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x19, 1);
        TT_ERROR("ReadArray.cpp", "int ReadArray(...)", " gameInfo not exists");
    }

    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    CKSTRING cmo = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    CNemoArrayList *nemoArraylist = man->GetNemoArrayList();
    CNemoArray *nemoArray = nemoArraylist->Search(cmo, array);
    if (nemoArray)
    {
        nemoArray->Read(array);

        BOOL bShowMessage = FALSE;
        beh->GetInputParameterValue(2, &bShowMessage);
        if (bShowMessage)
        {
            char *msg = new char[128];
            sprintf(msg, " '%s'  from file '%s' read from manager", array->GetName(), cmo);
            ::MessageBoxA((HWND)context->GetMainWindow(), msg, "message...", MB_OK);
            delete[] msg;
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}