/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetChannelMaterial
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetChannelMaterialDecl();
CKERROR CreateSetChannelMaterialProto(CKBehaviorPrototype **);
int SetChannelMaterial(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetChannelMaterialDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Channel Material");
    od->SetDescription("Sets the Material of a Channel applied to the 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>the channel which material will be changed.<BR>
    <SPAN CLASS=pin>Material: </SPAN>the material that will be put in the channel.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e0a6b92, 0x366a3f9b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetChannelMaterialProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Channel");
    return od;
}

CKERROR CreateSetChannelMaterialProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Channel Material");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetChannelMaterial);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetChannelMaterial(const CKBehaviorContext &behcontext)
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

    CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(1);

    mesh->SetChannelMaterial(channel, material);

    return CKBR_OK;
}
