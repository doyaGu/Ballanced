/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            LaunchScene
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLaunchSceneDecl();
CKERROR CreateLaunchSceneProto(CKBehaviorPrototype **);
int LaunchSceneOldVersion(const CKBehaviorContext &context);
int LaunchScene(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorLaunchSceneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Launch Scene");
    od->SetDescription("Launches a specified scene.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scene: </SPAN>the scene name.<BR>
    <SPAN CLASS=pin>Activity Options: </SPAN><BR>
    <U>Scene Defaults: </U>use the 'Activation' flag of the scene (the one that appear in the Level View).<BR>
    <U>Force activate: </U>force the launched scene to be active.<BR>
    <U>Force deactivate: </U>force the launched scene to be deactivate.<BR>
    <U>Do nothing: </U>the state of all the objects is kept as they were. Thus if you choose this option, the "Launch Scene" BB will
    only hide object that aren't in the scene, and show the ones that are in the scene.<BR>
    <BR>
    <SPAN CLASS=pin>Reset Options: </SPAN><BR>
    <U>Scene Defaults: </U>use the 'Reset at Start' flag of the scene (the one that appear in the Level View).<BR>
    <U>Force Reset: </U>force the launched scene to be reset (restore initial conditions).<BR>
    <U>Do nothing: </U>do not restore initial conditions.<BR>
    <BR>
    */
    /* warning:
    - The scene will actually be launched next frame.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x188d6d43, 0x169613dd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLaunchSceneProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Scene Management");
    return od;
}

#define NOPARAM_VERSION 0x00010000
#define CURRENT_VERSION 0x00020000

CKERROR CreateLaunchSceneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Launch Scene");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scene", CKPGUID_SCENE);
    proto->DeclareInParameter("Activity Options", CKPGUID_SCENEACTIVITYFLAGS, "0");
    proto->DeclareInParameter("Reset Options", CKPGUID_SCENERESETFLAGS, "0");

    //  Old Version
    //	proto->DeclareInParameter("Reset IC", CKPGUID_BOOL,"FALSE");
    //	proto->DeclareInParameter("Reset LastTime IC", CKPGUID_BOOL,"FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LaunchScene);

    *pproto = proto;
    return CK_OK;
}

int LaunchScene(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK_SCENEOBJECTACTIVITY_FLAGS ActivityFlags = CK_SCENEOBJECTACTIVITY_SCENEDEFAULT;
    CK_SCENEOBJECTRESET_FLAGS ResetFlags = CK_SCENEOBJECTRESET_SCENEDEFAULT;

    CKParameterIn *pin = beh->GetInputParameter(1);
    if (pin->GetGUID() == CKPGUID_BOOL)
    {
        CKBOOL ic = FALSE;
        beh->GetInputParameterValue(1, &ic);

        ActivityFlags = CK_SCENEOBJECTACTIVITY_SCENEDEFAULT;
        if (!ic)
        {
            ResetFlags = CK_SCENEOBJECTRESET_DONOTHING;
        }
        else
        {
            ResetFlags = CK_SCENEOBJECTRESET_SCENEDEFAULT;
        }
    }
    else
    {
        beh->GetInputParameterValue(1, &ActivityFlags);
        beh->GetInputParameterValue(2, &ResetFlags);
    }

    CKScene *scene = (CKScene *)beh->GetInputParameterObject(0);
    CKLevel *level = behcontext.CurrentLevel;

    // No scene provided ? we launch the default scene
    if (!scene)
        scene = level->GetLevelScene();

    if (scene != level->GetCurrentScene() || (ResetFlags == CK_SCENEOBJECTRESET_RESET))
        level->SetNextActiveScene(scene, ActivityFlags, ResetFlags);

    return CKBR_OK;
}
