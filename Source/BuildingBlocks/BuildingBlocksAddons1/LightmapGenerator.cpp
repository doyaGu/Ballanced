#include "CKAll.h"

#include <assert.h>

#include "LightmapGenerator.h"
#include "PolyhedronConstructor.h"

// Gaussian Blur Matrix
int CMGaussianBlur2[3][3] = {2, 4, 2, 4, 0, 4, 2, 4, 2};

struct intcolor
{
    intcolor(){};
    intcolor(CKDWORD col)
    {
        r = ColorGetRed(col);
        g = ColorGetGreen(col);
        b = ColorGetBlue(col);
        a = ColorGetAlpha(col);
    }
    intcolor &operator+=(const intcolor &v)
    {
        r += v.r;
        g += v.g;
        b += v.b;
        a += v.a;
        return *this;
    }
    intcolor &operator*=(int v)
    {
        r *= v;
        g *= v;
        b *= v;
        a *= v;
        return *this;
    }
    intcolor &operator/=(int v)
    {
        r /= v;
        g /= v;
        b /= v;
        a /= v;
        return *this;
    }
    void Normalize()
    {
        if (r < 0)
            r = 0;
        else if (r > 255)
            r = 255;
        if (g < 0)
            g = 0;
        else if (g > 255)
            g = 255;
        if (b < 0)
            b = 0;
        else if (b > 255)
            b = 255;
        if (a < 0)
            a = 0;
        else if (a > 255)
            a = 255;
    }
    int r, g, b, a;
};

void ApplyMatrixToTextureInsides(CKTexture *tex, int matrix[3][3], int sum)
{
    if (!tex)
        return;

    if (!sum)
    { // The user wants us to calculate the sum
        for (int a = 0; a < 3; ++a)
            for (int b = 0; b < 3; ++b)
                sum += matrix[a][b];
    }
    if (!sum)
        sum = 1;

    int width = tex->GetWidth();
    int height = tex->GetHeight();
    CKDWORD *image = (CKDWORD *)tex->LockSurfacePtr();

    CKContext *ctx = tex->GetCKContext();
    VxMemoryPool mem(width * height * sizeof(CKDWORD));

    CKDWORD *newcolors = (CKDWORD *)mem.Buffer();
    CKDWORD *colptr = newcolors;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if ((i == 0) || (i == height - 1) || (j == 0) || (j == width - 1))
            {
                *colptr = image[j + (i)*width];
                colptr++;
                continue;
            }

            int im1 = i - 1;
            if (i == 0)
                im1 = 0;

            int ip1 = i + 1;
            if (i == height - 1)
                ip1 = height - 1;

            int jm1 = j - 1;
            if (j == 0)
                jm1 = 0;

            int jp1 = j + 1;
            if (j == width - 1)
                jp1 = width - 1;

            intcolor sumcol(0);
            intcolor oldcol;

            oldcol = intcolor(image[jm1 + (im1)*width]);
            oldcol *= matrix[0][0];
            sumcol += oldcol;

            oldcol = intcolor(image[jm1 + (i)*width]);
            oldcol *= matrix[0][1];
            sumcol += oldcol;

            oldcol = intcolor(image[jm1 + (ip1)*width]);
            oldcol *= matrix[0][2];
            sumcol += oldcol;

            oldcol = intcolor(image[j + (im1)*width]);
            oldcol *= matrix[1][0];
            sumcol += oldcol;

            oldcol = intcolor(image[j + (i)*width]);
            oldcol *= matrix[1][1];
            sumcol += oldcol;

            oldcol = intcolor(image[j + (ip1)*width]);
            oldcol *= matrix[1][2];
            sumcol += oldcol;

            oldcol = intcolor(image[jp1 + (im1)*width]);
            oldcol *= matrix[2][0];
            sumcol += oldcol;

            oldcol = intcolor(image[jp1 + (i)*width]);
            oldcol *= matrix[2][1];
            sumcol += oldcol;

            oldcol = intcolor(image[jp1 + (ip1)*width]);
            oldcol *= matrix[2][2];
            sumcol += oldcol;

            sumcol /= sum;

            *colptr = RGBAITOCOLOR(sumcol.r, sumcol.g, sumcol.b, sumcol.a);
            colptr++;
        }
    }

    memcpy(image, newcolors, width * height * 4);
}

LightmapGenerator::LightmapGenerator(CKContext *ctx)
{
    m_Context = ctx;
    m_ShowPolygons = FALSE;
}

LightmapGenerator::~LightmapGenerator()
{
    Clean();
}

#define LIGHTMAPSV1_IDENTIFIER 4510204

void LightmapGenerator::Save(CKStateChunk *chunk)
{
    chunk->StartWrite();
    // start identifier
    chunk->WriteIdentifier(LIGHTMAPSV1_IDENTIFIER);
    // size
    chunk->WriteInt(m_Objects.Size());
    // we write each object and its lightmap data
    for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
    {
        LightMapFaces *faces = *it;

        chunk->WriteObjectID(faces->object); // Object

        chunk->WriteInt(faces->textures.Size()); // Lightmap Number

        for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
        {
            LightMapTexture *text = *it2;

            chunk->WriteObject(text->texture); // Texture

            // Indices
            chunk->WriteInt(text->indices.Size());
            chunk->WriteBufferNoSize_LEndian16(text->indices.Size() * sizeof(CKWORD), text->indices.Begin());

            // Vertices
            chunk->WriteInt(text->vertices.Size());
            chunk->WriteBufferNoSize_LEndian(text->vertices.Size() * sizeof(LightVertex), text->vertices.Begin());
        }
    }
    // end identifier
    chunk->CloseChunk();
}

void LightmapGenerator::Load(CKStateChunk *chunk)
{
    Clean();

    // Start reading
    chunk->StartRead();

    if (chunk->SeekIdentifier(LIGHTMAPSV1_IDENTIFIER))
    {
        // Size
        m_Objects.Resize(chunk->ReadInt());

        // we read each object and its lightmap data
        for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
        {
            LightMapFaces *faces = new LightMapFaces;
            *it = faces;

            faces->object = chunk->ReadObjectID(); // Object

            faces->textures.Resize(chunk->ReadInt()); // Lightmap Number

            for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
            {
                LightMapTexture *text = new LightMapTexture;
                *it2 = text;

                text->texture = (CKTexture *)chunk->ReadObject(m_Context); // Texture

                // Indices
                text->indices.Resize(chunk->ReadInt());
                chunk->ReadAndFillBuffer_LEndian16(text->indices.Size() * sizeof(CKWORD), text->indices.Begin());

                // Vertices
                text->vertices.Resize(chunk->ReadInt());
                chunk->ReadAndFillBuffer_LEndian(text->vertices.Size() * sizeof(LightVertex), text->vertices.Begin());
            }
        }
    }
}

void LightmapGenerator::MarkTextureAsPrivate(CKBOOL priv)
{
    for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
    {
        LightMapFaces *faces = *it;
        for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
        {
            LightMapTexture *text = *it2;
            // we mark the texture
            if (priv)
                text->texture->ModifyObjectFlags(CK_OBJECT_NOTTOBESAVED, 0);
            else
                text->texture->ModifyObjectFlags(0, CK_OBJECT_NOTTOBESAVED);
        }
    }
}

void LightmapGenerator::Clean(CKBOOL iDestroyTextures)
{
    for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
    {
        LightMapFaces *faces = *it;
        for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
        {
            LightMapTexture *text = *it2;

            // we delete the texture
            if (iDestroyTextures)
                CKDestroyObject(text->texture, CK_DESTROY_FREEID);

            // we delete the face
            delete text;
        }
        // We delete the faces
        delete faces;
    }
    // we Clear the array
    m_Objects.Resize(0);

#ifdef DEBUGPOINT
    // we clear the debug points
    m_DebugPoints.Resize(0);
#endif
}

void LightmapGenerator::AttachRenderCallbacks()
{
    for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
    {
        LightMapFaces *faces = *it;

        CK3dEntity *ent = (CK3dEntity *)m_Context->GetObject(faces->object);
        if (ent)
            ent->AddPostRenderCallBack(LightmapGenerator::LightMapsRender, this, TRUE);
    }

#ifdef DEBUGPOINT
    CKRenderContext *rc = m_Context->GetPlayerRenderContext();
    if (rc)
        rc->AddPostRenderCallBack(LightmapGenerator::LightMapsRenderDebug, this, TRUE);
#endif
}

void LightmapGenerator::RemoveRenderCallbacks()
{
    for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
    {
        LightMapFaces *faces = *it;

        CK3dEntity *ent = (CK3dEntity *)m_Context->GetObject(faces->object);
        if (ent)
            ent->RemovePostRenderCallBack(LightmapGenerator::LightMapsRender, this);
    }

#ifdef DEBUGPOINT
    CKRenderContext *rc = m_Context->GetPlayerRenderContext();
    if (rc)
        rc->RemovePostRenderCallBack(LightmapGenerator::LightMapsRenderDebug, this);
#endif
}

