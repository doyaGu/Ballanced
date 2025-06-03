/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              CreateDecal
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCreateDecalDecl();
CKERROR CreateCreateDecalProto(CKBehaviorPrototype **);
int CreateDecal(const CKBehaviorContext &behcontext);
CKERROR CreateDecalCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateDecalDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Create Decal");
    od->SetDescription("Create a decal (object+mesh) on the targeted object.");
    /* rem:
     */
    od->SetCategory("Mesh Modifications/Creation");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x27b71ada, 0x22191538));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateDecalProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateCreateDecalProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Create Decal");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Frustum", CKPGUID_CAMERA);
    proto->DeclareInParameter("Decal Material", CKPGUID_MATERIAL);

    proto->DeclareInParameter("Offset", CKPGUID_FLOAT, "0.01");
    proto->DeclareInParameter("Backface Cull", CKPGUID_BOOL, "FALSE");
    //	proto->DeclareInParameter("Simplify",CKPGUID_BOOL,"FALSE"); // TODO
    proto->DeclareInParameter("Prelit", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Whole Faces", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Generate UVs", CKPGUID_BOOL, "FALSE");

    proto->DeclareSetting("Dynamic", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Decal", CKPGUID_3DENTITY);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CreateDecal);

    *pproto = proto;
    return CK_OK;
}

struct VxPolygon
{
    // Ctor
    VxPolygon() {}

    ////
    // Methods

    void AddVertex(const VxVector &iP)
    {
        vertices.PushBack(iP);
    }

    enum
    {
        OUTSIDE = 0,
        INSIDE = 1,
        CLIPPED = 2
    };

    CKBOOL Clip(const VxPlane &iPlane)
    {
        int vcount = vertices.Size();
        XArray<VxVector> newVertices(vcount);

        float dist0 = iPlane.Classify(vertices[0]);
        CKBOOL inside = (dist0 <= 0.0f);
        CKBOOL clipped = FALSE;

        float dist1 = dist0;
        int i;
        for (i = 1; i < vcount; ++i)
        { // n-1 first edges
            float dist2 = iPlane.Classify(vertices[i]);

            /////////////////////////////////////
            // Sutherland - Hodgeman clipping

            if (inside && (dist2 <= 0.0f))
            { // Both inside
                newVertices.PushBack(vertices[i]);
            }
            else if (!inside && (dist2 <= -EPSILON))
            { // Going In
                clipped = inside = TRUE;
                float d = dist1 / (dist1 - dist2);
                VxVector t(vertices[i - 1].x + (vertices[i].x - vertices[i - 1].x) * d,
                           vertices[i - 1].y + (vertices[i].y - vertices[i - 1].y) * d,
                           vertices[i - 1].z + (vertices[i].z - vertices[i - 1].z) * d);
                newVertices.PushBack(t);
                newVertices.PushBack(vertices[i]);
            }
            else if (inside && dist2 >= EPSILON)
            { // Going out
                clipped = TRUE;
                inside = FALSE;
                float d = dist1 / (dist1 - dist2);
                VxVector t(vertices[i - 1].x + (vertices[i].x - vertices[i - 1].x) * d,
                           vertices[i - 1].y + (vertices[i].y - vertices[i - 1].y) * d,
                           vertices[i - 1].z + (vertices[i].z - vertices[i - 1].z) * d);
                newVertices.PushBack(t);
            }
            else
                clipped = TRUE; // Both out

            dist1 = dist2;
        }

        // last edge
        if (inside && (dist0 <= 0.0f))
        { // Both inside
            newVertices.PushBack(vertices[0]);
        }
        else if (!inside && (dist0 <= -EPSILON))
        { // Going In
            clipped = inside = TRUE;
            float d = dist1 / (dist1 - dist0);
            VxVector t(vertices[i - 1].x + (vertices[0].x - vertices[i - 1].x) * d,
                       vertices[i - 1].y + (vertices[0].y - vertices[i - 1].y) * d,
                       vertices[i - 1].z + (vertices[0].z - vertices[i - 1].z) * d);
            newVertices.PushBack(t);
            newVertices.PushBack(vertices[0]);
        }
        else if (inside && dist0 >= EPSILON)
        { // Going out
            clipped = TRUE;
            inside = FALSE;
            float d = dist1 / (dist1 - dist0);
            VxVector t(vertices[i - 1].x + (vertices[0].x - vertices[i - 1].x) * d,
                       vertices[i - 1].y + (vertices[0].y - vertices[i - 1].y) * d,
                       vertices[i - 1].z + (vertices[0].z - vertices[i - 1].z) * d);
            newVertices.PushBack(t);
        }
        else
            clipped = TRUE; // Both out

        if (!clipped)
            return INSIDE; // All IN

        if (newVertices.Size() < 3)
            return OUTSIDE; // All Out

        // change the polygon to the clipped one
        vertices.Swap(newVertices);
        return CLIPPED;
    }

