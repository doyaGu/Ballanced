//////////////////////////////
//////////////////////////////
//
//        TT_BumpMap
//
//////////////////////////////
//////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTBumpMapBehaviorProto(CKBehaviorPrototype **pproto);
int TTBumpMap(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTBumpMapDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_BumpMap");
    od->SetDescription("erzeugt lichtabh�ngiges BumpMapping");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x630d05f7,0x1dda541f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTBumpMapBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTBumpMapBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_BumpMap");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Light", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Emboss-Factor", CKPGUID_FLOAT, "0.02");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTBumpMap);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    *pproto = proto;
    return CK_OK;
}

int TTBumpMap(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTBumpMapCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}