void LightmapGenerator::SetReceivers(CKGroup *g, int Start, int Count)
{
    m_Receivers.Resize(0);
    if (g)
    {
        if (Count < 0)
            Count = g->GetObjectCount();
        if (Count > g->GetObjectCount())
            Count = g->GetObjectCount();

        //		m_Receivers.Resize(Count);
        int i = Start;
        while (Count--)
        {
            m_Receivers.PushBack(g->GetObject(i));
            i++;
        }
        //		for (XObjectPointerArray::Iterator it=m_Receivers.Begin(); it!=m_Receivers.End() && Count>0; ++it,++i,--Count) {
        //			*it = g->GetObject(i + Start);
        //		}
    }
    else
    {
        int _3DObjectsCount = m_Context->GetObjectsCountByClassID(CKCID_3DOBJECT);
        CK_ID *id = m_Context->GetObjectsListByClassID(CKCID_3DOBJECT);
        if (Count < 0)
            Count = _3DObjectsCount;
        if (Count > _3DObjectsCount)
            Count = _3DObjectsCount;

        int i = Start;
        while (Count--)
        {
            m_Receivers.PushBack((CK3dObject *)m_Context->GetObject(id[i]));
            i++;
        }
        //		m_Receivers.Resize(count);
        //		int i=0;
        //		for (XObjectPointerArray::Iterator it=m_Receivers.Begin(); it!=m_Receivers.End(); ++it,++i) {
        //			*it = (CK3dObject*)m_Context->GetObject( id[i] );
        //		}
    }
}

void LightmapGenerator::SetOccluders(CKGroup *g)
{
    m_Occluders.Resize(0);
    if (g)
    {
        m_Occluders.Resize(g->GetObjectCount());
        int i = 0;
        for (XObjectPointerArray::Iterator it = m_Occluders.Begin(); it != m_Occluders.End(); ++it, ++i)
        {
            *it = g->GetObject(i);
        }
    }
    else
    {
        int count = m_Context->GetObjectsCountByClassID(CKCID_3DOBJECT);
        CK_ID *id = m_Context->GetObjectsListByClassID(CKCID_3DOBJECT);

        m_Occluders.Resize(count);
        int i = 0;
        for (XObjectPointerArray::Iterator it = m_Occluders.Begin(); it != m_Occluders.End(); ++it, ++i)
        {
            *it = (CK3dObject *)m_Context->GetObject(id[i]);
        }
    }
}

void LightmapGenerator::SetLights(CKGroup *g)
{
    m_Lights.Resize(0);
    if (g)
    {
        int count = g->GetObjectCount();
        m_Lights.Reserve(count);
        for (int i = 0; i < count; ++i)
        {
            CKLight *light = (CKLight *)g->GetObject(i);
            if (CKIsChildClassOf(light, CKCID_LIGHT))
            {
                m_Lights.PushBack(light);
            }
        }
    }
    else
    {
        int tcount = m_Context->GetObjectsCountByClassID(CKCID_TARGETLIGHT);
        CK_ID *tid = m_Context->GetObjectsListByClassID(CKCID_TARGETLIGHT);
        int count = m_Context->GetObjectsCountByClassID(CKCID_LIGHT);
        CK_ID *id = m_Context->GetObjectsListByClassID(CKCID_LIGHT);

        m_Lights.Reserve(count + tcount);
        int i;
        for (i = 0; i < count; ++i)
        {
            CKLight *light = (CKLight *)m_Context->GetObject(id[i]);
            assert(CKIsChildClassOf(light, CKCID_LIGHT));
            m_Lights.PushBack(light);
        }
        for (i = 0; i < tcount; ++i)
        {
            CKTargetLight *light = (CKTargetLight *)m_Context->GetObject(tid[i]);
            assert(CKIsChildClassOf(light, CKCID_TARGETLIGHT));
            m_Lights.PushBack(light);
        }
    }
}

