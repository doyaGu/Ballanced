/////////////////////////////////////
/////////////////////////////////////
//
//        TT PMS_Meshdeform
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorPMSMeshdeformDecl();
CKERROR CreatePMSMeshdeformProto(CKBehaviorPrototype **pproto);
int PMSMeshdeform(const CKBehaviorContext &behcontext);
CKERROR PMSMeshdeformCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPMSMeshdeformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PMS_Meshdeform");
    od->SetDescription("generiert Wellen �ber Pierson-Moskowitz-Spektrum");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38b87d43, 0x149b755b));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePMSMeshdeformProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreatePMSMeshdeformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PMS_Meshdeform");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Winddirection", CKPGUID_ANGLE, "0");
    proto->DeclareInParameter("Max. Amplitude", CKPGUID_FLOAT, "0.6");
    proto->DeclareInParameter("WaveScale", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("TimeScale", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("1.WaveLength in Y", CKPGUID_FLOAT, "24");
    proto->DeclareInParameter("1.WaveLength in X", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("1.Amplitude in X", CKPGUID_FLOAT, "0.5");

    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("Time", CKPGUID_FLOAT);
    proto->DeclareLocalParameter("WaveX", CKPGUID_POINTER);
    proto->DeclareLocalParameter("WaveY", CKPGUID_POINTER);
    proto->DeclareLocalParameter("WindMatrix", CKPGUID_MATRIX);

    proto->DeclareSetting("Wellenanzahl", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PMSMeshdeform);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PMSMeshdeformCallBack);

    *pproto = proto;
    return CK_OK;
}

int PMSMeshdeform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR PMSMeshdeformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}