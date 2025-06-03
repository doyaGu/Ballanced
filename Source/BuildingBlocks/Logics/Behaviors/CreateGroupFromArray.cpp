/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              CreateGroupFrom Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorCreateGroupFromArrayDecl();
CKERROR CreateCreateGroupFromArrayProto(CKBehaviorPrototype **);
int CreateGroupFromArray(const CKBehaviorContext &behcontext);
CKERROR CreateGroupFromArrayCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateGroupFromArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Create Group From Array");
    od->SetDescription("Create a group with entities elements matching the given value.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Created: </SPAN>is activated when matching elements have been found.<BR>
    <SPAN CLASS=out>No Elements Found: </SPAN>is activated when no matching elements have been found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>Entities Column: </SPAN>column in which the entities (BeObject) are taken.<BR>
    <SPAN CLASS=pin>Search Column: </SPAN>index of the column in which your search will be done.<BR>
    <SPAN CLASS=pin>Operator: </SPAN>comparison operator that will be used to compare the array value with the reference value.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>this output parameter will be created when choosing the column and editing the building block.
    The type of this parameter is choosen according to the type of the given column.<BR>
    <BR>
    <SPAN CLASS=pout>Group: </SPAN>created group, which contains all the entities matching the value comparison.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x22c71529, 0x6bb265d2));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateGroupFromArrayProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateCreateGroupFromArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Create Group From Array");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Created");
    proto->DeclareOutput("No Elements Found");

    proto->DeclareInParameter("Entities Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Search Column", CKPGUID_INT, "1");
    proto->DeclareInParameter("Operator", CKPGUID_COMPOPERATOR, "Equal");

    proto->DeclareOutParameter("Group", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(CreateGroupFromArray);
    proto->SetBehaviorCallbackFct(CreateGroupFromArrayCallBack);

    *pproto = proto;
    return CK_OK;
}

int CreateGroupFromArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
    {
        beh->ActivateOutput(1);
        return CKBR_OWNERERROR;
    }

    int ecolumn = 0;
    beh->GetInputParameterValue(0, &ecolumn);

    int scolumn = 0;
    beh->GetInputParameterValue(1, &scolumn);

    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(2, &op);

    CKDWORD key = NULL;

    // we find the key
    int size = GetInputValue(array, beh, scolumn, 3, key);

    CKGroup *group = (CKGroup *)beh->GetOutputParameterObject(0);
    if (!group)
    {
        beh->ActivateOutput(1);
        return CKBR_PARAMETERERROR;
    }

    // we fill the array with entities
    array->CreateGroup(scolumn, op, key, size, group, ecolumn);

    if (group->GetObjectCount() == 0)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR CreateGroupFromArrayCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORCREATE:
    {
        CKGroup *group = (CKGroup *)behcontext.Context->CreateObject(CKCID_GROUP, "ArrayGroup");
        group->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        beh->SetOutputParameterObject(0, group);
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        CKObject *obj = beh->GetOutputParameterObject(0);
        if (obj)
            behcontext.Context->DestroyObject(obj);
    }
    break;

    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIOREDITED:
    {
        CKDataArray *array = (CKDataArray *)beh->GetTarget();
        if (!array)
            return CKBR_OK;

        int scolumn = 0;
        beh->GetInputParameterValue(1, &scolumn);

        // we create the input parameter
        CreateInputParameter(array, beh, scolumn, 3);
    }
    break;
    }

    return CKBR_OK;
}