void LightmapGenerator::Generate(float Density, float Threshold, int SuperSampling, int Blur)
{
#ifndef USEMT
    // First of all, we delete the previous lightmaps
    Clean();
#endif

    m_SuperSampling = SuperSampling;
    m_InverseSampling = 1.0f / SuperSampling;

    float texelWorldSize = 1.0f / Density;

    m_RayBoxPerformed = 0;
    m_RayIntPerformed = 0;

    // We fill the octree
    /*
    Octree::m_MaxPrimitiveNumber = 1;
    Octree::m_MinHalfSize = 0.5f;
    m_Octree.CreateOctree(m_Context,m_Occluders);
    */

    CKRenderContext *rc = m_Context->GetPlayerRenderContext();
    m_AmbientColor.Set(rc->GetAmbientLight());
    m_AmbientColor.a = 1.0f;

    char buffer[256];

    ///
    // Objects loop
    for (XObjectPointerArray::Iterator it = m_Receivers.Begin(); it != m_Receivers.End(); ++it)
    {
        m_CurrentEntity = (CK3dObject *)*it;
        if (!m_CurrentEntity)
            continue;

        m_CurrentMesh = m_CurrentEntity->GetCurrentMesh();
        if (!m_CurrentMesh)
            continue;

        VxVector objectScale;
        m_CurrentEntity->GetScale(&objectScale, FALSE);

        const VxMatrix &worldMatrix = m_CurrentEntity->GetWorldMatrix();

        m_PositionsPtr = (CKBYTE *)m_CurrentMesh->GetPositionsPtr(&m_PositionStride);
        m_NormalsPtr = (CKBYTE *)m_CurrentMesh->GetNormalsPtr(&m_NormalStride);

        VxVector va, vb, vc, facenormal;

        int facescount = m_CurrentMesh->GetFaceCount();
        if (!facescount)
            continue;

        LightMapFaces *objectmap = new LightMapFaces;
        m_Objects.PushBack(objectmap);
        objectmap->object = CKOBJID(m_CurrentEntity);

        // PolyhedronConstructor pc(m_CurrentMesh);
        // m_PolygonConstructor = &pc;
        m_PolygonConstructor = new PolyhedronConstructor(m_CurrentMesh);

        try
        {
            m_PolygonConstructor->Simplify(Threshold);
        }
        catch(...)
        {
            m_Context->OutputToConsoleEx("Lightmap does not support object : %s", m_CurrentEntity->GetName());
            continue;
        }

        int pcount = m_PolygonConstructor->GetPolyhedronCount();
        for (int p = 0; p < pcount; ++p)
        {
            m_PolygonIndex = p;

            if (!m_PolygonConstructor->IsPolyhedronValid(p))
                continue;

            VxBbox box = m_PolygonConstructor->GetPolyhedronBox(p);
            CKBOOL smooth = m_PolygonConstructor->GetPolyhedronSmooth(p);

            int minindex = 0;
            VxVector size = 2.0f * objectScale * box.GetSize();
            float min = size.x;
            int x = 1;
            int y = 2;

            if (min > size.y)
            {
                min = size.y;
                minindex = 1;
                x = 0;
                y = 2;
            }
            if (min > size.z)
            {
                minindex = 2;
                x = 0;
                y = 1;
            }

            // we want all the textures larger than taller
            if (size.v[y] > size.v[x])
            {
                XSwap(x, y);
            }

            m_MajorX = x;
            m_MajorY = y;

            int twidth = Near2Power(int(size.v[x] * Density));
            if (twidth < 4)
                twidth = 4;
            int theight = Near2Power(int(size.v[y] * Density));
            if (theight < 4)
                theight = 4;
            float invtwidth = 1.0f / twidth;
            float invtheight = 1.0f / theight;

            // We create the light map
            sprintf(buffer, "%s - face %d", m_CurrentEntity->GetName(), p);
            CKTexture *tex = (CKTexture *)m_Context->CreateObject(CKCID_TEXTURE, buffer);

            // We create the light map face with vertex in the world coordinates
            LightMapTexture *lightface = new LightMapTexture;
            lightface->texture = tex;

            // We add the ligth map face to the array
            objectmap->textures.PushBack(lightface);

            const XArray<int> &vindices = m_PolygonConstructor->GetPolyhedronVertexIndices(p);

            // we need to fill the vertices
            int vcount = vindices.Size();
            lightface->vertices.Resize(vcount);

            int j;
            for (j = 0; j < vcount; ++j)
            {
                // Old Index
                lightface->vertices[j].index = vindices[j];
                // Position
                m_CurrentMesh->GetVertexPosition(vindices[j], &lightface->vertices[j].position);
                // Colors
                lightface->vertices[j].diffuse = 0xffffffff;
                lightface->vertices[j].specular = 0x00000000;
                // Uvs
                const VxVector &v = lightface->vertices[j].position;
                lightface->vertices[j].uv.u = (v.v[x] - box.Min.v[x]) / box.GetSize().v[x];
                lightface->vertices[j].uv.v = (v.v[y] - box.Min.v[y]) / box.GetSize().v[y];

                // rescaling in the center of a texture, with 1 texel border
                lightface->vertices[j].uv.u = invtwidth + (1.0f - 2.0f * invtwidth) * lightface->vertices[j].uv.u;
                lightface->vertices[j].uv.v = invtheight + (1.0f - 2.0f * invtheight) * lightface->vertices[j].uv.v;
            }

            const XArray<int> &findices = m_PolygonConstructor->GetPolyhedronFaceIndices(p);

            // we need to fill the indices
            int fcount = findices.Size();
            lightface->indices.Resize(fcount * 3);
            lightface->originalface.Resize(fcount);
            int a, b, c;
            for (j = 0; j < fcount; ++j)
            {
                // m_CurrentMesh->GetFaceVertexIndex(findices[j],a,b,c);
                // new modif
                m_PolygonConstructor->GetFaceVertexIndex(findices[j], a, b, c);

                // original face
                lightface->originalface[j] = findices[j];

                // Indices
                lightface->indices[j * 3] = vindices.GetPosition(a);
                XASSERT(lightface->indices[j * 3] != 0xffff);
                lightface->indices[j * 3 + 1] = vindices.GetPosition(b);
                XASSERT(lightface->indices[j * 3 + 1] != 0xffff);
                lightface->indices[j * 3 + 2] = vindices.GetPosition(c);
                XASSERT(lightface->indices[j * 3 + 2] != 0xffff);
            }

            ///
            // We create the polygon plane
            VxVector pts[3];
            m_CurrentEntity->Transform(&pts[0], &lightface->vertices[lightface->indices[0]].position);
            m_CurrentEntity->Transform(&pts[1], &lightface->vertices[lightface->indices[1]].position);
            m_CurrentEntity->Transform(&pts[2], &lightface->vertices[lightface->indices[2]].position);

            // VxVector normal = m_PolygonConstructor->GetPolyhedronNormal(p);
            // m_PolygonPlane.Create(normal, pts[0]);
            m_PolygonPlane.Create(pts[0], pts[1], pts[2]);

            // We Calculate the extrema of the box in the world
            m_BoxWorldVertices[0] = box.Max;
            m_BoxWorldVertices[0].v[x] = box.Min.v[x];
            m_BoxWorldVertices[0].v[y] = box.Min.v[y];
            m_BoxWorldVertices[1] = box.Max;
            m_BoxWorldVertices[1].v[x] = box.Min.v[x];
            m_BoxWorldVertices[2] = box.Max;
            m_BoxWorldVertices[3] = box.Max;
            m_BoxWorldVertices[3].v[y] = box.Min.v[y];
            Vx3DMultiplyMatrixVectorMany(m_BoxWorldVertices, worldMatrix, m_BoxWorldVertices, 4, sizeof(VxVector));

            ///
            // We compute the intersections of the box vertices with the plane supporting the face

            // We then project these vertices on the polygon plane
            float dummy;
            VxVector boxdir = Normalize(CrossProduct(m_BoxWorldVertices[3] - m_BoxWorldVertices[0], m_BoxWorldVertices[1] - m_BoxWorldVertices[0]));
            VxIntersect::LinePlane(VxRay(m_BoxWorldVertices[0], boxdir, NULL), m_PolygonPlane, m_PolygonWorldVertices[0], dummy);
            VxIntersect::LinePlane(VxRay(m_BoxWorldVertices[1], boxdir, NULL), m_PolygonPlane, m_PolygonWorldVertices[1], dummy);
            VxIntersect::LinePlane(VxRay(m_BoxWorldVertices[2], boxdir, NULL), m_PolygonPlane, m_PolygonWorldVertices[2], dummy);
            VxIntersect::LinePlane(VxRay(m_BoxWorldVertices[3], boxdir, NULL), m_PolygonPlane, m_PolygonWorldVertices[3], dummy);

            // We filter potential lights
            if (!smooth)
            { // we filter only if we are somewhat planar

                m_FilteredLights.Resize(0);
                for (XObjectPointerArray::Iterator lightit = m_Lights.Begin(); lightit != m_Lights.End(); ++lightit)
                {
                    CKLight *light = (CKLight *)*lightit;
                    const VxVector &lightpos = light->GetWorldMatrix()[3];

                    // discard light on the back side of the polygon
                    float dp = m_PolygonPlane.Classify(lightpos);
                    if (dp < 0.0f)
                    {
                        continue;
                    }

                    // we now test the distance of the light for omnis and spot
                    if (light->GetType() != VX_LIGHTDIREC)
                    {

                        float lightRange = light->GetRange();

                        // discard light if distance to the plane greater than range
                        if (lightRange < dp)
                            continue;

                        // we square the lightrange
                        lightRange *= lightRange;

                        CKBOOL inRange = FALSE;

                        VxRay edge(m_PolygonWorldVertices[0], m_PolygonWorldVertices[1]);
                        edge.m_Direction.Normalize();
                        if (edge.SquareDistance(lightpos) < lightRange)
                            inRange = TRUE;
                        else
                        {
                            edge = VxRay(m_PolygonWorldVertices[1], m_PolygonWorldVertices[2]);
                            edge.m_Direction.Normalize();
                            if (edge.SquareDistance(lightpos) < lightRange)
                                inRange = TRUE;
                            else
                            {
                                edge = VxRay(m_PolygonWorldVertices[2], m_PolygonWorldVertices[3]);
                                edge.m_Direction.Normalize();
                                if (edge.SquareDistance(lightpos) < lightRange)
                                    inRange = TRUE;
                                else
                                {
                                    edge = VxRay(m_PolygonWorldVertices[3], m_PolygonWorldVertices[0]);
                                    edge.m_Direction.Normalize();
                                    if (edge.SquareDistance(lightpos) < lightRange)
                                        inRange = TRUE;
                                }
                            }
                        }

                        if (!inRange)
                        {
                            continue;
                        }
                    }

                    m_FilteredLights.PushBack(light);
                }
            }
            else
            {
                m_FilteredLights = m_Lights;
            }

            // we filter the potential occluders to keep only the usefull ones
            {
                CKBOOL swingPlanes = FALSE;
                if (DotProduct(m_PolygonPlane.m_Normal, boxdir) > 0.0f)
                    swingPlanes = TRUE; // the planes are in the wrong sens

                for (int lightindex = 0; lightindex < m_FilteredLights.Size(); ++lightindex)
                {
                    CKLight *light = (CKLight *)m_FilteredLights[lightindex];
                    const VxVector &lightpos = light->GetWorldMatrix()[3];

                    // We create 4 planes around our polygon, passing by the light

                    VxPlane left(m_PolygonWorldVertices[0], m_PolygonWorldVertices[1], lightpos);
                    VxPlane right(m_PolygonWorldVertices[1], m_PolygonWorldVertices[2], lightpos);
                    VxPlane top(m_PolygonWorldVertices[2], m_PolygonWorldVertices[3], lightpos);
                    VxPlane down(m_PolygonWorldVertices[3], m_PolygonWorldVertices[0], lightpos);

                    if (swingPlanes)
                    {
                        left = -left;
                        right = -right;
                        top = -top;
                        down = -down;
                    }

                    m_FilteredOccluders[lightindex].Resize(0);

                    for (XObjectPointerArray::Iterator occit = m_Occluders.Begin(); occit != m_Occluders.End(); ++occit)
                    {
                        CK3dObject *currentEntity = (CK3dObject *)*occit;
                        if (!currentEntity)
                            continue;

                        CKMesh *currentMesh = currentEntity->GetCurrentMesh();
                        if (!currentMesh)
                            continue;

                        // Planes filtering
                        if (m_PolygonPlane.Classify(currentMesh->GetLocalBox(), currentEntity->GetWorldMatrix()) < 0.0f)
                            continue;
                        if (left.Classify(currentMesh->GetLocalBox(), currentEntity->GetWorldMatrix()) > 0.0f)
                            continue;
                        if (right.Classify(currentMesh->GetLocalBox(), currentEntity->GetWorldMatrix()) > 0.0f)
                            continue;
                        if (top.Classify(currentMesh->GetLocalBox(), currentEntity->GetWorldMatrix()) > 0.0f)
                            continue;
                        if (down.Classify(currentMesh->GetLocalBox(), currentEntity->GetWorldMatrix()) > 0.0f)
                            continue;

                        // the object is to be considered as occluder for this light
                        m_FilteredOccluders[lightindex].PushBack(currentEntity);
                    }
                }
            }

            // Now the calcul of light interaction

            if (m_FilteredLights.Size())
            { // The lightmap must be generated

                // We create the texture
                tex->Create(twidth, theight);
                CKDWORD *startmapx = (CKDWORD *)tex->LockSurfacePtr();

                CKDWORD purple = RGBAFTOCOLOR(1.0f, 0.0f, 1.0f, 0.5f);

                VxFillStructure(twidth * theight, startmapx, sizeof(CKDWORD), sizeof(CKDWORD), &purple);

                if (smooth)
                {
                    ComputeLightmapSmooth(startmapx, twidth, theight);
                }
                else
                {
                    ComputeLightmapFlat(startmapx, twidth, theight);
                }

                tex->ReleaseSurfacePtr();
                // tex->SetDesiredVideoFormat(_32_RGB888);

                // We bleed the border of the texture
                // if (Blur) DuplicateBorderTexture(tex);

                if (!Blur)
                {
                    // We bleed the border of the texture
                    ExtendBorderTexture(tex);
                }

                ///
                //	ring
                for (int i = 0; i < Blur; ++i)
                {
                    // We bleed the border of the texture
                    ExtendBorderTexture(tex);

                    ApplyMatrixToTextureInsides(tex, CMGaussianBlur2, 0);
                }
            }
            else
            { // lightmap is fully ambientcolor

                // we resize the texture to be 2x2
                tex->Create(2, 2);

                for (j = 0; j < vcount; ++j)
                {
                    // rescaling in the center of a texture, with 1 texel border
                    lightface->vertices[j].uv.u = 0.5f;
                    lightface->vertices[j].uv.v = 0.5f;
                }
                CKDWORD col = RGBAFTOCOLOR(&m_AmbientColor);
                CKBYTE *surf = tex->LockSurfacePtr();
                VxFillStructure(4, surf, sizeof(CKDWORD), sizeof(CKDWORD), &col);
                tex->ReleaseSurfacePtr();
                // tex->SetDesiredVideoFormat(_32_RGB888);
            }

        } // For polygons

        ///
        // We now will try to regroup the textures in one
#ifndef USEMT
        PackTextures(*objectmap);
#endif
        delete m_PolygonConstructor;
        m_PolygonConstructor = NULL;
    } // For objects

#ifdef _DEBUG
    // m_Context->OutputToConsoleEx("Test Performed %d/%d",m_RayIntPerformed,m_RayBoxPerformed);
#endif
}