    CKBOOL Clip(const VxFrustum &iFrustum)
    {
        CKBOOL clip = FALSE;
        clip |= Clip(iFrustum.GetLeftPlane());
        clip |= Clip(iFrustum.GetRightPlane());
        clip |= Clip(iFrustum.GetFarPlane());
        clip |= Clip(iFrustum.GetBottomPlane());
        clip |= Clip(iFrustum.GetNearPlane());
        clip |= Clip(iFrustum.GetUpPlane());
        return clip;
    }

    ////
    // Members

    XArray<VxVector> vertices;
};

// XHashFun defined for a VxVector
template <>
struct XHashFun<VxVector>
{
    int operator()(const VxVector &v) const
    {
        return (int)((*(int *)&v.x ^ *(int *)&v.y) ^ *(int *)&v.z);
    }
};
typedef XHashFun<VxVector> XHashFunVector;

void Weld(CKMesh *iMesh)
{
    int vcount = iMesh->GetVertexCount();

    CKDWORD stride;
    void *ptr = iMesh->GetPositionsPtr(&stride);
    XPtrStrided<VxVector> pos(ptr, stride);

    XHashTable<int, VxVector, XHashFunVector> table(vcount);

    XArray<CKWORD> remap;
    remap.Resize(vcount);
    remap.Fill(-1);

    int i, index = 0;
    for (i = 0; i < vcount; ++i)
    {
        XHashTable<int, VxVector, XHashFunVector>::Pair p = table.TestInsert(pos[i], index);

        if (p.m_New)
        { // new vertex
            remap[i] = index++;
        }
        else
        {
            remap[i] = *(p.m_Iterator);
        }
    }

    // remap the vertices
    for (i = 0; i < vcount; ++i)
    {
        if (remap[i] != (CKWORD)-1)
        { // vertex remapped
            pos[(int)remap[i]] = pos[i];
        }
    }

    // remap the faces
    int fcount = iMesh->GetFaceCount();
    CKWORD *face = iMesh->GetFacesIndices();

    for (i = 0; i < fcount; ++i, face += 3)
    {
        *(face + 0) = remap[*(face + 0)];
        *(face + 1) = remap[*(face + 1)];
        *(face + 2) = remap[*(face + 2)];
    }

    iMesh->SetVertexCount(index);
}

