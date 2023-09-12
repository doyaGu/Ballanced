/////////////////////////////////////////////
/////////////////////////////////////////////
//
//        TT GetEnvironmentVariable
//
/////////////////////////////////////////////
/////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <stdlib.h>

CKObjectDeclaration *FillBehaviorGetEnvironmentVariableDecl();
CKERROR CreateGetEnvironmentVariableProto(CKBehaviorPrototype **pproto);
int GetEnvironmentVariable(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetEnvironmentVariableDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetEnvironmentVariable");
    od->SetDescription("Gets the value of a square");
    od->SetCategory("TT Toolbox/Advanced");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c6513e9, 0x20e11177));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetEnvironmentVariableProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetEnvironmentVariableProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetEnvironmentVariable");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Go and get it!");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Variablename", CKPGUID_STRING, "N2DEVPATH");

    proto->DeclareOutParameter("Environment Variable", CKPGUID_STRING, "C:\\Programme\\Virtools Dev 2.0\\");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetEnvironmentVariable);

    *pproto = proto;
    return CK_OK;
}

int GetEnvironmentVariable(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    CKSTRING varName = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!varName)
    {
        beh->ActivateOutput(1);
        context->OutputToConsole("No Variablename defined.", FALSE);
        return CKBR_PARAMETERERROR;
    }

    const char *env = getenv(varName);
    if (!env)
    {
        beh->ActivateOutput(1);
        context->OutputToConsole("No Variable with the specified name found.", FALSE);
        return CKBR_PARAMETERERROR;
    }

    beh->SetOutputParameterValue(0, env, strlen(env) + 1);
    beh->ActivateOutput(0);
    return CKBR_OK;
}