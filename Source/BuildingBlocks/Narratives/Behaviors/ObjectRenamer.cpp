#include "CKAll.h"

CKERROR CreateObjectRenameProto(CKBehaviorPrototype **pproto);
int ObjectRename(const CKBehaviorContext &context);
CKERROR ObjectRenameCB(int ckm, CKBehavior *beh, void *arg);

CKObjectDeclaration *FillObjectRenameDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Rename");
    od->SetDescription("Renames an object");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Name: </SPAN>new name to give to the object.<BR>
    <BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6f074ca7, 0x45427431));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectRenameProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObjectRenameProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Rename");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_OBJECT);
    proto->DeclareInParameter("Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ObjectRename);

    *pproto = proto;
    return CK_OK;
}

int ObjectRename(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKObject *beo = (CKObject *)beh->GetInputParameterObject(0);
    if (!beo)
        return CKBR_PARAMETERERROR;

    CKSTRING name = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    beo->SetName(name);

    return CKBR_OK;
}

CKERROR CreateObsoleteObjectRenamerProto(CKBehaviorPrototype **pproto);
int ObsoleteObjectRenamer(const CKBehaviorContext &context);
CKERROR ObsoleteObjectRenamerCB(int ckm, CKBehavior *beh, void *arg);

CKObjectDeclaration *FillObsoleteObjectRenamerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Rename");
    od->SetDescription("Renames an object");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Name: </SPAN>new name to give to the object.<BR>
    <BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4cae18cc, 0x3520c5a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObsoleteObjectRenamerProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObsoleteObjectRenamerProto(CKBehaviorPrototype **pproto)
{

    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Rename");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(ObsoleteObjectRenamer);

    *pproto = proto;
    return CK_OK;
}

int ObsoleteObjectRenamer(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *beo = beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    CKSTRING name = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    beo->SetName(name);

    return CKBR_OK;
}
