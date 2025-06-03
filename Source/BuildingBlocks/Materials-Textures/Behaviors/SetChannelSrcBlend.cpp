/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetChannelSrcBlend
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetChannelSrcBlendDecl();
CKERROR CreateSetChannelSrcBlendProto(CKBehaviorPrototype **);
int SetChannelSrcBlend(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetChannelSrcBlendDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Channel Src Blend");
    od->SetDescription("Sets the Source mode when Blending the Material of a specific channel.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>the channel which material will be changed.<BR>
    <SPAN CLASS=pin>Blending Mode: </SPAN>Source mode when Blending.<BR>
    <BR>
    Load the composition "undocumented samples\ring.cmo" for an illustration of this behavior.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3b9b0e7f, 0xb1316800));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetChannelSrcBlendProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Channel");
    return od;
}

CKERROR CreateSetChannelSrcBlendProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Channel Src Blend");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Blending Mode", CKPGUID_BLENDFACTOR, "Source Alpha");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetChannelSrcBlend);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetChannelSrcBlend(const CKBehaviorContext &behcontext)
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

    int src_blend = VXBLEND_SRCALPHA;
    beh->GetInputParameterValue(1, &src_blend);

    mesh->SetChannelSourceBlend(channel, (VXBLEND_MODE)src_blend);

    return CKBR_OK;
}
