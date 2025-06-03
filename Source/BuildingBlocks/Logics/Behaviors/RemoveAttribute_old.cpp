/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Remove Attribute
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveAttribute_old_oldDecl();
CKERROR CreateRemoveAttribute_oldProto(CKBehaviorPrototype **);
int RemoveAttribute_old(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveAttribute_oldDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Attribute Obsolete");
    od->SetDescription("Removes an attribute from an object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>the object that should remove its attribute. It can be of any type. NULL means owner<BR>
    <SPAN CLASS=pin>Attribute: </SPAN>the attribute type to remove from the object.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x321d56c4, 0x6d56d5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveAttribute_oldProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Attribute");
    return od;
}

CKERROR CreateRemoveAttribute_oldProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Attribute Obsolete");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareInParameter("Attribute", CKPGUID_ATTRIBUTE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(RemoveAttribute_old);

    *pproto = proto;
    return CK_OK;
}

int RemoveAttribute_old(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *beo = (CKBeObject *)beh->GetInputParameterObject(0);
    if (!beo)
        beo = (CKBeObject *)beh->GetOwner();

    int Attribute = -1;
    beh->GetInputParameterValue(1, &Attribute);

    beo->RemoveAttribute(Attribute);

    return CKBR_OK;
}
