/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Activate Channel
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorActivateChannelDecl();
CKERROR CreateActivateChannelProto(CKBehaviorPrototype **);
int ActivateChannel(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorActivateChannelDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Activate Channel");
    od->SetDescription("Activates or Deactivates a specific Channel of the Mesh.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>index of channel to be activated or deactivated.<BR>
    <SPAN CLASS=pin>Activate: </SPAN>if TRUE, the channel is activated (i.e. shown), if FALSE, the channel is deactivated (i.e. hidden).<BR>
    <BR>
    Load the composition "undocumented samples\ring.cmo" for an illustration of this.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x55566666, 0x66666444));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateActivateChannelProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Channel");
    return od;
}

CKERROR CreateActivateChannelProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Activate Channel");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Activate", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ActivateChannel);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ActivateChannel(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKBeObject *beo = beh->GetTarget();

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMesh *mesh = NULL;
    // check classid for back compatibility
    if (CKIsChildClassOf(beo, CKCID_MESH))
        mesh = (CKMesh *)beo;
    else if (CKIsChildClassOf(beo, CKCID_3DENTITY))
        mesh = ((CK3dEntity *)beo)->GetCurrentMesh();
    else
        return CK_OK;

    int index = 0;
    beh->GetInputParameterValue(0, &index);

    CKBOOL a = FALSE;
    beh->GetInputParameterValue(1, &a);

    mesh->ActivateChannel(index, a);

    return CKBR_OK;
}
