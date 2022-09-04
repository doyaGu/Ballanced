//////////////////////////////
//////////////////////////////
//
//        TT_GetTime
//
//////////////////////////////
//////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetTimeDecl();
CKERROR CreateGetTimeProto(CKBehaviorPrototype **pproto);
int GetTime(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetTimeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetTime");
    od->SetDescription("Chronometer (ON=Start, OFF=Stop).");
    od->SetCategory("TT Toolbox/Time");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3e715746, 0x45ce3b5a));
    od->SetAuthorGuid(CKGUID(0x53c80889, 0x57f84916));
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetTimeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetTimeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetTime");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("GameTime", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetTime);

    *pproto = proto;
    return CK_OK;
}

int GetTime(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}