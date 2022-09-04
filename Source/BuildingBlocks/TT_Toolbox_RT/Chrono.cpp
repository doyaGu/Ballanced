/////////////////////////////
/////////////////////////////
//
//        TT_Chrono
//
/////////////////////////////
/////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorChronoDecl();
CKERROR CreateChronoProto(CKBehaviorPrototype **pproto);
int Chrono(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChronoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Chrono");
    od->SetDescription("Chronometer (ON=Start, OFF=Stop).");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x325d722d, 0x3d9e16e9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChronoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateChronoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Chrono");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Stop");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Stop");
    proto->DeclareOutput("Exit Off");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Chrono);

    *pproto = proto;
    return CK_OK;
}

int Chrono(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}