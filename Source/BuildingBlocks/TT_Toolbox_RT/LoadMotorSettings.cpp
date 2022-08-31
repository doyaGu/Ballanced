////////////////////////////////////////
////////////////////////////////////////
//
//        TT_LoadMotorSettings
//
////////////////////////////////////////
////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTLoadMotorSettingsBehaviorProto(CKBehaviorPrototype **pproto);
int TTLoadMotorSettings(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTLoadMotorSettingsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_LoadMotorSettings");
    od->SetDescription("Loads sound settings for TT_MotorSound behavior");
    od->SetCategory("TT Toolbox/Sounds");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf822838,0x74116583));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTLoadMotorSettingsBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTLoadMotorSettingsBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_LoadMotorSettings");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Start");

    proto->DeclareOutput("Loading finished");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("SettingsFile", CKPGUID_STRING);

    proto->DeclareLocalParameter("FirstTime", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTLoadMotorSettings);

    *pproto = proto;
    return CK_OK;
}

int TTLoadMotorSettings(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTLoadMotorSettingsCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}