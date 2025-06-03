/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RemoveFromScene
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveFromSceneDecl();
CKERROR CreateRemoveFromSceneProto(CKBehaviorPrototype **);
int RemoveFromScene(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorRemoveFromSceneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Object From Scene");
    od->SetDescription("Remove an object from a specified scene.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scene: </SPAN>the scene name. NULL means the current scene.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x683925f3, 0x6fdd3f16));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveFromSceneProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Scene Management");
    return od;
}

CKERROR CreateRemoveFromSceneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Object From Scene");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scene", CKPGUID_SCENE);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RemoveFromScene);

    *pproto = proto;
    return CK_OK;
}

int RemoveFromScene(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // We get the target object
    CKSceneObject *sceneobject = (CKSceneObject *)beh->GetTarget();

    // we get the scene
    CKScene *scene = (CKScene *)beh->GetInputParameterObject(0);

    // we add the object to the scene
    if (scene)
        scene->RemoveObjectFromScene(sceneobject);
    else
        behcontext.CurrentScene->RemoveObjectFromScene(sceneobject);

    return CKBR_OK;
}
