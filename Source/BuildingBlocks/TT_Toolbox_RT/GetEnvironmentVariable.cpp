/////////////////////////////////////////////
/////////////////////////////////////////////
//
//        TT GetEnvironmentVariable
//
/////////////////////////////////////////////
/////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTGetEnvironmentVariableBehaviorProto(CKBehaviorPrototype **pproto);
int TTGetEnvironmentVariable(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTGetEnvironmentVariableDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetEnvironmentVariable");
    od->SetDescription("Gets the value of a square");
    od->SetCategory("TT Toolbox/Advanced");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c6513e9,0x20e11177));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTGetEnvironmentVariableBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTGetEnvironmentVariableBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetEnvironmentVariable");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Go and get it!");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Variablename", CKPGUID_STRING, "N2DEVPATH");

    proto->DeclareOutParameter("Environment Variable", CKPGUID_STRING, "C:\Programme\Virtools Dev 2.0\");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTGetEnvironmentVariable);

    *pproto = proto;
    return CK_OK;
}

int TTGetEnvironmentVariable(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTGetEnvironmentVariableCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}