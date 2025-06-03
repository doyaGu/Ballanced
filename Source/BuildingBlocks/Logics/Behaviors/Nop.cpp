/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Nop
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorNopDecl();
CKERROR CreateNopProto(CKBehaviorPrototype **);
int Nop(const CKBehaviorContext &behcontext);

CKERROR NopCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorNopDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Nop");
    od->SetDescription("Join several links and dispatch the stream to all the outputs.");
    /* rem:
    <SPAN CLASS=in>In K: </SPAN>any of the inputs will trigger the process.<BR>
    <SPAN CLASS=out>Out K: </SPAN>if the building block is activated, all the outputs are then activated.<BR>
    <BR>
    This convenient building block acts just like an interface object in the schematique.<BR>
    You can add as many inputs and outputs as needed.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x302561c4, 0xd282980));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateNopProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateNopProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Nop");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareOutput("Out 0");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_VARIABLEOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Nop);

    *pproto = proto;
    return CK_OK;
}

int Nop(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int i, count = beh->GetInputCount();
    for (i = 0; i < count; ++i)
    {
        beh->ActivateInput(i, FALSE);
    }
    count = beh->GetOutputCount();
    for (i = 0; i < count; ++i)
    {
        beh->ActivateOutput(i);
    }

    return CKBR_OK;
}