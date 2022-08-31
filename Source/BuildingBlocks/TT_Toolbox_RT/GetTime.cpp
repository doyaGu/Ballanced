//////////////////////////////
//////////////////////////////
//
//        TT_GetTime
//
//////////////////////////////
//////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTGetTimeBehaviorProto(CKBehaviorPrototype **pproto);
int TTGetTime(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTGetTimeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetTime");
    od->SetDescription("Chronometer (ON=Start, OFF=Stop).");
    od->SetCategory("TT Toolbox/Time");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3e715746,0x45ce3b5a));
    od->SetAuthorGuid(CKGUID(0x53c80889,0x57f84916));
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTGetTimeBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTGetTimeBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetTime");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("GameTime", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTGetTime);

    *pproto = proto;
    return CK_OK;
}

int TTGetTime(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTGetTimeCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}