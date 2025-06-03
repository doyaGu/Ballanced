/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            StreamingNOT
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorStreamingNOTDecl();
CKERROR CreateStreamingNOTProto(CKBehaviorPrototype **);
int StreamingNOT(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorStreamingNOTDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Stop & Go");
    od->SetDescription("Activates the output only when the input stops being active.");
    /* rem:
    <SPAN CLASS=in>In: <SPAN CLASS=out>triggers the process.<BR>
    <SPAN CLASS=out>Out: <SPAN CLASS=out>is only when the input stops being active.<BR>
    <BR>
    */
    /* warning:
    - This building block loops internally, therefore it is activated automatically each frame.
    Beware to choose a proper priority !<BR>
    eg: if you use a "Swicth On Key" building block to activate each frame the "Stop & Go", give the "Stop & Go" a smaller priority than the one you give to the "Switch On Key".<BR>
    This is because there's no specific order in which the building block are called if they must loop internally.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78a667f0, 0x5a3c0323));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateStreamingNOTProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateStreamingNOTProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Stop & Go");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(StreamingNOT);

    *pproto = proto;
    return CK_OK;
}

int StreamingNOT(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        return CKBR_ACTIVATENEXTFRAME;
    }
    else
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }
}