int LightmapGenerator::TextureSort(const void *elem1, const void *elem2)
{
    LightMapTexture *lt1 = *(LightMapTexture **)elem1;
    LightMapTexture *lt2 = *(LightMapTexture **)elem2;

    CKTexture *tex1 = lt1->texture;
    CKTexture *tex2 = lt2->texture;

    return ((tex2->GetWidth() << 8) + tex2->GetHeight()) - ((tex1->GetWidth() << 8) + tex1->GetHeight());
}

void LightmapGenerator::DuplicateBorderTexture(CKTexture *tex)
{
    CKDWORD *texImage = (CKDWORD *)tex->LockSurfacePtr();
    int texWidth = tex->GetWidth();
    int texHeight = tex->GetHeight();

    // we first make the vertical borders
    int i;
    for (i = 1; i < texHeight - 1; ++i)
    {
        *(texImage + i * texWidth) = *(texImage + i * texWidth + 1);                           // Left Border
        *(texImage + i * texWidth + texWidth - 1) = *(texImage + i * texWidth + texWidth - 2); // Right Border
    }

    // the we copy the first line
    memcpy(texImage, texImage + texWidth, texWidth * sizeof(CKDWORD));

    // and the last line
    memcpy(texImage + texWidth * i, texImage + texWidth * (i - 1), texWidth * sizeof(CKDWORD));
}

void LightmapGenerator::ExtendBorderTexture(CKTexture *tex)
{
    CKDWORD *texImage = (CKDWORD *)tex->LockSurfacePtr();
    int texWidth = tex->GetWidth();
    int texHeight = tex->GetHeight();

    CKDWORD colA, colB;
    int r, g, b;

    // we first make the vertical borders
    if (texWidth > 2)
    {
        for (int i = 1; i < texHeight - 1; ++i)
        {
            // Left Border

            colA = *(texImage + i * texWidth + 1);
            colB = *(texImage + i * texWidth + 2);

            r = 2 * ColorGetRed(colA) - ColorGetRed(colB);
            if (r < 0)
                r = 0;
            else if (r > 255)
                r = 255;

            g = 2 * ColorGetGreen(colA) - ColorGetGreen(colB);
            if (g < 0)
                g = 0;
            else if (g > 255)
                g = 255;

            b = 2 * ColorGetBlue(colA) - ColorGetBlue(colB);
            if (b < 0)
                b = 0;
            else if (b > 255)
                b = 255;

            *(texImage + i * texWidth) = RGBAITOCOLOR(r, g, b, 255);

            // Right Border
            colA = *(texImage + i * texWidth + texWidth - 2);
            colB = *(texImage + i * texWidth + texWidth - 3);
            r = 2 * ColorGetRed(colA) - ColorGetRed(colB);
            if (r < 0)
                r = 0;
            else if (r > 255)
                r = 255;

            g = 2 * ColorGetGreen(colA) - ColorGetGreen(colB);
            if (g < 0)
                g = 0;
            else if (g > 255)
                g = 255;

            b = 2 * ColorGetBlue(colA) - ColorGetBlue(colB);
            if (b < 0)
                b = 0;
            else if (b > 255)
                b = 255;

            *(texImage + i * texWidth + texWidth - 1) = RGBAITOCOLOR(r, g, b, 255);
        }
    }

    // we then proceed the horizontal borders
    if (texHeight > 2)
    {
        for (int i = 0; i < texWidth; ++i)
        {
            // Left Border
            colA = *(texImage + 1 * texWidth + i);
            colB = *(texImage + 2 * texWidth + i);

            r = 2 * ColorGetRed(colA) - ColorGetRed(colB);
            if (r < 0)
                r = 0;
            else if (r > 255)
                r = 255;

            g = 2 * ColorGetGreen(colA) - ColorGetGreen(colB);
            if (g < 0)
                g = 0;
            else if (g > 255)
                g = 255;

            b = 2 * ColorGetBlue(colA) - ColorGetBlue(colB);
            if (b < 0)
                b = 0;
            else if (b > 255)
                b = 255;

            *(texImage + i) = RGBAITOCOLOR(r, g, b, 255);

            // Right Border
            colA = *(texImage + (texHeight - 2) * texWidth + i);
            colB = *(texImage + (texHeight - 3) * texWidth + i);

            r = 2 * ColorGetRed(colA) - ColorGetRed(colB);
            if (r < 0)
                r = 0;
            else if (r > 255)
                r = 255;

            g = 2 * ColorGetGreen(colA) - ColorGetGreen(colB);
            if (g < 0)
                g = 0;
            else if (g > 255)
                g = 255;

            b = 2 * ColorGetBlue(colA) - ColorGetBlue(colB);
            if (b < 0)
                b = 0;
            else if (b > 255)
                b = 255;

            *(texImage + (texHeight - 1) * texWidth + i) = RGBAITOCOLOR(r, g, b, 255);
        }
    }
}

void LightmapGenerator::PackAllTextures()
{

    for (LightMapFaces **It = m_Objects.Begin();
         It < m_Objects.End();
         It++)
    {
        PackTextures(**It);
    }
}

