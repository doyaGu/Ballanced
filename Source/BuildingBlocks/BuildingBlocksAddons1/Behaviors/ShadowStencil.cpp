/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              ShadowStencil
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorShadowStencilDecl();
CKERROR CreateShadowStencilProto(CKBehaviorPrototype **);
int ShadowStencil(const CKBehaviorContext &behcontext);

extern char *shadowStencilCasterAttributeName;
extern char *shadowStencilCasterMeshAttributeName;

/**********************************************/
/*				DECLARATION
/**********************************************/
CKObjectDeclaration *FillBehaviorShadowStencilDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("ShadowStencil");
    od->SetDescription("Use the stencil buffer to create a shadow volume that cast on objects using the ShadowStencil attribute.");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x60a873c5, 0x51245a67));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShadowStencilProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Visuals/Shadows");
    return od;
}

/**********************************************/
/*				PROTO
/**********************************************/
CKERROR CreateShadowStencilProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("ShadowStencil");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Shadow Range Factor", CKPGUID_FLOAT, "20");
    proto->DeclareInParameter("Shadow Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Show Shadow Volumes", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Camera Can Penetrate Shadow", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("Light Occluders Count", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShadowStencil);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

//______________________________________________
//                                EDGE STRUCTURE

// NeigFace
struct NeigFace
{
    CKWORD neig0; // neighboor face of edge 0
    CKWORD neig1; // neighboor face of edge 1
    CKWORD neig2; // neighboor face of edge 2
    CKWORD flag;  // 0=Not to be parsed, 1=Front face, 2=Back face
};

// Vector2Int
struct Vector2Int
{
    int i1, i2;
};
inline int operator==(const Vector2Int &v1, const Vector2Int &v2)
{
    return ((v1.i1 == v2.i1) && (v1.i2 == v2.i2));
}

// XHashFun defined for a edge
template <>
struct XHashFun<Vector2Int>
{
    int operator()(const Vector2Int &__s) const { return (int)(__s.i1 * __s.i2); }
};
typedef XHashFun<Vector2Int> XHashFunEdge;

// XHashFun defined for a VxVector
template <>
struct XHashFun<VxVector>
{
    int operator()(const VxVector &__s) const { return (int)(__s.x + __s.y + __s.z); }
};
typedef XHashFun<VxVector> XHashFunVector;

//--- Convenient Functions
inline void getUniqueEdge(int i1, int i2, Vector2Int *v2i)
{
    if (i1 < i2)
    {
        v2i->i1 = i1;
        v2i->i2 = i2;
    }
    else
    {
        v2i->i1 = i2;
        v2i->i2 = i1;
    }
}

void addEdge(int i1, int i2, int face, XHashTable<Vector2Int, Vector2Int, XHashFunEdge> &hEdge)
{
    Vector2Int v2iKey;
    getUniqueEdge(i1, i2, &v2iKey);

    Vector2Int *v2iFacesPtr = hEdge.FindPtr(v2iKey);
    if (v2iFacesPtr)
        v2iFacesPtr->i2 = face;
    else
    {
        Vector2Int faceids;
        faceids.i1 = face;
        faceids.i2 = 0xFFFF;
        hEdge.Insert(v2iKey, faceids);
    }
}
//---

// Flags On The Mesh
enum NeigFaceStructureFlag
{
    NFS_NONE = 0x00000000,
    NFS_MESHCHANGE = 0x00000001
};

// Edge Structure
// Used to store edges once for all.
class NeigFaceStructure
{
public:
    // Final Array of edges containing all edges between non-coplanar faces
    XArray<NeigFace> m_NeigFace;

    // Face Count, and Vertex Count for which the Neig Face Structure has been calculated
    int m_FCount, m_VCount;
    int m_skinUpdate;

    CKDWORD m_Flag;

    // Position of the Light for which the Flags has been calculated
    VxVector m_LightPos;

    //---------------------------------------------------
    //--- Get Face Neighboor
    CKDWORD GetFaceNeighboor(int face, int i1, int i2, XHashTable<Vector2Int, Vector2Int, XHashFunEdge> &hEdge)
    {
        Vector2Int v2iKey, v2f;
        getUniqueEdge(i1, i2, &v2iKey);
        v2f = *hEdge.Find(v2iKey);
        if (v2f.i1 == face)
            return v2f.i2;
        else
            return v2f.i1;
    }

    //---------------------------------------------------
    //--- Build Neig Face Structure
    CKBOOL BuildNeigFaceStruct(CKMesh *mesh, float threshold)
    {

        if (mesh->GetFlags() & VXMESH_PROCEDURALPOS && m_skinUpdate < 1)
        {
            ++m_skinUpdate;
            return FALSE;
        }

        // init flags
        m_Flag = NFS_NONE;

        // put a non valid VxVector value into m_LightPos
        // to force the first building of face Flags
        *(CKDWORD *)&m_LightPos.x = 0xFFFFFFFF;
        *(CKDWORD *)&m_LightPos.y = 0xFFFFFFFF;
        *(CKDWORD *)&m_LightPos.z = 0xFFFFFFFF;

        m_VCount = mesh->GetVertexCount();

        int a;

        // Array of point indices built such that two points
        // at the same position will use only one index
        XArray<int> m_P;

        m_P.Resize(m_VCount);

        // Hash table to store vertices index
        // in relation to their position
        XHashTable<int, VxVector, XHashFunVector> VIndex(m_VCount + 1);

        CKDWORD posStride;
        VxVector *pos = (VxVector *)mesh->GetPositionsPtr(&posStride);
        if (!pos)
            return FALSE;
        int *correspondingV;
        for (a = 0; a < m_VCount; ++a)
        {
            correspondingV = VIndex.FindPtr(*pos);
            if (correspondingV)
            {
                m_P[a] = *correspondingV;
            }
            else
            {
                m_P[a] = a;
                VIndex.Insert(*pos, a);
            }
            pos = (VxVector *)((CKBYTE *)pos + posStride);
        }

        // Now we only have in m_P unique position vertices index

        m_FCount = mesh->GetFaceCount();
        // Hash table to store edges
        // in relation to their [p1,p2]
        XHashTable<Vector2Int, Vector2Int, XHashFunEdge> hEdge(m_FCount * 3 + 1);
        int i1, i2, i3;
        for (a = 0; a < m_FCount; ++a)
        {
            mesh->GetFaceVertexIndex(a, i1, i2, i3);

            i1 = m_P[i1];
            i2 = m_P[i2];
            i3 = m_P[i3];

            addEdge(i1, i2, a, hEdge);
            addEdge(i2, i3, a, hEdge);
            addEdge(i3, i1, a, hEdge);
        }

        // Parse All Faces, and build the m_NeigFace structure
        m_NeigFace.Resize(m_FCount);
        for (a = 0; a < m_FCount; ++a)
        {

            m_NeigFace[a].flag = 1; // for the moment we admit all face to be parsable

            mesh->GetFaceVertexIndex(a, i1, i2, i3);

            i1 = m_P[i1];
            i2 = m_P[i2];
            i3 = m_P[i3];

            m_NeigFace[a].neig0 = (CKWORD)GetFaceNeighboor(a, i1, i2, hEdge);
            m_NeigFace[a].neig1 = (CKWORD)GetFaceNeighboor(a, i2, i3, hEdge);
            m_NeigFace[a].neig2 = (CKWORD)GetFaceNeighboor(a, i3, i1, hEdge);
        }

        return TRUE;
    }

    //---------------------------------------------------
    //--- Parse Faces And Flag Them
    void ParseFacesAndFlagThem(CKMesh *mesh, VxVector &posLight)
    {

        m_LightPos = posLight;

        CKDWORD nStride;
        VxVector *fnorm = (VxVector *)mesh->GetFaceNormalsPtr(&nStride);

        for (int a = 0; a < m_FCount; ++a)
        {
            if (m_NeigFace[a].flag)
            {
                // if face is to be parsed
                m_NeigFace[a].flag = (DotProduct(mesh->GetFaceVertex(a, 0) - m_LightPos, *fnorm) < 0.0f) ? 1 : 2;
            }
            fnorm = (VxVector *)((CKBYTE *)fnorm + nStride);
        }
    }

    //--- Constructor
    NeigFaceStructure()
    {
        m_FCount = m_VCount = 0;
        m_Flag = NFS_NONE;
        m_LightPos.Set(0, 0, 0);
        m_skinUpdate = 0;
    }

    //--- Destructor
    ~NeigFaceStructure()
    {
    }
};

//______________________________________________
//        POINT IS INSIDE SHADOW VOLUME FUNCTION
//
// retrieves the number of intersection between the light and camera through Obj.
// posCam, and posLight are both given in object's referential.
inline int LightIsHiddenByObject(const VxVector *posLight, const VxVector *posCam, CK3dEntity *obj)
{
    return obj->RayIntersection(posLight, posCam, NULL, obj, CK_RAYINTERSECTION(CKRAYINTERSECTION_SEGMENT));
}

//______________________________________________
//                                SHAPE RENDERER

//--- Convenient Function for the shape renderer
inline void fillVertexInfo(CKWORD *findice, int face3, int indexOnFace, int *pointAlreadyParsed,
                           VxVector *&dataPos, CKDWORD dataPosStride,
                           VxVector *pos, CKDWORD posStride, int &currentIndex, int &resIndex,
                           VxVector lightPos, float range)
{
    const CKWORD i = *(findice + face3 + indexOnFace);

    if (pointAlreadyParsed[i] == -1)
    {
        pointAlreadyParsed[i] = currentIndex;
        resIndex = currentIndex;

        // vertex position
        *dataPos = *(VxVector *)((CKBYTE *)pos + posStride * i);

        // projected position
        *((VxVector *)((CKBYTE *)dataPos + dataPosStride)) = *dataPos + (*dataPos - lightPos) * range;

        currentIndex += 2;

        dataPos = (VxVector *)((CKBYTE *)dataPos + dataPosStride + dataPosStride);
    }
    else
    {
        // use a already parsed vertex
        resIndex = pointAlreadyParsed[i];
    }
}

inline void fill6IndicesFrom1Edge(CKWORD *&indicesPtr, int i0, int i1)
{
    *indicesPtr = i0;
    ++indicesPtr;
    *indicesPtr = i0 + 1;
    ++indicesPtr;
    *indicesPtr = i1;
    ++indicesPtr;

    *indicesPtr = i1;
    ++indicesPtr;
    *indicesPtr = i0 + 1;
    ++indicesPtr;
    *indicesPtr = i1 + 1;
    ++indicesPtr;
}
//---

// Shape Renderer
// Used to build and render the shadow volume
class ShapeRenderer
{
public:
    CKRenderContext *m_RenderContext;
    float m_Range, m_OldZProjMat;
    CKMesh *m_Mesh;
    VxVector m_LightPos;
    VxDrawPrimitiveData *m_Data;
    VxVector *m_DataPos;
    CKDWORD m_DataPosStride;
    int m_IndiceCount;
    CKWORD *m_Indices;

    //---------------------------------------------------
    //--- Prepare To Render
    void PrepareToRender(int maxVertexCount, int maxFaceCount, CK3dEntity *obj, VxVector *lightPos)
    {
        // the m_Data is reserved for the position of all points,
        // and their projected position (beside each position)
        m_Data = m_RenderContext->GetDrawPrimitiveStructure(
            (CKRST_DPFLAGS)(CKRST_DP_TR_CL_VC), maxVertexCount * 2);

        // array of maximum indices used for the Draw Primitive
        // in worse case we can have 3 shape edges per faces = 18 indices per faces
        m_Indices = m_RenderContext->GetDrawPrimitiveIndices(maxFaceCount);

        // init variables
        m_Data->VertexCount = 0; // will be recalculated further
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        m_DataPos = (VxVector *)m_Data->PositionPtr;
        m_DataPosStride = m_Data->PositionStride;
#else
        m_DataPos = (VxVector *)m_Data->Positions.Ptr;
        m_DataPosStride = m_Data->Positions.Stride;
#endif
        m_RenderContext->SetWorldTransformationMatrix(obj->GetWorldMatrix());
        m_LightPos = *lightPos;
        m_Mesh = obj->GetCurrentMesh();
    }

    //---------------------------------------------------
    //--- Parse Faces And Fill Draw Primitives
    void ParseFacesAndFillDrawPrimitives(NeigFaceStructure *nfs)
    {
        int fcount = nfs->m_FCount;
        XArray<NeigFace> &neigFace = nfs->m_NeigFace;
        NeigFace *neigFacePtr = nfs->m_NeigFace.Begin();

        CKDWORD posStride;
        VxVector *pos = (VxVector *)m_Mesh->GetPositionsPtr(&posStride);
        CKWORD *findice = m_Mesh->GetFacesIndices();

        VxVector *dataPos = m_DataPos;

        int face3 = 0;
        int currentIndex = 0;
        int i0, i1;

        // array of point already parsed
        // (to avoid calculate the projection twice and to retrieve correct indices)
        // (so we also don't have to tranform all points)
        VxMemoryPool memPool1(nfs->m_VCount * sizeof(CKDWORD));
        int *pointAlreadyParsed = (int *)memPool1.Buffer();
        memset(pointAlreadyParsed, -1, sizeof(int) * nfs->m_VCount);

        CKWORD *indicesPtr = m_Indices;

        // parse faces and fill VertexInfo array
        for (int face = 0; face < fcount; ++face, ++neigFacePtr, face3 += 3)
        {
            if (neigFacePtr->flag == 1)
            {
                // it's a front face

                // store edge 0 ?
                if ((neigFacePtr->neig0 == 0xFFFF) || (neigFace[neigFacePtr->neig0].flag == 2))
                {
                    fillVertexInfo(findice, face3, 0, pointAlreadyParsed, dataPos, m_DataPosStride, pos, posStride, currentIndex, i0, m_LightPos, m_Range);
                    fillVertexInfo(findice, face3, 1, pointAlreadyParsed, dataPos, m_DataPosStride, pos, posStride, currentIndex, i1, m_LightPos, m_Range);
                    fill6IndicesFrom1Edge(indicesPtr, i0, i1);
                }

                // store edge 1 ?
                if ((neigFacePtr->neig1 == 0xFFFF) || (neigFace[neigFacePtr->neig1].flag == 2))
                {
                    fillVertexInfo(findice, face3, 1, pointAlreadyParsed, dataPos, m_DataPosStride, pos, posStride, currentIndex, i0, m_LightPos, m_Range);
                    fillVertexInfo(findice, face3, 2, pointAlreadyParsed, dataPos, m_DataPosStride, pos, posStride, currentIndex, i1, m_LightPos, m_Range);
                    fill6IndicesFrom1Edge(indicesPtr, i0, i1);
                }

                // store edge 2 ?
                if ((neigFacePtr->neig2 == 0xFFFF) || (neigFace[neigFacePtr->neig2].flag == 2))
                {
                    fillVertexInfo(findice, face3, 2, pointAlreadyParsed, dataPos, m_DataPosStride, pos, posStride, currentIndex, i0, m_LightPos, m_Range);
                    fillVertexInfo(findice, face3, 0, pointAlreadyParsed, dataPos, m_DataPosStride, pos, posStride, currentIndex, i1, m_LightPos, m_Range);
                    fill6IndicesFrom1Edge(indicesPtr, i0, i1);
                }
            }
        }

        // calculate final shadow volume vertex count
        m_Data->VertexCount = ((CKBYTE *)dataPos - (CKBYTE *)m_DataPos) / m_DataPosStride;

        // calculate final shadow volume indices count
        m_IndiceCount = indicesPtr - m_Indices;
    }

    //---------------------------------------------------
    //--- Draw View Rect
    void DrawViewRect(CKDWORD *dwcol)
    {
        VxDrawPrimitiveData *data = m_RenderContext->GetDrawPrimitiveStructure(CKRST_DP_VC, 4);

        VxRect rect;
        m_RenderContext->GetViewRect(rect);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        XPtrStrided<VxVector4> rectVPos(data->PositionPtr, data->PositionStride);
#else
        XPtrStrided<VxVector4> rectVPos(data->Positions.Ptr, data->Positions.Stride);
#endif

        rectVPos->Set(rect.left, rect.top, 0.5f, 1.0f); // point0
        ++rectVPos;

        rectVPos->Set(rect.right, rect.top, 0.5f, 1.0f); // point1
        ++rectVPos;

        rectVPos->Set(rect.right, rect.bottom, 0.5f, 1.0f); // point2
        ++rectVPos;

        rectVPos->Set(rect.left, rect.bottom, 0.5f, 1.0f); // point3
        ++rectVPos;

        if (dwcol)
        {
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            VxFillStructure(4, data->ColorPtr, data->ColorStride, sizeof(CKDWORD), dwcol);
#else
            VxFillStructure(4, data->Colors.Ptr, data->Colors.Stride, sizeof(CKDWORD), dwcol);
#endif
        }

        m_RenderContext->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);

        m_RenderContext->DrawPrimitive(VX_TRIANGLEFAN, (CKWORD *)0, 0, data);
    }

    //---------------------------------------------------
    //--- Draw Shadow
    void DrawShadow(const VxColor *colPtr)
    {
        // fill colors data (if necessary)
        if (colPtr)
        {
            CKDWORD dwcol = colPtr->GetRGBA();
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            VxFillStructure(m_Data->VertexCount, m_Data->ColorPtr, m_Data->ColorStride, sizeof(CKDWORD), &dwcol);
#else
            VxFillStructure(m_Data->VertexCount, m_Data->Colors.Ptr, m_Data->Colors.Stride, sizeof(CKDWORD), &dwcol);
#endif
        }

        m_RenderContext->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CCW);
        m_RenderContext->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_INCR);

        m_RenderContext->DrawPrimitive(VX_TRIANGLELIST, m_Indices, m_IndiceCount, m_Data);

        m_RenderContext->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CW);
        m_RenderContext->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_DECR);

        m_RenderContext->DrawPrimitive(VX_TRIANGLELIST, m_Indices, m_IndiceCount, m_Data);
    }

    //--- Constructor
    ShapeRenderer(CKRenderContext *rc, float range) : m_RenderContext(rc), m_Range(range)
    {
        VxMatrix projmat = rc->GetProjectionTransformationMatrix();
        m_OldZProjMat = projmat[3].z;
        projmat[3].z = m_OldZProjMat * 0.999f; // to disable Z buffer fighting
        rc->SetProjectionTransformationMatrix(projmat);
    }

    //--- Destructor
    ~ShapeRenderer()
    {
        VxMatrix projmat = m_RenderContext->GetProjectionTransformationMatrix();
        projmat[3].z = m_OldZProjMat; // restore projection matrix
        m_RenderContext->SetProjectionTransformationMatrix(projmat);
    }
};

