#include "CKAll.h"

CKERROR CreateObjectCreatorProto(CKBehaviorPrototype **pproto);
int ObjectCreator(const CKBehaviorContext &context);
CKERROR ObjectCreatorCB(int ckm, CKBehavior *beh, void *arg);

extern void CleanUp(CKBehavior *beh);
extern void Load(CKBehavior *beh, CKScene *scn);

CKObjectDeclaration *FillObjectCreatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Create");
    od->SetDescription("Creates an object");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Class: </SPAN>class of the object to create.<BR>
    <SPAN CLASS=pin>Name: </SPAN>name of the object to create.<BR>
    <BR>
    <SPAN CLASS=setting>Dynamic: </SPAN>specifies if the object created must be dynamics.<BR>
    <BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x271538e6, 0x2fae49ac));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectCreatorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObjectCreatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Create");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Class", CKPGUID_CLASSID, "3D Entity");
    proto->DeclareInParameter("Name", CKPGUID_STRING, "New Object");

    proto->DeclareSetting("Dynamic", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Object", CKPGUID_OBJECT, NULL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ObjectCreator);

    *pproto = proto;
    return CK_OK;
}

int ObjectCreator(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Dynamic ?
    CKBOOL dynamic = TRUE;
    beh->GetLocalParameterValue(0, &dynamic);

    int classid = CKCID_3DENTITY;
    beh->GetInputParameterValue(0, &classid);

    if (CKIsChildClassOf(classid, CKCID_LEVEL))
    {
        context.Context->OutputToConsole("Cannot create a level", FALSE);
        return CKBR_OK;
    }

    CKSTRING name = (CKSTRING)beh->GetInputParameterReadDataPtr(1);

    CK_OBJECTCREATION_OPTIONS creaoptions = CK_OBJECTCREATION_NONAMECHECK;
    if (dynamic)
        creaoptions = CK_OBJECTCREATION_DYNAMIC;

    // The Creation
    CKObject *object = context.Context->CreateObject(classid, name, creaoptions);

    // we add it to the level
    if (CKIsChildClassOf(classid, CKCID_SCENE)) // a scene need a particular adding
    {
        context.CurrentLevel->AddScene((CKScene *)object);
    }
    else
    {
        context.CurrentLevel->AddObject(object);
    }

    // TODO : Current Scene ?

    beh->SetOutputParameterObject(0, object);

    return CKBR_OK;
}
