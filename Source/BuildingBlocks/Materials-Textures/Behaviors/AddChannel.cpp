/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Add Channel
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddChannelDecl();
CKERROR CreateAddChannelProto(CKBehaviorPrototype **);
int AddChannel(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddChannelDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Channel");
    od->SetDescription("Adds a Channel Material to the 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>material to be added to the 3D Entity.<BR>
    <SPAN CLASS=pin>Channel: </SPAN>channel number assigned to the added material<BR>
    <BR>
    The "Add Channel" BB adds a rendering layer to a whole mesh. If your 3D card supports multi-texture rendering in one single pass,
    the channel will be combined to the material to render the whole mesh in just one rendering pass.<BR>
    If the material you try to add to the mesh via the "Add Channel" BB is already one of the mesh channels, then it won't be added anymore,
    until you remove it.<BR>
    <BR>
    Load the composition "undocumented samples\ring.cmo" for an illustration of this behavior.<BR>
    <BR>
    */
    /* warning:
    - If the material you want to add to the mesh via the "Add Channel" BB is already one of the mesh's channels, then it won't be added.
    This is to prevent excessive channel addition.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x89658965, 0x36654789));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddChannelProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Channel");
    return od;
}

CKERROR CreateAddChannelProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Channel");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareOutParameter("Channel", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AddChannel);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AddChannel(const CKBehaviorContext &behcontext)
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

    CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(0);

    int channel = mesh->AddChannel(material);

    beh->SetOutputParameterValue(0, &channel);

    return CKBR_OK;
}