//______________________________________________
//             SHADOW STENCIL ATTRIBUTE CALLBACK
//--- Callback of Object
void ShadowStencilAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *beobj, void *arg)
{
    CK3dEntity *ent = (CK3dEntity *)beobj;
    CKParameterOut *pout = ent->GetAttributeParameter(AttribType);
    if (!pout)
        return;

    CKAttributeManager *attman = ent->GetCKContext()->GetAttributeManager();
    int stencilMeshAttribute = attman->GetAttributeTypeByName(shadowStencilCasterMeshAttributeName);

    if (Set)
    {
        // Attribute is Set

        for (int a = 0; a < ent->GetMeshCount(); ++a)
        {
            CKMesh *mesh = ent->GetMesh(a);
            if (!mesh)
                continue;
            mesh->SetAttribute(stencilMeshAttribute);
        }
    }
    else
    {
        // Attribute is Unset

        for (int a = 0; a < ent->GetMeshCount(); ++a)
        {
            CKMesh *mesh = ent->GetMesh(a);
            if (!mesh)
                continue;
            mesh->RemoveAttribute(stencilMeshAttribute);
        }
    }
}

//--- Callback of Mesh
void ShadowStencilMeshAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *beobj, void *arg)
{
    CKMesh *mesh = (CKMesh *)beobj;
    CKParameterOut *pout = mesh->GetAttributeParameter(AttribType);
    if (!pout)
        return;

    if (Set)
    {
        // Attribute is Set

        // build the structure
        NeigFaceStructure *nfs = new NeigFaceStructure;
        nfs->BuildNeigFaceStruct(mesh, 0.01f);

        pout->SetValue(&nfs);
    }
    else
    {
        // Attribute is Unset

        NeigFaceStructure *nfs = NULL;
        pout->GetValue(&nfs, FALSE);

        delete nfs;
    }
}