void LightmapGenerator::PackTextures(LightMapFaces &ObjectMaps)
{
    tLightMapArray &lightmapsArray = ObjectMaps.textures;

    CKLevel *level = m_Context->GetCurrentLevel();
    CKScene *currentScene = level->GetCurrentScene();

    // Only one texture, no packing to do !
    if (lightmapsArray.Size() == 1)
    {
        // we add the texture to the level though
        level->AddObject(lightmapsArray[0]->texture);
        if (currentScene != level->GetLevelScene())
            currentScene->AddObject(lightmapsArray[0]->texture);

        return;
    }

    CKObject *object = m_Context->GetObject(ObjectMaps.object);

    // let's first calculate the total area of the textures

    int totalArea = 0;
    int maxWidth = 0;
    tLightMapArray::Iterator it;
    for (it = lightmapsArray.Begin(); it != lightmapsArray.End(); ++it)
    {
        CKTexture *tex = (*it)->texture;

        totalArea += tex->GetWidth() * tex->GetHeight();
        if (maxWidth < tex->GetWidth())
            maxWidth = tex->GetWidth();
    }

    int approximateWidth = Near2Power(int(sqrtf(float(totalArea))));
    if (approximateWidth * approximateWidth < totalArea)
        approximateWidth *= 2;
    int finalWidth = XMax(approximateWidth, maxWidth);

    char buffer[64];
    sprintf(buffer, "LightMap %s", object->GetName());
    CKTexture *finalTexture = (CKTexture *)m_Context->CreateObject(CKCID_TEXTURE, buffer);
    finalTexture->Create(finalWidth, finalWidth);
    CKDWORD *destImage = (CKDWORD *)finalTexture->LockSurfacePtr();
    CKDWORD fillColor = 0x00000000;
    VxFillStructure(finalWidth * finalWidth, destImage, sizeof(CKDWORD), sizeof(CKDWORD), &fillColor);

    level->AddObject(finalTexture);
    if (currentScene != level->GetLevelScene())
        currentScene->AddObject(finalTexture);

    lightmapsArray.Sort(LightmapGenerator::TextureSort);

    typedef XArray<Vx2DVector> PositionArray;
    PositionArray freePositions;

    // First free position
    freePositions.PushBack(Vx2DVector(0, 0));

    // We create the final lightmap information
    LightMapTexture *finalLightmap = new LightMapTexture;
    finalLightmap->texture = finalTexture;

    tLightMapArray finalArray;
    finalArray.PushBack(finalLightmap);

    int lastX = 0;
    int lastY = 0;

    // We now put the old texture into the big one
    for (it = lightmapsArray.Begin(); it != lightmapsArray.End();)
    {
        CKTexture *tex = (*it)->texture;
        int texWidth = tex->GetWidth();
        int texHeight = tex->GetHeight();

        // No More free positions, we exit !
        if (!freePositions.Size())
        {
            break;
        }

        // we get the first free position
        Vx2DVector fp = freePositions.PopBack();

        // we check if the texture can enter there
        if (fp.x + texWidth > finalWidth)
            continue;
        if (fp.y + texHeight > finalWidth)
            continue;

        lastX = XMax(lastX, int(fp.x) + texWidth);
        lastY = XMax(lastY, int(fp.y) + texHeight);

        CKDWORD *srcImage = (CKDWORD *)tex->LockSurfacePtr();

        // we copy the texture in place
        for (int i = 0; i < texHeight; ++i)
        {
            memcpy(destImage + finalWidth * (int(fp.y) + i) + int(fp.x), srcImage + texWidth * i, texWidth * sizeof(CKDWORD));
        }

        ///
        // Copy Information
        const tLightVertexArray &srcVertices = (*it)->vertices;
        tLightVertexArray &dstVertices = finalLightmap->vertices;

        // we need to offset the indices
        int offset = dstVertices.Size();
        for (XArray<CKWORD>::Iterator iit = (*it)->indices.Begin(); iit != (*it)->indices.End(); ++iit)
        {
            *iit += offset;
        }

        ///
        // Adding indices
        finalLightmap->indices += (*it)->indices;

        ///
        // Adding vertices
        VxUV startUV(fp.x / finalWidth, fp.y / finalWidth);
        VxUV sizeUV(float(texWidth) / finalWidth, float(texHeight) / finalWidth);

        // Adding original indices
        finalLightmap->originalface += (*it)->originalface;

        // We need to rescale the uvs
        for (tLightVertexArray::Iterator vit = srcVertices.Begin(); vit != srcVertices.End(); ++vit)
        {
            // UV rescaling
            (*vit).uv.u = startUV.u + (*vit).uv.u * sizeUV.u;
            (*vit).uv.v = startUV.v + (*vit).uv.v * sizeUV.v;
        }

        dstVertices += srcVertices;

        ///
        // Adding free positions for the next textures

        // Right One
        Vx2DVector newPosition(int(fp.x) + texWidth, int(fp.y));
        // We can insert the new position at the right only if it does not overlap a preceding free position in x
        int maxX = finalWidth;
        for (PositionArray::Iterator pit = freePositions.Begin(); pit != freePositions.End(); ++pit)
            maxX = XMin(int((*pit).x), maxX);
        if ((newPosition.x < finalWidth) && (newPosition.x < maxX))
            freePositions.PushBack(newPosition);

        // Bottom One
        newPosition.Set(int(fp.x), int(fp.y) + texHeight);
        if (newPosition.y < finalWidth)
            freePositions.PushBack(newPosition);

        // Next lightmap
        ++it;
    }

    int newWidth = Near2Power(lastX);
    if (newWidth < finalWidth)
    {
        // we need to resize the texture in width
        CKDWORD *newData = new CKDWORD[newWidth * finalWidth];
        for (int i = 0; i < finalWidth; ++i)
            memcpy(newData + newWidth * i, destImage + finalWidth * i, newWidth * sizeof(CKDWORD));
        finalTexture->Create(newWidth, finalWidth);
        destImage = (CKDWORD *)finalTexture->LockSurfacePtr();
        memcpy(destImage, newData, newWidth * finalWidth * sizeof(CKDWORD));
        delete[] newData;

        float factor = float(finalWidth) / newWidth;
        tLightVertexArray &srcVertices = finalLightmap->vertices;

        // We need to rescale the uvs
        for (tLightVertexArray::Iterator vit = srcVertices.Begin(); vit != srcVertices.End(); ++vit)
        {
            // UV rescaling
            (*vit).uv.u = (*vit).uv.u * factor;
        }
    }

    int newHeight = Near2Power(lastY);
    if (newHeight < finalWidth)
    {
        newWidth = finalTexture->GetWidth();
        // we need to resize the texture in width
        CKDWORD *newData = new CKDWORD[newHeight * finalWidth];
        for (int i = 0; i < newHeight; ++i)
            memcpy(newData + newWidth * i, destImage + newWidth * i, newWidth * sizeof(CKDWORD));
        finalTexture->Create(newWidth, newHeight);
        destImage = (CKDWORD *)finalTexture->LockSurfacePtr();
        memcpy(destImage, newData, newWidth * newHeight * sizeof(CKDWORD));
        delete[] newData;

        float factor = float(finalWidth) / newHeight;
        tLightVertexArray &srcVertices = finalLightmap->vertices;

        // We need to rescale the uvs
        for (tLightVertexArray::Iterator vit = srcVertices.Begin(); vit != srcVertices.End(); ++vit)
        {
            // UV rescaling
            (*vit).uv.v = (*vit).uv.v * factor;
        }
    }

    finalTexture->ReleaseSurfacePtr();
    // finalTexture->SetDesiredVideoFormat(_32_RGB888);

    // We destroy the old lightmaps
    for (tLightMapArray::Iterator it2 = lightmapsArray.Begin(); it2 != it; ++it2)
    {
        LightMapTexture *text = *it2;
        // we delete the texture
        CKDestroyObject(text->texture, CK_DESTROY_TEMPOBJECT);
        // we delete the face
        delete text;
    }

    // We add the texture not fitting in the final array
    for (; it != lightmapsArray.End(); ++it)
    {
        finalArray.PushBack(*it);

        // we add the texture to the level though
        level->AddObject((*it)->texture);
        if (currentScene != level->GetLevelScene())
            currentScene->AddObject((*it)->texture);

        *it = NULL;
    }

    // We swap the array
    lightmapsArray.Swap(finalArray);
}

void LightmapGenerator::ComputeLightmapFlat(const CKDWORD *pixelmap, const int twidth, const int theight)
{
    VxVector vX = (m_PolygonWorldVertices[3] - m_PolygonWorldVertices[0]) / (float)(twidth - 2);
    VxVector vY = (m_PolygonWorldVertices[1] - m_PolygonWorldVertices[0]) / (float)(theight - 2);
    VxVector svX = vX * m_InverseSampling;
    VxVector svY = vY * m_InverseSampling;

    CKDWORD *startmapx = const_cast<CKDWORD *>(pixelmap);
    VxVector startVX = m_PolygonWorldVertices[0] + m_PolygonPlane.m_Normal * 0.01f + svX * 0.5f + svY * 0.5f;

    // Loop variables
    VxColor lumelColor;
    VxVector startSVX, sv;

    startmapx += twidth; // skip the first line
    for (int i = 1; i < theight - 1; ++i, startmapx += twidth, startVX += vY)
    {
        CKDWORD *pixel = startmapx + 1;
        VxVector v = startVX;
        for (int j = 1; j < twidth - 1; ++j, ++pixel, v += vX)
        {

            // Color
            lumelColor.Set(0.0f, 0.0f, 0.0f, 0.0f);

            // Super Sampling
            startSVX = v;
            for (int si = 0; si < m_SuperSampling; ++si, startSVX += svY)
            {

                sv = startSVX;
                for (int sj = 0; sj < m_SuperSampling; ++sj, sv += svX)
                {

                    // The computationnal call
                    ComputePointColor(lumelColor, sv, m_PolygonPlane.m_Normal);
                }
            }

            // Supersampling median
            lumelColor *= m_InverseSampling * m_InverseSampling;

            // Ambiant color
            lumelColor += m_AmbientColor;
            lumelColor.a = 1.0f;

            *pixel = RGBAFTOCOLOR(&lumelColor);
        }
    }
}

