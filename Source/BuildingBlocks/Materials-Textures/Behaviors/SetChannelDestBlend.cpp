/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetChannelDestBlend
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetChannelDestBlendDecl();
CKERROR CreateSetChannelDestBlendProto(CKBehaviorPrototype **);
int SetChannelDestBlend(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetChannelDestBlendDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Channel Dest Blend");
    od->SetDescription("Sets the dest mode when Blending the Material of a specific channel.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>the channel which material will be changed.<BR>
    <SPAN CLASS=pin>Blending Mode: </SPAN>dest mode when Blending.<BR>
    <BR>
    Load the composition "undocumented samples\ring.cmo" for an illustration of this behavior.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2f5a0eb9, 0x27067e93));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetChannelDestBlendProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Channel");
    return od;
}

CKERROR CreateSetChannelDestBlendProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Channel Dest Blend");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Blending Mode", CKPGUID_BLENDFACTOR, "Inverse Source Alpha");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetChannelDestBlend);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetChannelDestBlend(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKBeObject *beo = beh->GetTarget(); // check classid for back compatibility

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMesh *mesh = NULL;
    if (CKIsChildClassOf(beo, CKCID_MESH))
        mesh = (CKMesh *)beo;
    else if (CKIsChildClassOf(beo, CKCID_3DENTITY))
        mesh = ((CK3dEntity *)beo)->GetCurrentMesh();
    else
        return CK_OK;

    int channel = 0;
    beh->GetInputParameterValue(0, &channel);

    int dest_blend = VXBLEND_INVSRCALPHA;
    beh->GetInputParameterValue(1, &dest_blend);

    mesh->SetChannelDestBlend(channel, (VXBLEND_MODE)dest_blend);

    return CKBR_OK;
}
