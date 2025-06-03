/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Remove Channel
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveChannelDecl();
CKERROR CreateRemoveChannelProto(CKBehaviorPrototype **);
int RemoveChannel(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveChannelDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Channel");
    od->SetDescription("Removes a channel material from the 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>channel .<BR>
    <SPAN CLASS=pin>Channel: </SPAN>channel to be removed from the 3D Entity.<BR>
    <BR>
    To choose which channel you want to remove, set one of the two input parameters: <BR>
    - If you don't know the channel number, set it to -1 and specify the corresponding material.<BR>
    - If you don't know which material correspond to your channel, specify a channel number greater than -1.<BR>
    <BR>
    */
    /* warning:
    - If the channel number is smaller than 0, and if the material is NULL, no channel will be removed.<BR>
    - If the channel number is greater than -1, and if the material is non-NULL, the preference will be given to the
    channel number as it is the fastest way to find a channel.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a462e58, 0x022e369b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveChannelProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Channel");
    return od;
}

CKERROR CreateRemoveChannelProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Channel");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RemoveChannel);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RemoveChannel(const CKBehaviorContext &behcontext)
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

    int channel = -1;
    beh->GetInputParameterValue(0, &channel);

    if (channel > -1)
    { // we specify a channel
        mesh->RemoveChannel(channel);
    }
    else
    {
        CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(1);
        mesh->RemoveChannelByMaterial(material);
    }

    return CKBR_OK;
}