void LightmapGenerator::ComputeLightmapSmooth(const CKDWORD *pixelmap, const int twidth, const int theight)
{
    const XArray<int> &findices = m_PolygonConstructor->GetPolyhedronFaceIndices(m_PolygonIndex);
    const VxBbox &box = m_PolygonConstructor->GetPolyhedronBox(m_PolygonIndex);
    int fcount = findices.Size();
    int a, b, c;

    ///
    // Lightmap loop
    VxVector init = box.Min;
    float pvx = box.GetSize().v[m_MajorX] / (twidth - 2);
    float pvy = box.GetSize().v[m_MajorY] / (theight - 2);
    float svx = pvx * m_InverseSampling;
    float svy = pvy * m_InverseSampling;

    VxVector start = init;
    CKDWORD *startmapx = const_cast<CKDWORD *>(pixelmap);
    startmapx += 1 + twidth; // Offseting for skipping the first line and pixel

    int i;
    for (i = 1; i < theight - 1; ++i, startmapx += twidth)
    {
        start.v[m_MajorX] = init.v[m_MajorX];

        CKDWORD *pixel = startmapx;
        for (int j = 1; j < twidth - 1; ++j, ++pixel)
        {
            VxVector v = start;

            // ambient light
            VxColor lumcolor;

            ///
            // Normal
            VxVector worldNormal = m_PolygonPlane.m_Normal; // If no face is found, we use the global normal
            float c0, c1, c2;
            for (int f = 0; f < fcount; ++f)
            {
                m_CurrentMesh->GetFaceVertexIndex(findices[f], a, b, c);

                const VxVector &pt0 = *(VxVector *)(m_PositionsPtr + a * m_PositionStride);
                const VxVector &pt1 = *(VxVector *)(m_PositionsPtr + b * m_PositionStride);
                const VxVector &pt2 = *(VxVector *)(m_PositionsPtr + c * m_PositionStride);

                VxIntersect::GetPointCoefficients(v, pt0, pt1, pt2, m_MajorX, m_MajorY, c0, c1, c2);

                if (c0 >= -EPSILON && c1 >= -EPSILON && c2 >= -EPSILON)
                { // we are on a real face
                    break;
                }
            }

            VxColor lastColor;

            ///
            // Supersampling
            for (int sy = 0; sy < m_SuperSampling; ++sy)
            { // Y

                for (int sx = 0; sx < m_SuperSampling; ++sx)
                { // X

                    ///
                    // We transform the data in the world

                    ///
                    // Position
                    const VxVector &pt0 = *(VxVector *)(m_PositionsPtr + a * m_PositionStride);
                    const VxVector &pt1 = *(VxVector *)(m_PositionsPtr + b * m_PositionStride);
                    const VxVector &pt2 = *(VxVector *)(m_PositionsPtr + c * m_PositionStride);

                    VxIntersect::GetPointCoefficients(v, pt0, pt1, pt2, m_MajorX, m_MajorY, c0, c1, c2);
                    if (c0 >= -EPSILON && c1 >= -EPSILON && c2 >= -EPSILON)
                    { // we are on a real face

                        VxVector lv = c0 * pt0 + c1 * pt1 + c2 * pt2;
                        VxVector worldPosition;
                        m_CurrentEntity->Transform(&worldPosition, &lv);

                        // we calculate the real normal
                        const VxVector &n0 = *(VxVector *)(m_NormalsPtr + a * m_NormalStride);
                        const VxVector &n1 = *(VxVector *)(m_NormalsPtr + b * m_NormalStride);
                        const VxVector &n2 = *(VxVector *)(m_NormalsPtr + c * m_NormalStride);

                        // TODO : bad way to calculate the normal !!!
                        VxVector n = c0 * n0 + c1 * n1 + c2 * n2;

                        m_CurrentEntity->TransformVector(&worldNormal, &n);
                        // m_CurrentEntity->TransformVector(&worldNormal,&m_PolygonConstructor->GetPolyhedronNormal(m_PolygonIndex));
                        worldNormal.Normalize();

                        // Offset
                        worldPosition += worldNormal * 0.01f;

                        lastColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
                        ComputePointColor(lastColor, worldPosition, worldNormal);

                        v.v[m_MajorX] += svx;
                    }
                    else
                    {
                        lastColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
                        ComputePointColor(lastColor, v, worldNormal);
                    }
                    lumcolor += lastColor;

                } // sx for

                v.v[m_MajorX] = start.v[m_MajorX];
                v.v[m_MajorY] += svy;

            } // Supersampling

            lumcolor *= (m_InverseSampling * m_InverseSampling);

            lumcolor += m_AmbientColor;
            lumcolor.a = 1.0f;

            *pixel = RGBAFTOCOLOR(&lumcolor);

            start.v[m_MajorX] += pvx;
        }
        start.v[m_MajorY] += pvy;
    } // lightmap filling
}

int LightmapGenerator::LightMapsRender(CKRenderContext *dev, CKRenderObject *ro, void *arg)
{
    LightmapGenerator *lg = (LightmapGenerator *)arg;
    CKContext *ctx = lg->GetCKContext();

    if (lg->m_ShowPolygons)
    {

        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
        dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CCW);
        dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
        dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ZERO);
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);

        CKDWORD colors[3];
        VxVector4 vertices[3];
        float uv[6] = {0, 0, 1, 0, 0, 1};
        colors[0] = colors[1] = colors[2] = RGBAFTOCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

        VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 3);

        // init the randomize generator
        srand(1);

        ///
        // Object loop
        for (tLightMappedObjectArray::Iterator it = lg->m_Objects.Begin(); it != lg->m_Objects.End();)
        {
            LightMapFaces *faces = *it;

            // We set the object matrix as the current one
            CK3dEntity *ent = (CK3dEntity *)ctx->GetObject(faces->object);
            if (ent)
            {
                // we draw only the lightmap if the object is visible
                if (ent != ro)
                {
                    ++it;
                    continue;
                }

                // We set the good vertices
                CKMesh *mesh = ent->GetCurrentMesh();
                CKDWORD Stride;
                CKBYTE *meshvertices = (CKBYTE *)mesh->GetPositionsPtr(&Stride);

                CKWORD *meshindices = mesh->GetFacesIndices();

                // We draw the light faces
                int f = 0;
                for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
                {
                    LightMapTexture *texture = *it2;

                    // Vertices
                    data->VertexCount = texture->vertices.Size();
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    data->PositionPtr = (CKBYTE *)&texture->vertices[0].position;
                    data->PositionStride = sizeof(LightVertex);
                    data->ColorPtr = (CKBYTE *)&texture->vertices[0].diffuse;
                    data->ColorStride = sizeof(LightVertex);
                    data->SpecularColorPtr = (CKBYTE *)&texture->vertices[0].specular;
                    data->SpecularColorStride = sizeof(LightVertex);
#else
                    data->Positions.Set(&texture->vertices[0].position, sizeof(LightVertex));
                    data->Colors.Set(&texture->vertices[0].diffuse, sizeof(LightVertex));
                    data->SpecularColors.Set(&texture->vertices[0].specular, sizeof(LightVertex));
#endif

                    // we need to set an "unique" color
                    CKDWORD uniqueColor = RGBAFTOCOLOR(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, 1.0f);

                    // color filling
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    VxFillStructure(data->VertexCount, data->ColorPtr, data->ColorStride, sizeof(CKDWORD), &uniqueColor);
#else
                    VxFillStructure(data->VertexCount, data->Colors.Ptr, data->Colors.Stride, sizeof(CKDWORD), &uniqueColor);
#endif

                    // The drawing
                    dev->DrawPrimitive(VX_TRIANGLELIST, texture->indices.Begin(), texture->indices.Size(), data);

                    // Restorng the initial color
                    uniqueColor = 0xffffffff;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    VxFillStructure(data->VertexCount, data->ColorPtr, data->ColorStride, sizeof(CKDWORD), &uniqueColor);
#else
                    VxFillStructure(data->VertexCount, data->Colors.Ptr, data->Colors.Stride, sizeof(CKDWORD), &uniqueColor);
#endif
                }
                break; // we only draw one entity
                ++it;
            }
            else
            {

                LightMapFaces *faces = *it;
                for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
                {
                    LightMapTexture *text = *it2;
                    // we delete the texture
                    CKDestroyObject(text->texture, CK_DESTROY_FREEID);
                    // we delete the face
                    delete text;
                }
                // We Clear the texture array
                faces->textures.Clear();
                // We delete the faces
                delete faces;

                // We remove the object
                it = lg->m_Objects.Remove(it);
            }
        }
    }
    else
    {
        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
        dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CCW);
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
        dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ZERO);
        dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_SRCCOLOR);

        VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 3);

        VxMatrix projmat = dev->GetProjectionTransformationMatrix();
        float oldproj = projmat[3].z;
        // projmat[3].z = oldproj*1.0001f; // to disable Z buffer fighting
        dev->SetProjectionTransformationMatrix(projmat);

        ///
        // Object loop
        for (tLightMappedObjectArray::Iterator it = lg->m_Objects.Begin(); it != lg->m_Objects.End();)
        {
            LightMapFaces *faces = *it;

            // We set the object matrix as the current one
            CK3dEntity *ent = (CK3dEntity *)ctx->GetObject(faces->object);
            if (ent)
            {
                // we draw only the lightmap if the object is visible
                if (ent != ro)
                {
                    ++it;
                    continue;
                }

                // We draw the light faces
                for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
                {
                    LightMapTexture *texture = *it2;

                    // Vertices
                    data->VertexCount = texture->vertices.Size();
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    data->PositionPtr = (CKBYTE *)&texture->vertices[0].position;
                    data->PositionStride = sizeof(LightVertex);
                    data->ColorPtr = (CKBYTE *)&texture->vertices[0].diffuse;
                    data->ColorStride = sizeof(LightVertex);
                    data->SpecularColorPtr = (CKBYTE *)&texture->vertices[0].specular;
                    data->SpecularColorStride = sizeof(LightVertex);
                    data->TexCoordPtr = (CKBYTE *)&texture->vertices[0].uv;
                    data->TexCoordStride = sizeof(LightVertex);
#else
                    data->Positions.Set(&texture->vertices[0].position, sizeof(LightVertex));
                    data->Colors.Set(&texture->vertices[0].diffuse, sizeof(LightVertex));
                    data->SpecularColors.Set(&texture->vertices[0].specular, sizeof(LightVertex));
                    data->TexCoord.Set(&texture->vertices[0].uv, sizeof(LightVertex));
#endif

                    // color filling
                    if (texture->vertices[0].diffuse != lg->m_Opacity)
                    {
                        // we fill only if different
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                        VxFillStructure(data->VertexCount, data->ColorPtr, data->ColorStride, sizeof(CKDWORD), &lg->m_Opacity);
#else
                        VxFillStructure(data->VertexCount, data->Colors.Ptr, data->Colors.Stride, sizeof(CKDWORD), &lg->m_Opacity);
#endif
                    }

                    // We set the good texture
                    if (texture->texture)
                        texture->texture->SetAsCurrent(dev);
                    dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
                    dev->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_MODULATE);
                    dev->SetTextureStageState(CKRST_TSS_ADDRESS, VXTEXTURE_ADDRESSCLAMP);
                    // dev->SetTextureStageState(CKRST_TSS_MAGFILTER , VXTEXTUREFILTER_NEAREST);
                    // dev->SetTextureStageState(CKRST_TSS_MINFILTER , VXTEXTUREFILTER_NEAREST);
                    dev->SetTextureStageState(CKRST_TSS_MAGFILTER, VXTEXTUREFILTER_LINEAR);
                    dev->SetTextureStageState(CKRST_TSS_MINFILTER, VXTEXTUREFILTER_LINEAR);

                    dev->DrawPrimitive(VX_TRIANGLELIST, texture->indices.Begin(), texture->indices.Size(), data);
                }

                break; // we only manage one entity
                ++it;
            }
            else
            {

                LightMapFaces *faces = *it;
                for (tLightMapArray::Iterator it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
                {
                    LightMapTexture *text = *it2;
                    // we delete the texture
                    CKDestroyObject(text->texture, CK_DESTROY_FREEID);
                    // we delete the face
                    delete text;
                }
                // We Clear the texture array
                faces->textures.Clear();
                // We delete the faces
                delete faces;

                // We remove the object
                it = lg->m_Objects.Remove(it);
            }
        }

        projmat[3].z = oldproj; // to disable Z buffer fighting
        dev->SetProjectionTransformationMatrix(projmat);

        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
    }

    return 0;
}

