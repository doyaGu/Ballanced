/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Has Attribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHasAttribute_oldDecl();
CKERROR CreateHasAttribute_oldProto(CKBehaviorPrototype **);
int HasAttribute_old(const CKBehaviorContext &behcontext);
CKERROR HasAttribute_oldCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHasAttribute_oldDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Has Attribute Obsolete");
    od->SetDescription("Gets the parameter of an attribute (if it exists) on the given object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the object has the given attribute.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the object has not the given attribute.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>the object that should have or not the attribute. NULL means owner<BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the type of attribute to test.<BR>
    <BR>
    <SPAN CLASS=pout>Attribute Value : </SPAN>if the "True" output is activated, this output parameter contains the value of the attribute.<BR>
    */
    /* warning:
    - If the specified type "Attribute" has no parameter value then the output parameter "Attribute Value" is desabled.<BR>
    */
    /* warning:
    - This building block is now obsolete, and as been replaced with a TARGETABLE building block version.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x321e41c4, 0x6e41d5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHasAttribute_oldProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Attribute");
    return od;
}

CKERROR CreateHasAttribute_oldProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Has Attribute Obsolete");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->DeclareOutParameter("Attribute Value", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFunction(HasAttribute_old);
    proto->SetBehaviorCallbackFct(HasAttribute_oldCallBack);

    *pproto = proto;
    return CK_OK;
}

int HasAttribute_old(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Set IO states
    beh->ActivateInput(0, FALSE);

    CKBeObject *beo = (CKBeObject *)beh->GetInputParameterObject(0);
    if (!beo)
        beo = (CKBeObject *)beh->GetOwner();

    int Attribute = -1;
    beh->GetInputParameterValue(1, &Attribute);

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

CKERROR HasAttribute_oldCallBack(const CKBehaviorContext &behcontext)
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

        CKParameterOut *pout = beh->GetOutputParameter(0);
        if (ParamType == -1)
        {
            beh->EnableOutputParameter(0, FALSE);
        }
        else
        {
            beh->EnableOutputParameter(0, TRUE);
            pout->SetType(ParamType);
        }
    }
    }

    return CKBR_OK;
}
