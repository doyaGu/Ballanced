/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//                TT Register Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "DatabaseManager.h"

CKObjectDeclaration *FillBehaviorRegisterArrayDecl();
CKERROR CreateRegisterArrayProto(CKBehaviorPrototype **pproto);
int RegisterArray(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRegisterArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Register Array");
    od->SetDescription("Registers an array for storage in a database");
    od->SetCategory("TT Database");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x348773dc, 0x19ae6322));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRegisterArrayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRegisterArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Register Array");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Register");
    proto->DeclareInput("Clear");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Array", CKPGUID_DATAARRAY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RegisterArray);

    *pproto = proto;
    return CK_OK;
}

int RegisterArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    DatabaseManager *man = DatabaseManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_RegisterArray: dm==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0)) // Register
    {
        beh->ActivateInput(0, FALSE);

        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
        if (!array)
        {
            context->OutputToConsoleExBeep("TT_RegisterArray: No array specified");
            beh->ActivateOutput(1);
            return CKBR_OK;
        }

        int ret = man->Register(array->GetName());
        switch (ret)
        {
        case 1:
            beh->ActivateOutput(0);
            break;
        case 21:
            context->OutputToConsoleExBeep("TT_RegisterArray: Array is already registered");
            beh->ActivateOutput(1);
            break;
        default:
            context->OutputToConsoleExBeep("TT_RegisterArray: Error in registration");
            beh->ActivateOutput(1);
            break;
        }
    }
    else if (beh->IsInputActive(1)) // Clear
    {
        beh->ActivateInput(1, FALSE);
        man->Clear();
        beh->ActivateOutput(0);
    }

    return CKBR_OK;
}