/***********************************************/
//	PreRender Callback
/***********************************************/
void ShadowStencilPreRender(CKRenderContext *renderContext, void *arg)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(renderContext->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return;
    // CKBehavior *beh = (CKBehavior *)arg;
    CKContext *ctx = renderContext->GetCKContext();

    // Get Attribute List
    CKAttributeManager *attman = ctx->GetAttributeManager();
    const int shadowStencilAttribute = attman->GetAttributeTypeByName(shadowStencilCasterAttributeName);
    const XObjectPointerArray &stencilShadowCasters = attman->GetAttributeListPtr(shadowStencilAttribute);

    // exit if there's no shadow stencil objects
    if (!stencilShadowCasters.Size())
        return;

    CK3dEntity *obj;
    CKMesh *mesh;
    CKObject **it;
    CKParameterOut *pout;
    NeigFaceStructure *nfs;

    const int stencilMeshAttribute = attman->GetAttributeTypeByName(shadowStencilCasterMeshAttributeName);

    // parse all objects, and see if the topology of their mesh has changed
    // if so, flag them, so they'll the face orientation test will be called
    // even if light didn't moved
    for (it = stencilShadowCasters.Begin(); it != stencilShadowCasters.End(); ++it)
    {
        // get the object
        obj = (CK3dEntity *)*it;
        if (!obj)
            continue;

        // only for visible objects
        if (!obj->IsVisible())
            continue;

        // get the mesh
        mesh = obj->GetCurrentMesh(); // we should normally parse ALL the object's meshes
        if (!mesh)
            continue;

        if (!(mesh->GetFlags() & VXMESH_OPTIMIZED) || (mesh->GetFlags() & VXMESH_POS_CHANGED) || obj->GetSkin())
        {
            // get the neig face structure
            pout = mesh->GetAttributeParameter(stencilMeshAttribute);
            if (!pout)
            {
                mesh->SetAttribute(stencilMeshAttribute);
                pout = mesh->GetAttributeParameter(stencilMeshAttribute);
            }

            nfs = NULL;
            pout->GetValue(&nfs, FALSE);
            if (!nfs)
                continue; // theoretically impossible.

            nfs->m_Flag |= NFS_MESHCHANGE; // needed because the VXMESH_POS_CHANGED is reset after rendering by the render engine
        }
    }
}

