/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetUVs
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetUVsDecl();
CKERROR CreateSetUVsProto(CKBehaviorPrototype **);
int SetUVs(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetUVsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Copy Mapping");
    od->SetDescription("Copies the texture coordinates from a mesh to another mesh,");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Dest Channel: </SPAN>destination channel (-1 means the default material).<BR>
    <SPAN CLASS=pin>Src Mesh: </SPAN>source mesh (mesh from which to take the UVs coordinates.<BR>
    <SPAN CLASS=pin>Src Channel: </SPAN>source channel (-1 means the default material).<BR>
    <SPAN CLASS=pin>UV Tile: </SPAN>Multiply each copied UV coordinates by those factors.<BR>
    <BR>
    Copies the uv coordinates of a source mesh (or one of its channels) to the uv coordinates of another mesh (or one of its channels).<BR>
    */
    /* warning:
    - the two meshes MUST be identical. The only things that can be different are their mapping coordinates.<BR>
    */
    od->SetCategory("Materials-Textures/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x86a487c, 0x3cd44acb));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetUVsProto);
    od->SetCompatibleClassId(CKCID_MESH);
    return od;
}

CKERROR CreateSetUVsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Copy Mapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Dest Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Src Mesh", CKPGUID_MESH);
    proto->DeclareInParameter("Src Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("UV Tile", CKPGUID_2DVECTOR, "1,1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetUVs);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetUVs(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMesh *mesh = (CKMesh *)beh->GetTarget();
    if (!mesh)
        return CKBR_OWNERERROR;

    // get dest channel
    int destchannel = -1;
    beh->GetInputParameterValue(0, &destchannel);

    // mesh2
    CKMesh *mesh2 = (CKMesh *)beh->GetInputParameterObject(1);
    if (!mesh2)
        return CKBR_PARAMETERERROR;

    // get src channel
    int srcchannel = -1;
    beh->GetInputParameterValue(2, &srcchannel);

    // get factor
    Vx2DVector factor(1.0f, 1.0f);
    beh->GetInputParameterValue(3, &factor);

    CKDWORD uvStride, uv2Stride;
    Vx2DVector *uv = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride, destchannel);
    Vx2DVector *uv2 = (Vx2DVector *)mesh2->GetTextureCoordinatesPtr(&uv2Stride, srcchannel);

    int countsrc = mesh2->GetVertexCount();
    int count = mesh->GetVertexCount();

    if (countsrc != count && countsrc < count)
    {
        count = countsrc;
    }

    if (uv && uv2)
    {
        for (int a = 0; a < count; a++)
        {
            *uv = *uv2 * factor;
            uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
            uv2 = (Vx2DVector *)((CKBYTE *)uv2 + uv2Stride);
        }
    }

    //---
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    mesh->UVChanged();

    return CKBR_OK;
}
