////////////////////////////////////////
////////////////////////////////////////
//
//        TT_LoadMotorSettings
//
////////////////////////////////////////
////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorLoadMotorSettingsDecl();
CKERROR CreateLoadMotorSettingsProto(CKBehaviorPrototype **pproto);
int LoadMotorSettings(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLoadMotorSettingsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_LoadMotorSettings");
    od->SetDescription("Loads sound settings for TT_MotorSound behavior");
    od->SetCategory("TT Toolbox/Sounds");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf822838, 0x74116583));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLoadMotorSettingsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(SOUND_MANAGER_GUID);
    return od;
}

CKERROR CreateLoadMotorSettingsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_LoadMotorSettings");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Start");

    proto->DeclareOutput("Loading finished");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("SettingsFile", CKPGUID_STRING);

    proto->DeclareLocalParameter("FirstTime", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LoadMotorSettings);

    *pproto = proto;
    return CK_OK;
}

int LoadMotorSettings(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}