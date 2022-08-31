/////////////////////////////////////
/////////////////////////////////////
//
//        TT MagnetMeshForm
//
/////////////////////////////////////
/////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTMagnetMeshFormBehaviorProto(CKBehaviorPrototype **pproto);
int TTMagnetMeshForm(const CKBehaviorContext& behcontext);
CKERROR TTMagnetMeshFormCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTMagnetMeshFormDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT MagnetMeshForm");
    od->SetDescription("Objekt wird magnetisch von eimem anderen angezogen");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x8e53929,0x4c9c5205));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTMagnetMeshFormBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTMagnetMeshFormBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT MagnetMeshForm");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Magnet-Entity", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Max Distance", CKPGUID_FLOAT, "5.0");
    proto->DeclareInParameter("Min Distance", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Strength", CKPGUID_FLOAT, "1");

    proto->DeclareLocalParameter("VArray", CKPGUID_POINTER, "NULL");
    proto->DeclareLocalParameter("Time", CKPGUID_FLOAT);

    proto->DeclareSetting("Sinus-Deform?", CKPGUID_BOOL, "false");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTMagnetMeshForm);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TTMagnetMeshFormCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTMagnetMeshForm(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTMagnetMeshFormCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}