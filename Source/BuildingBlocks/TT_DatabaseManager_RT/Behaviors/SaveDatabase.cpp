/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//                TT Save Database
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "DatabaseManager.h"

CKObjectDeclaration *FillBehaviorSaveDatabaseDecl();
CKERROR CreateSaveDatabaseProto(CKBehaviorPrototype **pproto);
int SaveDatabase(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSaveDatabaseDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Save Database");
    od->SetDescription("Stores the registered arrays to a database");
    od->SetCategory("TT Database");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5d303e9d, 0x552c0af2));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSaveDatabaseProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSaveDatabaseProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Save Database");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Save");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SaveDatabase);

    *pproto = proto;
    return CK_OK;
}

int SaveDatabase(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    DatabaseManager *man = DatabaseManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SaveDatabase: dm==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    int ret = man->Save(context);
    switch (ret)
    {
    case 1:
        beh->ActivateOutput(0);
        break;
    case 41:
        context->OutputToConsoleExBeep("TT_SaveDatabase: File content error");
        beh->ActivateOutput(1);
        break;
    case 42:
        context->OutputToConsoleExBeep("TT_SaveDatabase: A registered array is no longer available! File was NOT saved");
        beh->ActivateOutput(1);
        break;
    default:
        beh->ActivateOutput(1);
        break;
    }

    return CKBR_OK;
}