/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            BinarySwitch
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBinarySwitchDecl();
CKERROR CreateBinarySwitchProto(CKBehaviorPrototype **);
int BinarySwitch(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBinarySwitchDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Binary Switch");
    od->SetDescription("Activates the appropriate output, according to a Boolean condition.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated when Condition=True.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated when Condition=False.<BR>
    <BR>
    <SPAN CLASS=pin>Condition: </SPAN>boolean value.<BR>
    <BR>
    This building block acts just like the 'While' building block.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xeb506901, 0x984afccc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBinarySwitchProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateBinarySwitchProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Binary Switch");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Condition", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BinarySwitch);

    *pproto = proto;
    return CK_OK;
}

int BinarySwitch(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    CKBOOL k = TRUE;
    beh->GetInputParameterValue(0, &k);

    if (k)
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    return CKBR_OK;
}
