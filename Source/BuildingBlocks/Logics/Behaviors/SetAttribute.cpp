/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Set Attribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetAttributeDecl();
CKERROR CreateSetAttributeProto(CKBehaviorPrototype **);
int SetAttribute(const CKBehaviorContext &behcontext);
CKERROR SetAttributeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetAttributeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Attribute");
    od->SetDescription("Gives an attribute to the object, and/or sets the value of this attribute.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the name of the attribute.<BR>
    <SPAN CLASS=pin>Attribute Value: </SPAN>this input parameter is automatically construct after the first edition of the behavior, it should contains the new value of the attribute.<BR>
    <BR>
    This building block gives the Object an attribute (if the attribute wasn't assigned to it yet) and gives it a value.<BR>
    */
    od->SetCategory("Logics/Attribute");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x373040f2, 0x5e01b34));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetAttributeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetAttributeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Attribute");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetFunction(SetAttribute);
    proto->SetBehaviorCallbackFct(SetAttributeCallBack);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int SetAttribute(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OK;

    int Attribute = -1;
    beh->GetInputParameterValue(0, &Attribute);
    beo->SetAttribute(Attribute);
    CKParameterOut *pout = beo->GetAttributeParameter(Attribute);

    if (pout) // If the attribute has a parameter attached
    {
        CKParameter *real;

        CKParameterIn *tpin = (CKParameterIn *)beh->GetInputParameter(1);
        if (tpin && (real = tpin->GetRealSource()))
            pout->CopyValue(real);
    }
    return CKBR_OK;
}

CKERROR SetAttributeCallBack(const CKBehaviorContext &behcontext)
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

        if (ParamType != -1) // we test if there is a parameter attached
        {
            beh->EnableInputParameter(1, TRUE);
            CKParameterIn *pin = beh->GetInputParameter(1);
            if (pin)
            {
                if (pin->GetType() != ParamType)
                {
                    pin->SetType(ParamType);
                }
            }
            else
                beh->CreateInputParameter("Attribute Value", ParamType);
        }
        else
            beh->EnableInputParameter(1, FALSE);
    }
    }

    return CKBR_OK;
}
