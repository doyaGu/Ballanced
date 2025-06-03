/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TextureScroller
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTextureScrollerDecl();
CKERROR CreateTextureScrollerProto(CKBehaviorPrototype **);
int TextureScroller(const CKBehaviorContext &behcontext);
CKERROR TextureScrollerCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextureScrollerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Texture Scroller");
    od->SetDescription("Scrolls the UV coords of a mesh's material (or one of its channel).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scroll Vector: </SPAN>translation 2D Vector.<BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the mapping is calculated only for the default material.
      Otherwise it is processed on the specified channel.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf11d010a, 0xfb1d010a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTextureScrollerProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Animation");
    return od;
}

CKERROR CreateTextureScrollerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Texture Scroller");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scroll Vector", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TextureScroller);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int TextureScroller(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // we get the Mesh
    CKMesh *mesh = (CKMesh *)beh->GetTarget();
    if (!CKIsChildClassOf(mesh, CKCID_MESH))
        return CKBR_OWNERERROR;

    // we get the vector
    VxUV vec;
    beh->GetInputParameterValue(0, &vec);

    int channel = -1;
    beh->GetInputParameterValue(1, &channel);

    CKDWORD Stride;
    VxUV *uvarray = (VxUV *)mesh->GetModifierUVs(&Stride, channel);
    if (!uvarray)
        return CKBR_OK;

    // Patch for progressive mesh : we have to modify all the vertices, else
    // we changing the level the scrolling is not coherent anymore
    int nbv = mesh->GetVertexCount();
    if (CKIsChildClassOf(mesh, CKCID_PATCHMESH))
        nbv = mesh->GetModifierUVCount(channel);

    // to avoid uvs going wild, we restrain them all the integer value of the first vertex
    vec.u -= (int)uvarray->u;
    vec.v -= (int)uvarray->v;

    for (int i = 0; i < nbv; i++, uvarray = (VxUV *)(((CKBYTE *)uvarray) + Stride))
    {
        *uvarray += vec;
    }

    mesh->ModifierUVMove();

    return CKBR_OK;
}
