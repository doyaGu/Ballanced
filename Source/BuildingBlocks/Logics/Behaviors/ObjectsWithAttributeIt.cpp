/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					ObjectsWithAttribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorObjectsWithAttributeIteratorDecl();
CKERROR CreateObjectsWithAttributeIteratorProto(CKBehaviorPrototype **);
int ObjectsWithAttributeIterator(const CKBehaviorContext &behcontext);
CKERROR ObjectsWithAttributeIteratorCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorObjectsWithAttributeIteratorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Objects With Attribute Iterator");
    od->SetDescription("Retrieves each element of a group with a particular attribute");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the attribute type.<BR>
    <BR>
    <SPAN CLASS=pout>Object: </SPAN>the current parsed objects carrying the attribute.<BR>
    <SPAN CLASS=pout>(Attribute value): </SPAN>The attribute value (if any) for the returned object.<BR>
    <BR>
    This building block parse all the objects of the current scene carrying the specified attribute.<BR>
    <BR>
    See also: Set Attribute, Remove Attribute.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6bc1494c, 0xc816ad3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectsWithAttributeIteratorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Attribute");
    return od;
}

CKERROR CreateObjectsWithAttributeIteratorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Objects With Attribute Iterator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); // "Index"

    proto->DeclareOutParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareOutParameter("Attribute Value", CKPGUID_INT);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ObjectsWithAttributeIterator);
    proto->SetBehaviorCallbackFct(ObjectsWithAttributeIteratorCallBack);

    *pproto = proto;
    return CK_OK;
}

int ObjectsWithAttributeIterator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int Attribute = -1;
    beh->GetInputParameterValue(0, &Attribute);

    CKAttributeManager *attman = behcontext.AttributeManager;

    const XObjectPointerArray &group = attman->GetAttributeListPtr(Attribute);

    CK3dEntity *ent;

    int index = 0;
    beh->GetLocalParameterValue(0, &index);

    if (beh->IsInputActive(0))
    {
        index = 0;
        beh->ActivateInput(0, FALSE);
        if (index >= group.Size()) // no object in group
        {
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }
    else
    {
        beh->ActivateInput(1, FALSE);
        index++;
        if (index >= group.Size()) // no more object in group
        {
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }

    ent = (CK3dEntity *)group[index];
    beh->SetOutputParameterObject(0, ent);
    beh->SetLocalParameterValue(0, &index);
    CKParameterOut *pout = ent->GetAttributeParameter(Attribute);
    // we write the parameter if any
    if (pout)
    {
        CKParameterOut *behpout = beh->GetOutputParameter(1);
        if (behpout)
            behpout->CopyValue(pout);
    }

    beh->ActivateOutput(1);
    return CKBR_OK;
}

CKERROR ObjectsWithAttributeIteratorCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKAttributeManager *Manager = behcontext.AttributeManager;

        int Attribute = -1;
        beh->GetInputParameterValue(0, &Attribute);
        int ParamType = Manager->GetAttributeParameterType(Attribute);

        CKParameterOut *pout = beh->GetOutputParameter(1);
        if (ParamType == -1)
        {
            beh->EnableOutputParameter(1, FALSE);
        }
        else
        {
            beh->EnableOutputParameter(1, TRUE);
            pout->SetType(ParamType);
        }
    }
    break;
    }

    return CKBR_OK;
}
