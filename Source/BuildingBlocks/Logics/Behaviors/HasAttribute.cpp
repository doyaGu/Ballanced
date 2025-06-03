/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Has Attribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHasAttributeDecl();
CKERROR CreateHasAttributeProto(CKBehaviorPrototype **);
int HasAttribute(const CKBehaviorContext &behcontext);
CKERROR HasAttributeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHasAttributeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Has Attribute");
    od->SetDescription("Gets the parameter of an attribute (if it exists) on the object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the object has the given attribute.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the object has not the given attribute.<BR>
    <BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the type of attribute to test.<BR>
    <BR>
    <SPAN CLASS=pout>Attribute Value : </SPAN>if the "True" output is activated, this output parameter contains the value of the attribute.<BR>
    */
    /* warning:
    - If the specified type "Attribute" has no parameter value then the output parameter "Attribute Value" is desabled.<BR>
    */
    od->SetCategory("Logics/Attribute");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x25b54079, 0x6ff90545));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHasAttributeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateHasAttributeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Has Attribute");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->DeclareOutParameter("Attribute Value", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFunction(HasAttribute);
    proto->SetBehaviorCallbackFct(HasAttributeCallBack);

    *pproto = proto;
    return CK_OK;
}

int HasAttribute(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Set IO states
    beh->ActivateInput(0, FALSE);

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    int Attribute = -1;
    beh->GetInputParameterValue(0, &Attribute);

    if (beo->HasAttribute(Attribute))
    {
        beh->ActivateOutput(0);

        CKParameterOut *pout = beo->GetAttributeParameter(Attribute);

        CKParameterOut *tpout = (CKParameterOut *)beh->GetOutputParameter(0);
        if (tpout)
        {
            tpout->CopyValue(pout);
        }
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

CKERROR HasAttributeCallBack(const CKBehaviorContext &behcontext)
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

        CKParameterOut *pout = beh->GetOutputParameter(0);
        if (ParamType <= 0)
        {
            beh->EnableOutputParameter(0, FALSE);
        }
        else
        {
            beh->EnableOutputParameter(0, TRUE);
            pout->SetType(ParamType);
            pout->SetName(Manager->GetAttributeNameByType(Attribute));
        }
    }
    }

    return CKBR_OK;
}
