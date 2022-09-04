//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT Scaleable Proximity
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorScaleableProximityDecl();
CKERROR CreateScaleableProximityProto(CKBehaviorPrototype **pproto);
int ScaleableProximity(const CKBehaviorContext &behcontext);
CKERROR ScaleableProximityCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorScaleableProximityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Scaleable Proximity");
    od->SetDescription("Compares the distance between two 3D Entities with a given threshold distance.");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2a2a63ca, 0x826175));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateScaleableProximityProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateScaleableProximityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Scaleable Proximity");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("InRange");
    proto->DeclareOutput("OutRange");
    proto->DeclareOutput("EnterRange");
    proto->DeclareOutput("ExitRange");

    proto->DeclareInParameter("Distance", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("ObjectA", CKPGUID_3DENTITY);
    proto->DeclareInParameter("ObjectB", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Barycenter?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Exactness min. Distance", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Exactness max. Distance", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Minimum Framedelay", CKPGUID_INT, "1");
    proto->DeclareInParameter("Maximum Framedelay", CKPGUID_INT, "10");

    proto->DeclareOutParameter("Current Distance", CKPGUID_FLOAT);

    proto->DeclareLocalParameter("", CKPGUID_INT, "2");
    proto->DeclareLocalParameter("Last Check", CKPGUID_INT, "1");

    proto->DeclareSetting("Use Ouputs:", CKPGUID_PROXIMITY, "InRange,OutRange,EnterRange,ExitRange");
    proto->DeclareSetting("Check Axis:", CKPGUID_CHECKAXIS, "X,Y,Z");
    proto->DeclareSetting("Squared Distance?", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ScaleableProximity);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetBehaviorCallbackFct(ScaleableProximityCallBack);

    *pproto = proto;
    return CK_OK;
}

int ScaleableProximity(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR ScaleableProximityCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}