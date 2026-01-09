////////////////////////////////////////
////////////////////////////////////////
//
//        TT_LoadMotorSettings
//
////////////////////////////////////////
////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "MotorSettings.h"

#include <stdio.h>

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
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    // Get the settings file name
    char fileName[240];
    beh->GetInputParameterValue(0, fileName);

    FILE *file = fopen(fileName, "rt");
    if (file)
    {
        // Read volume
        fscanf(file, "Volume:%f\n", &g_MotorVolume);

        // Read channel 1 settings (low speed)
        fscanf(file, "PitchMax:%f PitchMin:%f vMax:%f vMin:%f vFadeIn:%f vFadeOut:%f\n",
               &g_MotorChannel1.pitchMax,
               &g_MotorChannel1.pitchMin,
               &g_MotorChannel1.vMax,
               &g_MotorChannel1.vMin,
               &g_MotorChannel1.vFadeIn,
               &g_MotorChannel1.vFadeOut);

        // Read channel 2 settings (mid speed)
        fscanf(file, "PitchMax:%f PitchMin:%f vMax:%f vMin:%f vFadeIn:%f vFadeOut:%f\n",
               &g_MotorChannel2.pitchMax,
               &g_MotorChannel2.pitchMin,
               &g_MotorChannel2.vMax,
               &g_MotorChannel2.vMin,
               &g_MotorChannel2.vFadeIn,
               &g_MotorChannel2.vFadeOut);

        // Read channel 3 settings (high speed)
        fscanf(file, "PitchMax:%f PitchMin:%f vMax:%f vMin:%f vFadeIn:%f vFadeOut:%f\n",
               &g_MotorChannel3.pitchMax,
               &g_MotorChannel3.pitchMin,
               &g_MotorChannel3.vMax,
               &g_MotorChannel3.vMin,
               &g_MotorChannel3.vFadeIn,
               &g_MotorChannel3.vFadeOut);

        fclose(file);
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        // File not found - show error message
        ctx->OutputToConsoleExBeep("File Motor.txt nicht gefunden!");
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}