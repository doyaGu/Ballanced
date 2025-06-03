/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AddToScene
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddToSceneDecl();
CKERROR CreateAddToSceneProto(CKBehaviorPrototype **);
int AddToScene(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorAddToSceneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Object To Scene");
    od->SetDescription("Add an object to a specified scene.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scene: </SPAN>the scene name. NULL means the current scene.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f7d5221, 0x4c852520));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddToSceneProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Scene Management");
    return od;
}

CKERROR CreateAddToSceneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Object To Scene");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scene", CKPGUID_SCENE);
    proto->DeclareInParameter("Copy Current Scene Activation", CKPGUID_BOOL);
    proto->DeclareInParameter("Activity Options", CKPGUID_SCENEACTIVITYFLAGS, "0");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AddToScene);

    *pproto = proto;
    return CK_OK;
}

int AddToScene(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // We get the target object
    CKSceneObject *sceneobject = (CKSceneObject *)beh->GetTarget();

    // we get the scene
    CKScene *scene = (CKScene *)beh->GetInputParameterObject(0);
    if (!scene)
        scene = behcontext.CurrentScene;

    // we add the object to the scene
    scene->AddObjectToScene(sceneobject);

    CKBOOL copyActivation = FALSE;
    beh->GetInputParameterValue(1, &copyActivation);

    if (copyActivation && (scene != behcontext.CurrentScene))
    {
        // we shall copy the activation flags of the current scene
        // to the other scene for the object and its scripts

        // first the beobject
        CK_SCENEOBJECT_FLAGS oflags = behcontext.CurrentScene->GetObjectFlags(sceneobject);
        scene->SetObjectFlags(sceneobject, oflags);

        // then the scripts
        CKBeObject *beo = CKBeObject::Cast(sceneobject);
        if (beo)
        {
            int scount = beo->GetScriptCount();
            for (int i = 0; i < scount; ++i)
            {
                CKBehavior *script = beo->GetScript(i);

                CK_SCENEOBJECT_FLAGS sflags = behcontext.CurrentScene->GetObjectFlags(script);
                scene->SetObjectFlags(script, sflags);
            }
        }
    }
    else
    {
        CK_SCENEOBJECTACTIVITY_FLAGS activityFlags = CK_SCENEOBJECTACTIVITY_SCENEDEFAULT;
        beh->GetInputParameterValue(2, &activityFlags);

        if (activityFlags != CK_SCENEOBJECTACTIVITY_SCENEDEFAULT) // something to do ?
        {
            CKDWORD oflags = scene->GetObjectFlags(sceneobject);
            oflags &= ~(CK_SCENEOBJECT_START_ACTIVATE | CK_SCENEOBJECT_START_DEACTIVATE | CK_SCENEOBJECT_START_LEAVE); // clear all traces of activity

            switch (activityFlags)
            {
            case CK_SCENEOBJECTACTIVITY_SCENEDEFAULT:
                oflags |= CK_SCENEOBJECT_START_ACTIVATE;
                break;
            case CK_SCENEOBJECTACTIVITY_ACTIVATE:
                oflags |= CK_SCENEOBJECT_START_ACTIVATE;
                break;
            case CK_SCENEOBJECTACTIVITY_DEACTIVATE:
                oflags |= CK_SCENEOBJECT_START_DEACTIVATE;
                break;
            case CK_SCENEOBJECTACTIVITY_DONOTHING:
                oflags |= CK_SCENEOBJECT_START_LEAVE;
                break;
            }

            scene->SetObjectFlags(sceneobject, (CK_SCENEOBJECT_FLAGS)oflags);
        }
    }

    return CKBR_OK;
}