int CreateDecal(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OK;

    CKMesh *mesh = (CKMesh *)ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    CKCamera *cam = (CKCamera *)beh->GetInputParameterObject(0);
    if (!cam)
        return CKBR_OK;

    CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(1);

    CKBOOL cull = FALSE;
    beh->GetInputParameterValue(3, &cull);

    CKBOOL prelit = TRUE;
    beh->GetInputParameterValue(4, &prelit);

    CKBOOL wholePolygon = FALSE;
    beh->GetInputParameterValue(5, &wholePolygon);

    CKBOOL generateUVs = FALSE;
    beh->GetInputParameterValue(6, &generateUVs);

    int width, height;
    VxVector pos, dir, up, right;
    cam->GetPosition(&pos);
    cam->GetOrientation(&dir, &up, &right);
    cam->GetAspectRatio(width, height);
    VxFrustum frustum(pos, right, up, dir, cam->GetFrontPlane(), cam->GetBackPlane(), cam->GetFov(), (float)height / width);

    // put the frustum in local for the entity
    frustum.Transform(ent->GetInverseWorldMatrix());

    ///////////////////////////////////////
    // Object Creation
    ///////////////////////////////////////

    // Dynamic ?
    CKBOOL dynamic = TRUE;
    beh->GetLocalParameterValue(0, &dynamic);

    CK_OBJECTCREATION_OPTIONS creaoptions = (dynamic) ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK;

    XString name = ent->GetName();
    name << "_Decal";

    CK3dEntity *decal = (CK3dEntity *)behcontext.Context->CreateObject(CKCID_3DOBJECT, name.Str(), creaoptions);

    name << "Mesh";
    CKMesh *decalmesh = (CKMesh *)behcontext.Context->CreateObject(CKCID_MESH, name.Str(), creaoptions);
    if (!decalmesh)
        return CKBR_OK;

    beh->SetOutputParameterObject(0, decal);

    // Add to the level
    CKLevel *level = behcontext.CurrentLevel;
    CKScene *scene = behcontext.CurrentScene;

    level->AddObject(decal);
    level->AddObject(decalmesh);

    // And to the current scene if any
    if (scene != level->GetLevelScene())
    {
        scene->AddObject(decal);
        scene->AddObject(decalmesh);
    }

    // 3DEntity
    decal->SetWorldMatrix(VxMatrix::Identity());
    decal->SetCurrentMesh(decalmesh);

    // Mesh
    if (prelit)
        decalmesh->SetLitMode(VX_PRELITMESH);

    int vcount = mesh->GetVertexCount();

    // First, flag the vertex to the frustum
    XAP<CKDWORD> vflags(new CKDWORD[vcount]);

    CKDWORD stride;
    void *ptr = mesh->GetPositionsPtr(&stride);
    XPtrStrided<VxVector> vpos(ptr, stride);

    for (int i = 0; i < vcount; ++i)
    {
        vflags[i] = frustum.Classify(vpos[i]);
    }

    int fcount = mesh->GetFaceCount();
    CKWORD *face = mesh->GetFacesIndices();

    XClassArray<VxPolygon> polygons;
    polygons.Reserve(fcount * 3);

    XArray<CKWORD> insideFaces;
    insideFaces.Reserve(fcount * 3);

    XArray<VxVector> insideVertices;
    insideVertices.Reserve(vcount);

    XArray<CKWORD> verticesSrc2Dst;
    verticesSrc2Dst.Resize(vcount);
    verticesSrc2Dst.Fill(-1);

    int verticesNumber = 0;
    int facesNumber = 0;

    VxPolygon p;
    p.vertices.Resize(9);

    for (int f = 0; f < fcount; ++f, face += 3)
    {
        CKDWORD af = vflags[*(face + 0)];
        CKDWORD bf = vflags[*(face + 1)];
        CKDWORD cf = vflags[*(face + 2)];

        if (cull)
        {
            const VxVector &normal = mesh->GetFaceNormal(f);
            if (DotProduct(frustum.GetDir(), normal) > 0.0f)
                continue;
        }

        if (!(af & bf & cf))
        { // Crossing or inside
            if (wholePolygon || (!af && !bf && !cf))
            { // Inside
                for (int i = 0; i < 3; ++i)
                {
                    int vertexIndex = *(face + i);
                    if (verticesSrc2Dst[vertexIndex] == (CKWORD)-1)
                    { // new vertex
                        verticesSrc2Dst[vertexIndex] = insideVertices.Size();

                        insideVertices.PushBack(vpos[vertexIndex]);
                    }
                    insideFaces.PushBack(verticesSrc2Dst[vertexIndex]);
                }
            }
            else
            { // Crossing

                p.vertices.Resize(3);
                p.vertices[0] = vpos[(int)*(face + 0)];
                p.vertices[1] = vpos[(int)*(face + 1)];
                p.vertices[2] = vpos[(int)*(face + 2)];

                CKDWORD ff = (af ^ bf) | (af ^ cf);

                CKBOOL clip = FALSE;
                for (;;)
                {
                    if (ff & VXCLIP_LEFT)
                    {
                        if (!p.Clip(frustum.GetLeftPlane()))
                            break;
                    }
                    if (ff & VXCLIP_RIGHT)
                    {
                        if (!p.Clip(frustum.GetRightPlane()))
                            break;
                    }
                    if (ff & VXCLIP_FRONT)
                    {
                        if (!p.Clip(frustum.GetNearPlane()))
                            break;
                    }
                    if (ff & VXCLIP_BACK)
                    {
                        if (!p.Clip(frustum.GetFarPlane()))
                            break;
                    }
                    if (ff & VXCLIP_BOTTOM)
                    {
                        if (!p.Clip(frustum.GetBottomPlane()))
                            break;
                    }
                    if (ff & VXCLIP_TOP)
                    {
                        if (!p.Clip(frustum.GetUpPlane()))
                            break;
                    }

                    clip = TRUE;
                    break;
                }
                if (clip)
                { // The polygon is completely or partly inside

                    verticesNumber += p.vertices.Size();
                    facesNumber += p.vertices.Size() - 2;

                    polygons.PushBack(p);
                }
            }
        }
    }

    // filling vertices
    {
        const VxMatrix &worldMatrix = ent->GetWorldMatrix();

        decalmesh->SetVertexCount(verticesNumber + insideVertices.Size());

        VxVector w;
        int index = 0;

        // Add the inide vertices
        for (VxVector *v = insideVertices.Begin(); v != insideVertices.End(); ++v)
        {
            Vx3DMultiplyMatrixVector(&w, worldMatrix, v);
            decalmesh->SetVertexPosition(index++, &w);
        }

        // then the crossing vertices
        for (VxPolygon *p = polygons.Begin(); p != polygons.End(); ++p)
        {
            for (VxVector *v = p->vertices.Begin(); v != p->vertices.End(); ++v)
            {
                Vx3DMultiplyMatrixVector(&w, worldMatrix, v);
                decalmesh->SetVertexPosition(index++, &w);
            }
        }
    }

    // filling faces indices
    {
        decalmesh->SetFaceCount(facesNumber + insideFaces.Size() / 3);

        CKWORD *fptr = decalmesh->GetFacesIndices();

        // Add the insides faces
        memcpy(fptr, insideFaces.Begin(), insideFaces.Size() * sizeof(CKWORD));
        fptr += insideFaces.Size();

        // then the crossing faces
        CKWORD baseIndex = insideVertices.Size();
        for (VxPolygon *p = polygons.Begin(); p != polygons.End(); ++p)
        {
            int faceCount = p->vertices.Size() - 2;
            for (int i = 0; i < faceCount; ++i)
            {
                *(fptr++) = baseIndex;
                *(fptr++) = baseIndex + i + 1;
                *(fptr++) = baseIndex + i + 2;
            }
            baseIndex += p->vertices.Size();
        }
    }

    // welding
    Weld(decalmesh);

    // UVs
    if (generateUVs && decalmesh->GetVertexCount())
    {
        VxMatrix projMatrix;
        cam->ComputeProjectionMatrix(projMatrix);

        VxMatrix totalMatrix;
        Vx3DMultiplyMatrix4(totalMatrix, projMatrix, cam->GetInverseWorldMatrix());

        int count = decalmesh->GetVertexCount();

        ptr = decalmesh->GetPositionsPtr(&stride);
        VxStridedData src(ptr, stride);
        XAP<VxVector4> v4(new VxVector4[count]);
        VxStridedData dst(v4, sizeof(VxVector4));

        Vx3DMultiplyMatrixVector4Strided(&dst, &src, totalMatrix, count);

        ptr = decalmesh->GetTextureCoordinatesPtr(&stride);
        XPtrStrided<VxUV> uvs(ptr, stride);
        float invw;
        for (int i = 0; i < count; ++i, ++uvs)
        {
            invw = 1.0f / v4[i].w;
            uvs->u = (1.0f + v4[i].x * invw) * 0.5f;
            uvs->v = (1.0f - v4[i].y * invw) * 0.5f;
        }
    }

    decalmesh->BuildNormals();

    // Offseting
    float offset = 0.01f;
    beh->GetInputParameterValue(2, &offset);

    if (offset != 0.0f)
    {
        ptr = decalmesh->GetPositionsPtr(&stride);
        XPtrStrided<VxVector> positions(ptr, stride);

        ptr = decalmesh->GetNormalsPtr(&stride);
        XPtrStrided<VxVector> normals(ptr, stride);

        int count = decalmesh->GetVertexCount();
        for (int i = 0; i < count; ++i)
        {
            positions[i] += normals[i] * offset;
        }
    }

    // Material
    decalmesh->ApplyGlobalMaterial(mat);

    return CKBR_OK;
}
