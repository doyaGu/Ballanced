/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					Group Operator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGroupOperatorDecl();
CKERROR CreateGroupOperatorProto(CKBehaviorPrototype **);
int GroupOperator(const CKBehaviorContext &behcontext);
CKERROR GroupCalculatorCallBack(const CKBehaviorContext &behcontext); // CallBack Function

CKObjectDeclaration *FillBehaviorGroupOperatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Group Operator");
    od->SetDescription("Makes combination of groups and objects wearing attributes.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Operator: </SPAN>the Set Operator.<BR>
    <SPAN CLASS=pin>Group or Attribute: </SPAN>the set you want to operate the current group with. An attribute
    describe the set of all the objects of the current scene wearing this attribute.<BR>
    You can change this input parameter to a Group.<BR>
    <BR>
    Available operations are UNION, INTERSECTION and SUBTRACTION.<BR>
    <BR>
    See also: Add To Group, Remove From Group.<BR>
    */
    /* Warning:
    - Remember, you can either change the type of the second input parameter to a GROUP or an ATTRIBUTE.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7b377625, 0x16da3bda));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGroupOperatorProto);
    od->SetCompatibleClassId(CKCID_GROUP);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateGroupOperatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Group Operator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Operator", CKPGUID_SETOPERATOR, "Union");
    proto->DeclareInParameter("Set", CKPGUID_ATTRIBUTE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(GroupOperator);
    proto->SetBehaviorCallbackFct(GroupCalculatorCallBack);

    *pproto = proto;
    return CK_OK;
}

int GroupOperator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKGroup *group = (CKGroup *)beh->GetTarget();
    if (!group)
        return CKBR_PARAMETERERROR;

    for (int i = 0; i < beh->GetInputParameterCount(); i += 2)
    {
        // we get the operator
        int op = 0;
        beh->GetInputParameterValue(i, &op);

        CKParameterIn *pin = beh->GetInputParameter(i + 1);
        if (!pin)
            continue;

        CKGUID pinguid = pin->GetGUID();
        if (pinguid == CKPGUID_GROUP) // we work on a group
        {
            CKGroup *group2 = (CKGroup *)beh->GetInputParameterObject(i + 1);
            switch (op)
            {
            case CKUNION:
            {
                int group2_count = group2->GetObjectCount();
                for (int j = 0; j < group2_count; ++j)
                {
                    group->AddObject(group2->GetObject(j));
                }
            }
            break;
            case CKINTERSECTION:
            {
                int group_count = group->GetObjectCount();
                for (int j = 0; j < group_count;)
                {
                    if (!(group->GetObject(j)->IsInGroup(group2)))
                    {
                        group->RemoveObject(j);
                        group_count--;
                    }
                    else
                        ++j;
                }
            }
            break;
            case CKSUBTRACTION:
            {
                int group_count = group->GetObjectCount();
                for (int j = 0; j < group_count;)
                {
                    if (group->GetObject(j)->IsInGroup(group2))
                    {
                        group->RemoveObject(j);
                        group_count--;
                    }
                    else
                        ++j;
                }
            }
            break;
            }
        }
        else
        {
            if (pinguid == CKPGUID_ATTRIBUTE) // we work on objects with attributes
            {
                int att = -1;
                beh->GetInputParameterValue(i + 1, &att);
                switch (op)
                {
                case CKUNION:
                {
                    CKAttributeManager *attman = behcontext.AttributeManager;
                    const XObjectPointerArray &array = attman->GetAttributeListPtr(att);
                    for (CKObject **o = array.Begin(); o != array.End(); ++o)
                    {
                        group->AddObject((CKBeObject *)*o);
                    }
                }
                break;
                case CKINTERSECTION:
                {
                    int group_count = group->GetObjectCount();
                    for (int j = 0; j < group_count;)
                    {
                        if (!(group->GetObject(j)->HasAttribute(att)))
                        {
                            group->RemoveObject(j);
                            group_count--;
                        }
                        else
                            ++j;
                    }
                }
                break;
                case CKSUBTRACTION:
                {
                    int group_count = group->GetObjectCount();
                    for (int j = 0; j < group_count;)
                    {
                        if (group->GetObject(j)->HasAttribute(att))
                        {
                            group->RemoveObject(j);
                            group_count--;
                        }
                        else
                            ++j;
                    }
                }
                break;
                }
            }
        }
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR GroupCalculatorCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        CKGUID tmp_guid;

        for (int i = 0; i < beh->GetInputParameterCount(); ++i)
        {
            pin = beh->GetInputParameter(i);
            tmp_guid = pin->GetGUID();
            if (i % 2) // Odd : must be a Set
            {
                if ((tmp_guid != CKPGUID_GROUP) && (tmp_guid != CKPGUID_ATTRIBUTE))
                    pin->SetGUID(CKPGUID_ATTRIBUTE);
            }
            else // Even : must be an operator
            {
                if (tmp_guid != CKPGUID_SETOPERATOR)
                {
                    pin->SetGUID(CKPGUID_SETOPERATOR);
                }
            }
        }
    }
    break;
    }

    return CKBR_OK;
}