#ifdef DEBUGPOINT
void LightmapGenerator::LightMapsRenderDebug(CKRenderContext *dev, void *arg)
{
    LightmapGenerator *lg = (LightmapGenerator *)arg;
    CKContext *ctx = lg->GetCKContext();

    dev->SetWorldTransformationMatrix(VxMatrix::Identity());

    // Debug Rendering
    dev->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
    dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ZERO);

    int dcount = lg->m_DebugPoints.Size();

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, dcount);
    XPtrStrided<VxVector> pptr(data->Positions);
    XPtrStrided<CKDWORD> cptr(data->Colors);

    for (int i = 0; i < dcount; ++i, ++pptr, ++cptr)
    {
        const DebugPoint &dbp = lg->m_DebugPoints[i];
        *pptr = dbp.pos;
        if (dbp.hit)
        {
            *cptr = 0xffff0000;
        }
        else
        {
            *cptr = 0xff00ff00;
        }
    }

    dev->DrawPrimitive(VX_POINTLIST, NULL, NULL, data);
}
#endif

///
// Protected Methods

CKBOOL
LightmapGenerator::ComputeTransparency(const CK3dEntity &ent, const VxIntersectionDesc &desc, VxColor &trans)
{
    CKMaterial *mat = ent.GetCurrentMesh()->GetFaceMaterial(desc.FaceIndex);
    if (!mat)
        return FALSE;

    if (!mat->IsAlphaTransparent())
        return FALSE;

    VxColor diffuse = mat->GetDiffuse();
    CKTexture *tex;
    if (tex = mat->GetTexture())
    {
        VXTEXTURE_ADDRESSMODE add = mat->GetTextureAddressMode();
        float u = desc.TexU;
        float v = desc.TexV;

        switch (add)
        {
        case VXTEXTURE_ADDRESSWRAP:
            u -= (int)u;
            if (u < 0.0f)
                u += 1.0f;
            v -= (int)v;
            if (v < 0.0f)
                v += 1.0f;
            break;
        case VXTEXTURE_ADDRESSCLAMP:
        case VXTEXTURE_ADDRESSBORDER:
            if (u < 0.0f)
                u = 0.0f;
            else if (u > 1.0f)
                u = 1.0f;
            if (v < 0.0f)
                v = 0.0f;
            else if (v > 1.0f)
                v = 1.0f;
            break;
        case VXTEXTURE_ADDRESSMIRROR:
            int tu = (int)u;
            u -= tu;
            if (u < 0.0f)
                u += 1.0f;
            if (tu & 1) // odd => invert
                u = 1.0f - u;

            int tv = (int)v;
            v -= tv;
            if (v < 0.0f)
                v += 1.0f;
            if (tv & 1) // odd => invert
                v = 1.0f - v;
            break;
        };

        int X = (int)(u * (float)(tex->GetWidth() - 1));
        int Y = (int)(v * (float)(tex->GetHeight() - 1));
        VxColor col(tex->GetPixel(X, Y));

        // TODO : manage the modulate, decal,
        diffuse *= col;
    }
    // Todo : manage other blend modes than srcalpha invsrcalpha
    trans = VxColor(diffuse.a) * diffuse + trans * (1.0f - diffuse.a);

    return TRUE;
}

CKBOOL
LightmapGenerator::ComputeVisibility(CK3dEntity *caster, const VxVector &start, const VxVector &dest, VxColor &trans)
{
    trans.Set(1.0f, 1.0f, 1.0f, 0.0f);
    if (!m_FilteredOccluders[m_CurrentLight].Size())
        return TRUE; // Fast skip test
    /*
    // Bad Static here... Boo
    static VxIntersectionDesc desc;

    CK3dEntity* ent = m_Octree.RayIntersection(m_Context,start,dest,&desc);
    if (ent && (ent != caster)) return FALSE;

    return TRUE;
    */
    //*
    float distance = Magnitude(dest - start);
    VxRay ray(start, dest);
    VxVector dir = ray.m_Direction / distance;

    VxIntersectionDesc desc;

    VxVector temp = start;
    CKBOOL occlusionFound = FALSE;

    for (;;)
    {

        XObjectPointerArray::Iterator it;
        for (it = m_FilteredOccluders[m_CurrentLight].Begin(); it != m_FilteredOccluders[m_CurrentLight].End(); ++it)
        {
            CK3dObject *CurrentEntity = (CK3dObject *)*it;

            // Bounding Sphere
            /*
            VxVector v;CurrentEntity->GetBarycenter(&v);v-=start;
            float dist = DotProduct(v,dir);
            dist = SquareMagnitude(v)-dist*dist;
            float radius = CurrentEntity->GetRadius();
            if (radius*radius < dist) continue;
            */

            // World Box
            ++m_RayBoxPerformed;
            if (!VxIntersect::SegmentBox(ray, CurrentEntity->GetBoundingBox()))
                continue;

            // Local Box
            // VxRay localray;ray.Transform(localray,CurrentEntity->GetInverseWorldMatrix());
            // if (!VxIntersect::SegmentBox(localray,CurrentEntity->GetBoundingBox(TRUE))) continue;

            ++m_RayIntPerformed;
            if (CurrentEntity->RayIntersection(&temp, &dest, &desc, NULL))
            {

                // Distance rejection TODO check this some problem noted
                if (desc.Distance > distance)
                {
                    continue;
                }

                // Patch for self occlusion of smooth objects : a smooth polygon cannot self shadow itself
                if (m_CurrentEntity == CurrentEntity)
                {
                    const XArray<int> &findices = m_PolygonConstructor->GetPolyhedronFaceIndices(m_PolygonIndex);
                    if (findices.IsHere(desc.FaceIndex))
                        continue;
                }
#ifdef DEBUGPOINT
                m_DebugPoints.PushBack(DebugPoint(dest, TRUE));
#endif
                // we now search if the point was transparent
                if (ComputeTransparency(*CurrentEntity, desc, trans))
                {
                    occlusionFound = TRUE;
                    return TRUE;
                    // we calculate from where the light starts now
                    temp += (dest - temp) * (desc.Distance + 0.001f);
                }
                else
                {
                    return FALSE;
                }
            }
        }
        // No occluders have been found
        if (it == m_FilteredOccluders[m_CurrentLight].End())
            break;
    }

#ifdef DEBUGPOINT
    m_DebugPoints.PushBack(DebugPoint(dest, FALSE));
#endif

    return !occlusionFound;

    /*/
    BSPManager *BSPman = (BSPManager*)m_Context->GetManagerByGuid(BSP_MANAGER_GUID);

if (BSPman->BSPTree)
{
    VxVector Intersection;
    BSPFace *bspf = NULL;

    VxVector s(start);
    VxVector e(dest);

    BOOL inter = BSPman->RayIntersection(&e,&s,&Intersection,&bspf);
    if (inter) return bspf->m_Mesh == caster->GetCurrentMesh();
    return TRUE;

}
    return TRUE;


    //*/
}

