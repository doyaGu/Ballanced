/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//                 TT Load Database
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "DatabaseManager.h"

CKObjectDeclaration *FillBehaviorLoadDatabaseDecl();
CKERROR CreateLoadDatabaseProto(CKBehaviorPrototype **pproto);
int LoadDatabase(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLoadDatabaseDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Load Database");
    od->SetDescription("Get arrays from a database");
    od->SetCategory("TT Database");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5441494, 0x38ac7789));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLoadDatabaseProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLoadDatabaseProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Load Database");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Load");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Auto Register", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Array Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LoadDatabase);

    *pproto = proto;
    return CK_OK;
}

int LoadDatabase(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    DatabaseManager *man = DatabaseManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_LoadDatabase: dm==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
        beh->ActivateInput(0, FALSE);

    CKBOOL autoRegister;
    beh->GetInputParameterValue(0, &autoRegister);
    CKSTRING arrayName = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    int ret = man->Load(context, autoRegister == TRUE, arrayName);
    switch (ret)
    {
    case 1:
        beh->ActivateOutput(0);
        break;
    case 31:
        context->OutputToConsoleExBeep("TT_LoadDatabase: Error when opening file");
        beh->ActivateOutput(1);
        break;
    case 32:
        context->OutputToConsoleExBeep("TT_LoadDatabase: File content error");
        beh->ActivateOutput(1);
        break;
    case 33:
        context->OutputToConsoleExBeep("TT_LoadDatabase: There is no array in the file or the specified array does not exist");
        beh->ActivateOutput(1);
        break;
    default:
        beh->ActivateOutput(1);
        break;
    }

    return CKBR_OK;
}