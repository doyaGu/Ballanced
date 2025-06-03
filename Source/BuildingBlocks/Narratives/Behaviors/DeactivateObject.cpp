/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DeactivateObject
//
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDeactivateObjectDecl();
CKERROR CreateDeactivateObjectProto(CKBehaviorPrototype **);
int DeactivateObject(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorDeactivateObjectDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Deactivate Object");
    od->SetDescription("De-activates all the scripts of an object in the current scene.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>object to be de-activated.<BR>
    <BR>
    */
    /* warning:
    - the object will actually be de-activated in the next frame.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x160f4b7d, 0x67de224e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeactivateObjectProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR CreateDeactivateObjectProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Deactivate Object");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeactivateObject);

    *pproto = proto;
    return CK_OK;
}

int DeactivateObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    //----- Get Object
    CKBeObject *bo = (CKBeObject *)beh->GetInputParameterObject(0);
    if (!bo)
        return CKBR_PARAMETERERROR;

    behcontext.CurrentScene->DeActivate(bo);

    return CKBR_OK;
}
