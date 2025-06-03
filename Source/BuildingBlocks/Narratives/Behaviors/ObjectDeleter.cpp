#include "CKAll.h"

CKERROR CreateObjectDeleterProto(CKBehaviorPrototype **pproto);
int ObjectDeleter(const CKBehaviorContext &context);
CKERROR ObjectDeleterCB(int ckm, CKBehavior *beh, void *arg);

extern void CleanUp(CKBehavior *beh);
extern void Load(CKBehavior *beh, CKScene *scn);

CKObjectDeclaration *FillObjectDeleterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Delete");
    od->SetDescription("Deletes an object");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>object to delete.<BR>
    <SPAN CLASS=pin>Dependencies Options: </SPAN>options of deletion, sorted by object class.<BR>
    <BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x74120ded, 0x76524673));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateObjectDeleterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObjectDeleterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Delete");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_OBJECT);
    proto->DeclareInParameter("Dependency Options", CKPGUID_DELETEDEPENDENCIES);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ObjectDeleter);

    *pproto = proto;
    return CK_OK;
}

int ObjectDeleter(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    if (beh->GetInputParameter(1)) // new version
    {
        CKDependencies *dep = *(CKDependencies **)beh->GetInputParameterReadDataPtr(1);

        CKObject *beo = beh->GetInputParameterObject(0);
        if (!beo)
            return CKBR_OWNERERROR;

        // We mark the object to be deleted
        context.Context->DestroyObject(beo->GetID(), 0, dep);
    }
    else // Old version
    {
        CKDependencies *dep = *(CKDependencies **)beh->GetInputParameterReadDataPtr(0);

        CKBeObject *beo = beh->GetTarget();
        if (!beo)
            return CKBR_OWNERERROR;

        // We mark the object to be deleted
        context.Context->DestroyObject(beo->GetID(), 0, dep);
    }

    return CKBR_OK;
}