/***********************************************/
//	PostRender Callback
/***********************************************/
void ShadowStencilPostRender(CKRenderContext *renderContext, void *arg)
{
    //	renderContext->m_Context->UserProfileStart(0);

    CKBehavior *beh = (CKBehavior *)CKGetObject(renderContext->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return;
    // CKBehavior *beh = (CKBehavior *)arg;
    CKContext *ctx = renderContext->GetCKContext();

    // Get Attribute List
    CKAttributeManager *attman = ctx->GetAttributeManager();
    const int shadowStencilAttribute = attman->GetAttributeTypeByName(shadowStencilCasterAttributeName);
    const XObjectPointerArray &stencilShadowCasters = attman->GetAttributeListPtr(shadowStencilAttribute);

    // exit if there's no shadow stencil objects
    if (!stencilShadowCasters.Size())
        return;

    // get light
    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return;

    // get light position in world
    VxVector worldPosLight, objPosLight;
    light->GetPosition(&worldPosLight);

    // get camera
    CK3dEntity *cam = renderContext->GetAttachedCamera();

    // get camera position in world
    VxVector worldPosCam, objPosCam;
    float nearClip = 0.0f;

    //--- If there's an attached camera, add the near clip distance to camera's position
    if (cam)
    {
        cam->GetPosition(&worldPosCam);

        VxVector worldCamDir = cam->GetWorldMatrix()[2];
        worldCamDir.Normalize();
        worldPosCam += worldCamDir * ((CKCamera *)cam)->GetFrontPlane();
    }
    ///// Otherwise simply take viewport's position
    else
    {
        cam = renderContext->GetViewpoint();
        cam->GetPosition(&worldPosCam);
    }

    // get shadow range factor
    float shadowRangeFactor = 1.0f;
    beh->GetInputParameterValue(0, &shadowRangeFactor);

    int fcount, vcount;

    // show shadow volume ?
    CKBOOL showVolumes = FALSE;
    beh->GetInputParameterValue(2, &showVolumes);

    // camera can penetrate shadow ?
    CKBOOL cameraCanPenetrateShadow = FALSE;
    beh->GetInputParameterValue(3, &cameraCanPenetrateShadow);

    // prepare render context states
    CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(1);
    if (!material)
        return;

    const VxColor *colPtr = NULL;

    if (showVolumes)
    {
        //--- Show Shadow Volumes

        material->SetAsCurrent(renderContext, FALSE);
        colPtr = &material->GetDiffuse();
    }
    else
    {
        //--- Render in Stencil Buffer

        // Set the render State
        renderContext->SetTexture(0);
        renderContext->SetState(VXRENDERSTATE_FILLMODE, VXFILL_SOLID);
        renderContext->SetState(VXRENDERSTATE_LIGHTING, FALSE);
        renderContext->SetState(VXRENDERSTATE_SHADEMODE, VXSHADE_FLAT);
        renderContext->SetState(VXRENDERSTATE_ZENABLE, TRUE);
        renderContext->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
        renderContext->SetState(VXRENDERSTATE_ZFUNC, VXCMP_LESSEQUAL);
        renderContext->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
        renderContext->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ZERO);
        renderContext->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ONE);
        renderContext->SetState(VXRENDERSTATE_ALPHATESTENABLE, FALSE);

        renderContext->SetState(VXRENDERSTATE_STENCILENABLE, TRUE);
        renderContext->SetState(VXRENDERSTATE_STENCILMASK, 0xff);
        renderContext->SetState(VXRENDERSTATE_STENCILWRITEMASK, 0xff);
        renderContext->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_ALWAYS);
        renderContext->SetState(VXRENDERSTATE_STENCILFAIL, VXSTENCILOP_KEEP);
        renderContext->SetState(VXRENDERSTATE_STENCILZFAIL, VXSTENCILOP_KEEP);
    }

    CK3dEntity *obj;
    CKMesh *mesh;
    CKParameterOut *pout;
    NeigFaceStructure *nfs;
    CKObject **it;

    const int stencilMeshAttribute = attman->GetAttributeTypeByName(shadowStencilCasterMeshAttributeName);

    //--- If Camera Can Penetrate Shadow
    //--- Parse All Objects with attribute "Stencil Shadow"
    //--- to get the Light Occluders Count
    int lightCasterCount = 0;
    if (cameraCanPenetrateShadow)
    {
        for (it = stencilShadowCasters.Begin(); it != stencilShadowCasters.End(); ++it)
        {
            // get the object
            obj = (CK3dEntity *)*it;
            if (!obj)
                continue;

            // only for visible objects
            if (!obj->IsVisible())
                continue;

            // get light position in object referential
            obj->InverseTransform(&objPosLight, &worldPosLight);

            // get camera position in object referential
            obj->InverseTransform(&objPosCam, &worldPosCam);

            lightCasterCount += LightIsHiddenByObject(&objPosLight, &objPosCam, obj);
        }

        //--- if camera is inside some shadow volumes,
        //--- we must clear the stencil buffer with N
        if (lightCasterCount)
        {
            renderContext->Clear(CK_RENDER_CLEARSTENCIL, lightCasterCount);
        }
        else
        {
            renderContext->Clear(CK_RENDER_CLEARSTENCIL);
        }
    }
    else
    {
        renderContext->Clear(CK_RENDER_CLEARSTENCIL);
    }

    beh->SetOutputParameterValue(0, &lightCasterCount);

    // get an instance of the Shape Renderer
    ShapeRenderer sr(renderContext, shadowRangeFactor);

    //--- Parse All Objects with attribute "Stencil Shadow"
    //--- fill their current mesh's Attribute Info (FCount, VCount, Flags...)
    //--- and Draw Their Shadow Volume
    for (it = stencilShadowCasters.Begin(); it != stencilShadowCasters.End(); ++it)
    {
        // get the object
        obj = (CK3dEntity *)*it;
        if (!obj)
            continue;

        // only for visible objects
        if (!obj->IsVisible())
            continue;

        // get the mesh
        mesh = obj->GetCurrentMesh();
        if (!mesh)
            continue;

        // get the neig face structure
        pout = mesh->GetAttributeParameter(stencilMeshAttribute);
        if (!pout)
        {
            mesh->SetAttribute(stencilMeshAttribute);
            pout = mesh->GetAttributeParameter(stencilMeshAttribute);
        }

        nfs = NULL;
        pout->GetValue(&nfs, FALSE);
        if (!nfs)
            continue; // theorically impossible.

        // get face count
        fcount = mesh->GetFaceCount();

        // get vertex count
        vcount = mesh->GetVertexCount();

        // get light position in object referential
        obj->InverseTransform(&objPosLight, &worldPosLight);

        // if the mesh as change since the Edge Structure has been calculated
        if ((fcount != nfs->m_FCount) || (vcount != nfs->m_VCount))
        {
            nfs->BuildNeigFaceStruct(mesh, 0.01f);
            nfs->m_LightPos.x = objPosLight.x + 1.0f; // to force Flags calculation
        }

        if ((nfs->m_LightPos != objPosLight) || (nfs->m_Flag & NFS_MESHCHANGE))
        {
            // parse all faces (that need to be parsed)
            // and decide if they are front or back
            nfs->ParseFacesAndFlagThem(mesh, objPosLight);
            nfs->m_Flag &= ~NFS_MESHCHANGE;
        }

        // prepare to render
        sr.PrepareToRender(nfs->m_VCount * 2, nfs->m_FCount * 18, obj, &objPosLight);

        // parse faces and fill Draw Primitives
        sr.ParseFacesAndFillDrawPrimitives(nfs);

        // Draw Shadow
        sr.DrawShadow(colPtr);
    }

    //--- Render The Black Plane in Front of the Camera
    //---
    if (!showVolumes)
    {
        // renderContext->DumpToFile("D:\\Toto.bmp", NULL, VXBUFFER_STENCILBUFFER);

        material->SetAsCurrent(renderContext, FALSE);

        renderContext->SetState(VXRENDERSTATE_ZENABLE, FALSE);

        // Render Only If Stencil == 1
        renderContext->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_KEEP);
        renderContext->SetState(VXRENDERSTATE_STENCILREF, 0);
        renderContext->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_NOTEQUAL);

        //--- Render Black Rectangle
        CKDWORD dwcol = material->GetDiffuse().GetRGBA();
        sr.DrawViewRect(&dwcol);
        //---

        // Restore Some Flags
        renderContext->SetState(VXRENDERSTATE_STENCILENABLE, FALSE);
        renderContext->SetState(VXRENDERSTATE_ZENABLE, TRUE);
    }

    //	renderContext->m_Context->UserProfileEnd(0);
}

/**********************************************/
/*				FUNCTION
/**********************************************/
int ShadowStencil(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    if (beh->IsInputActive(1))
    {
        // enter by OFF
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        // enter by ON
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    CKRenderManager *rm = behcontext.Context->GetRenderManager();
    if (!rm)
        return CKBR_ACTIVATENEXTFRAME;

    CKRenderContext *rc = rm->GetRenderContext(0);
    if (!rc)
        return CKBR_ACTIVATENEXTFRAME;

    int bppStencil = 0;
    rc->GetPixelFormat(NULL, NULL, &bppStencil);

    if (!bppStencil)
        return CKBR_ACTIVATENEXTFRAME;

    behcontext.CurrentRenderContext->AddPostRenderCallBack(ShadowStencilPostRender, (void *)beh->GetID(), TRUE, TRUE);
    behcontext.CurrentRenderContext->AddPreRenderCallBack(ShadowStencilPreRender, (void *)beh->GetID(), TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

/**********************************************/
/*				CALLBACK
/**********************************************/