void LightmapGenerator::ComputePointColor(VxColor &lumcolor, const VxVector &worldPosition, const VxVector &worldNormal)
{
    VxColor transColor;

    // light traversal
    m_CurrentLight = 0;
    for (XObjectPointerArray::Iterator it = m_FilteredLights.Begin(); it != m_FilteredLights.End(); ++it, ++m_CurrentLight)
    {
        CKLight *light = (CKLight *)*it;

        const VxVector &lightPosition = light->GetWorldMatrix()[3];

        switch (light->GetType())
        {
        case VX_LIGHTDIREC:
        {
            VxVector lightDirection = Normalize(light->GetWorldMatrix()[2]);
            float factor = -DotProduct(worldNormal, lightDirection);
            if ((factor > 0) && ComputeVisibility(m_CurrentEntity, worldPosition + lightDirection * -100000.0f, worldPosition, transColor))
                lumcolor += light->GetColor() * transColor * factor;
        }
        break;
        case VX_LIGHTPOINT:
        {
            VxVector direction = lightPosition - worldPosition;
            float att = Magnitude(direction);
            float lightRange = light->GetRange();
            if (att < lightRange)
            {
                direction /= att;
                att = (lightRange - att) / lightRange;
                att = light->GetConstantAttenuation() + light->GetLinearAttenuation() * att + light->GetQuadraticAttenuation() * att * att;

                float factor = att * DotProduct(worldNormal, direction);
                if ((factor > 0) && ComputeVisibility(m_CurrentEntity, lightPosition, worldPosition, transColor))
                    lumcolor += light->GetColor() * transColor * factor;
            }
        }
        break;
        case VX_LIGHTSPOT:
        {
            VxVector direction = lightPosition - worldPosition;
            float att = Magnitude(direction);
            float lightRange = light->GetRange();
            float hotspot = light->GetHotSpot() * 0.5f;
            float falloff = light->GetFallOff() * 0.5f;
            float falloffshape = light->GetFallOffShape();
            float costheta = cosf(hotspot);
            float cosphi = cosf(falloff);
            float invthetaminusphi = 1.0f / (costheta - cosphi);

            if (att < lightRange)
            {
                direction /= att;
                float rho = DotProduct(-Normalize(light->GetWorldMatrix()[2]), direction);
                if (rho > cosphi)
                {
                    float spot = 1.0f;
                    if (rho <= costheta)
                    {
                        spot = (rho - cosphi) * (invthetaminusphi);
                        if (falloffshape != 1.0f)
                            spot = powf(spot, falloffshape); // TODO is falloff THIS important ?
                    }
                    float attspot = light->GetConstantAttenuation();
                    att = (lightRange - att) / lightRange;
                    attspot += light->GetLinearAttenuation() * att + light->GetQuadraticAttenuation() * att * att;
                    attspot *= spot;
                    float factor = attspot * DotProduct(worldNormal, direction);
                    if ((factor > 0) && ComputeVisibility(m_CurrentEntity, lightPosition, worldPosition, transColor))
                        lumcolor += light->GetColor() * transColor * factor;
                }
            }
        }
        break;
        }
    } // Lights for
}

///
// Objects Creation
void LightmapGenerator::GenerateObjects(CKContext *iCtx)
{
    CKLevel *level = iCtx->GetCurrentLevel();

    ///
    // Object loop
    for (tLightMappedObjectArray::Iterator it = m_Objects.Begin(); it != m_Objects.End(); ++it)
    {
        LightMapFaces *faces = *it;

        // We set the object matrix as the current one
        CK3dEntity *ent = (CK3dEntity *)iCtx->GetObject(faces->object);
        if (!ent)
            continue;
        CKMesh *mesh = ent->GetCurrentMesh();
        if (!mesh)
            continue;

        CKTexture *tex = (*it)->textures[0]->texture;

        XString temp = "LM ";
        // We create the object
        temp << ent->GetName();
        CK3dEntity *nent = (CK3dEntity *)iCtx->CreateObject(CKCID_3DOBJECT, temp.Str());
        // then the mesh
        temp << " Mesh";
        CKMesh *nmesh = (CKMesh *)iCtx->CreateObject(CKCID_MESH, temp.Str());
        // then the material
        temp = "LM ";
        temp << ent->GetName() << " Material";
        CKMaterial *nmat = (CKMaterial *)iCtx->CreateObject(CKCID_MATERIAL, temp.Str());

        CKScene *currentScene = level->GetCurrentScene();

        // we add the objects to the level
        level->AddObject(nent);
        level->AddObject(nmesh);
        level->AddObject(nmat);

        if (currentScene != level->GetLevelScene())
        { // If there is a current scene
            currentScene->AddObject(nent);
            currentScene->AddObject(nmesh);
            currentScene->AddObject(nmat);
        }

        ///
        // Entity creation
        nent->SetCurrentMesh(nmesh);
        nent->SetWorldMatrix(ent->GetWorldMatrix());
        nent->SetParent(ent->GetParent());

        ///
        // Material Creation
        nmat->SetTexture0(tex);

        ///
        // Mesh Creation
        CKBOOL prelit = (mesh->GetLitMode() == VX_PRELITMESH);

        if (prelit)
            nmesh->SetLitMode(VX_PRELITMESH);

        // first iterate for vertices and indices count
        int verticesCount = 0;
        int indicesCount = 0;

        tLightMapArray::Iterator it2;
        for (it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
        {
            LightMapTexture *texture = *it2;

            verticesCount += texture->vertices.Size();
            indicesCount += texture->indices.Size();
        }

        // Allocation
        nmesh->SetVertexCount(verticesCount);
        nmesh->SetFaceCount(indicesCount / 3);

        CKDWORD pStride;
        CKBYTE *positions = (CKBYTE *)nmesh->GetPositionsPtr(&pStride);

        CKDWORD uvStride;
        CKBYTE *uvs = (CKBYTE *)nmesh->GetTextureCoordinatesPtr(&uvStride);

        CKDWORD normalStride;
        CKBYTE *normals = (CKBYTE *)nmesh->GetNormalsPtr(&normalStride);

        CKDWORD dStride;
        CKBYTE *diffuse = (CKBYTE *)nmesh->GetColorsPtr(&dStride);

        CKDWORD sStride;
        CKBYTE *specular = (CKBYTE *)nmesh->GetSpecularColorsPtr(&sStride);

        int offset = 0;
        int faceCount = 0;

        // Create the mesh
        for (it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
        {
            LightMapTexture *texture = *it2;

            // add the vertices
            int i;
            for (i = 0; i < texture->vertices.Size(); ++i)
            {
                const LightVertex &v = texture->vertices[i];

                mesh->GetVertexPosition(v.index, (VxVector *)positions);
                positions += pStride;

                if (prelit)
                {
                    *(CKDWORD *)diffuse = mesh->GetVertexColor(v.index);
                    diffuse += dStride;

                    *(CKDWORD *)specular = mesh->GetVertexSpecularColor(v.index);
                    specular += sStride;
                }
                else
                {
                    mesh->GetVertexNormal(v.index, (VxVector *)normals);
                    normals += normalStride;
                }

                mesh->GetVertexTextureCoordinates(v.index, &((VxUV *)uvs)->u, &((VxUV *)uvs)->v);
                uvs += uvStride;
            }

            // Add the indices
            int f = 0;
            for (CKWORD *ind = texture->indices.Begin(); ind != texture->indices.End(); ++f)
            {
                CKWORD a = offset + *(ind++);
                CKWORD b = offset + *(ind++);
                CKWORD c = offset + *(ind++);

                nmesh->SetFaceVertexIndex(faceCount, a, b, c);

                // ARGH Bug !!!
                nmesh->SetFaceMaterial(faceCount, mesh->GetFaceMaterial(texture->originalface[f]));
                faceCount++;
            }

            // Offset for the indices
            offset += texture->vertices.Size();
        }

        // Channel Creation
        nmesh->AddChannel(nmat, FALSE);
        nmesh->ActivateChannel(0);
        nmesh->SetChannelSourceBlend(0, VXBLEND_ZERO);
        nmesh->SetChannelDestBlend(0, VXBLEND_SRCCOLOR);
        nmesh->LitChannel(0, FALSE);

        uvs = (CKBYTE *)nmesh->GetTextureCoordinatesPtr(&uvStride, 0);

        // Fill the channel
        for (it2 = faces->textures.Begin(); it2 != faces->textures.End(); it2++)
        {
            LightMapTexture *texture = *it2;

            // add the vertices
            int i;
            for (i = 0; i < texture->vertices.Size(); ++i)
            {
                const LightVertex &v = texture->vertices[i];

                *(VxUV *)uvs = v.uv;
                uvs += uvStride;
            }
        }

        nmesh->BuildFaceNormals();
    }
}
