/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Remove Attribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveAttributeDecl();
CKERROR CreateRemoveAttributeProto(CKBehaviorPrototype **);
int RemoveAttribute(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveAttributeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Attribute");
    od->SetDescription("Removes an attribute from the object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the attribute type to remove from the object.<BR>
    */
    od->SetCategory("Logics/Attribute");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6b6340c4, 0x61e94a41));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveAttributeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRemoveAttributeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Attribute");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RemoveAttribute);

    *pproto = proto;
    return CK_OK;
}

int RemoveAttribute(const CKBehaviorContext &behcontext)
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

    beo->RemoveAttribute(Attribute);

    return CKBR_OK;
}
