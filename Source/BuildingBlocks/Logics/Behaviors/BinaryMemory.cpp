/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            BinaryMemory
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBinaryMemoryDecl();
CKERROR CreateBinaryMemoryProto(CKBehaviorPrototype **);
int BinaryMemory(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBinaryMemoryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Binary Memory");
    od->SetDescription("Memorizes in a 'State Memory' whether the behavior was triggered by 'In 0' or 'In 1'.");
    /* rem:
    <SPAN CLASS=in>In 0: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>In 1: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out 0: </SPAN>is activated when the process was triggered by 'In 0'.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated when the process was triggered by 'In 1'.<BR>
    <BR>
    <SPAN CLASS=pout>State: </SPAN>"True" if the building block is activated by 'In 0', "False" otherwise.
    <BR>
    Memorizes in a 'State Memory' whether the behavior was triggered by 'In 0' or 'In 1'.<BR>
    This building block can be used as a simplified 'Parameter Selector'.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xd02d67dd, 0x10211fdd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBinaryMemoryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateBinaryMemoryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Binary Memory");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareInput("In 1");
    proto->DeclareOutput("Out 0");
    proto->DeclareOutput("Out 1");

    proto->DeclareOutParameter("State", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BinaryMemory);

    *pproto = proto;
    return CK_OK;
}

int BinaryMemory(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = beh->GetOwner();

    CKBOOL k;
    if (beh->IsInputActive(0))
    {
        k = TRUE;
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }
    else
    {
        k = FALSE;
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
    }

    beh->SetOutputParameterValue(0, &k);
    return CKBR_OK;
}
