/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		      TT Set Database Properties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_DatabaseManager_RT.h"

#include "DatabaseManager.h"

CKObjectDeclaration *FillBehaviorSetDatabasePropertiesDecl();
CKERROR CreateSetDatabasePropertiesProto(CKBehaviorPrototype **);
int SetDatabaseProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetDatabasePropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Database Properties");
    od->SetDescription("Sets the properties of the database");
    od->SetCategory("TT Database");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1436624F, 0x34E1290A));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetDatabasePropertiesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetDatabasePropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Database Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Set");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("File", CKPGUID_STRING);
    proto->DeclareInParameter("Crypted", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetDatabaseProperties);

    *pproto = proto;
    return CK_OK;
}

int SetDatabaseProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    CTTDatabaseManager *man = CTTDatabaseManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetDatabaseProperties: dm==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CKSTRING filename = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    BOOL crypted = (BOOL)beh->GetInputParameterReadDataPtr(1);

    if (!filename)
    {
        context->OutputToConsoleExBeep("TT_SetDatabaseProperties: No file specified");
        beh->ActivateOutput(1);
    }

    man->SetProperty(filename, crypted);

    beh->ActivateOutput(0);
    return CKBR_OK;
}