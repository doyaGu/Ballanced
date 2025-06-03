/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					GetCurrentScene
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetCurrentSceneDecl();
CKERROR CreateGetCurrentSceneProto(CKBehaviorPrototype **);
int GetCurrentScene(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorGetCurrentSceneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Current Scene");
    od->SetDescription("Returns the scene that is currently being played.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>Scene: </SPAN>name of the scene currently being played.<BR>
    <BR>
    See also : Launch Scene
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x00dc125f, 0x592b00a8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurrentSceneProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Scene Management");
    return od;
}

CKERROR CreateGetCurrentSceneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Current Scene");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Scene", CKPGUID_SCENE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurrentScene);

    *pproto = proto;
    return CK_OK;
}

int GetCurrentScene(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (behcontext.CurrentScene == behcontext.CurrentLevel->GetLevelScene())
        beh->SetOutputParameterObject(0, NULL);
    else
        beh->SetOutputParameterObject(0, behcontext.CurrentScene);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
