/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Set Attribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetAttribute_oldDecl();
CKERROR CreateSetAttribute_oldProto(CKBehaviorPrototype **);
int SetAttribute_old(const CKBehaviorContext &behcontext);
CKERROR SetAttribute_oldCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetAttribute_oldDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Attribute Obsolete");
    od->SetDescription("Gives an attribute to an object, and/or sets the value of this attribute.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>the object receiving the attribute. It can be of any type. NULL means owner<BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the name of the attribute.<BR>
    <SPAN CLASS=pin>Attribute Value: </SPAN>this input parameter is automatically construct after the first edition of the behavior, it should contains the new value of the attribute.<BR>
    <BR>
    This building block gives the Object an attribute (if the attribute wasn't assigned to it yet) and gives it a value.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x321f32c4, 0x6f32d5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetAttribute_oldProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Attribute");
    return od;
}

CKERROR CreateSetAttribute_oldProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Attribute Obsolete");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);

    proto->SetFunction(SetAttribute_old);
    proto->SetBehaviorCallbackFct(SetAttribute_oldCallBack);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int SetAttribute_old(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *beo = (CKBeObject *)beh->GetInputParameterObject(0);
    if (!beo)
        return CKBR_OK;

    int Attribute = -1;
    beh->GetInputParameterValue(1, &Attribute);
    beo->SetAttribute(Attribute);
    CKParameterOut *pout = beo->GetAttributeParameter(Attribute);

    if (pout) // If the attribute has a parameter attached
    {
        CKParameter *real;

        CKParameterIn *tpin = (CKParameterIn *)beh->GetInputParameter(2);
        if (tpin && (real = tpin->GetRealSource()))
            pout->CopyValue(real);
    }
    return CKBR_OK;
}

CKERROR SetAttribute_oldCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKAttributeManager *Manager = behcontext.AttributeManager;

        int Attribute = -1;
        beh->GetInputParameterValue(1, &Attribute);
        int ParamType = Manager->GetAttributeParameterType(Attribute);

        if (ParamType != -1) // we test if there is a parameter attached
        {
            beh->EnableInputParameter(2, TRUE);
            CKParameterIn *pin = beh->GetInputParameter(2);
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
            beh->EnableInputParameter(2, FALSE);
    }
    }

    return CKBR_OK;
}
