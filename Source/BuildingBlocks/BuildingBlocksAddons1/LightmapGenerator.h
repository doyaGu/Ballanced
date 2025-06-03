#pragma once

#include "PolyhedronConstructor.h"

#define MAX_LIGHT 256
//#define USEMT

class LightmapGenerator
{
public:
    LightmapGenerator(CKContext *);
    ~LightmapGenerator();

    // Clean Up Lightmaps
    void Clean(CKBOOL iDestroyTextures = TRUE);

    // save to a chunk
    void Save(CKStateChunk *chunk);

    // load from a chunk
    void Load(CKStateChunk *chunk);

    // mark the texture as private or not (for saving purpose)
    void MarkTextureAsPrivate(CKBOOL priv);

    // world information functiosn : call these before Generate !
    void SetReceivers(CKGroup *r, int Start = 0, int Count = -1);
    void SetOccluders(CKGroup *);
    void SetLights(CKGroup *);

    // Lightmap Generation function : does all the job in here !
    void Generate(float Density, float Threshold, int SuperSampling, int Blur);

    // Lightmapped Object creation
    void GenerateObjects(CKContext *iCtx);

    // Set the render callback on the lightmapped objects
    void AttachRenderCallbacks();
    void RemoveRenderCallbacks();
    void ShowPolygons(CKBOOL s) { m_ShowPolygons = s; }

    // rendering function
    static int LightMapsRender(CKRenderContext *dev, CKRenderObject *ent, void *arg);
    static void LightMapsRenderDebug(CKRenderContext *dev, void *arg);

    void SetOpacity(const CKDWORD opacity) { m_Opacity = opacity; }

    void PackAllTextures();

protected:
    // Vertex Structure
    struct LightVertex
    {
        VxVector position;
        CKDWORD diffuse;
        CKDWORD specular;
        VxUV uv;
        CKDWORD index;
    };
    typedef XArray<LightVertex> tLightVertexArray;

    // Structure binding faces with a lightmap
    struct LightMapTexture
    {
        CKTexture *texture;
        tLightVertexArray vertices;
        XArray<CKWORD> indices;
        XArray<CKWORD> originalface;
    };
    typedef XArray<LightMapTexture *> tLightMapArray;

    // structure binding one object with all its lightmaps
    struct LightMapFaces
    {
        CK_ID object;
        tLightMapArray textures;
    };
    typedef XArray<LightMapFaces *> tLightMappedObjectArray;

    ///
    // Methods

    static int TextureSort(const void *elem1, const void *elem2);
    void ExtendBorderTexture(CKTexture *tex);
    void DuplicateBorderTexture(CKTexture *tex);
    void PackTextures(LightMapFaces &objectmap);
    CKBOOL ComputeTransparency(const CK3dEntity &ent, const VxIntersectionDesc &desc, VxColor &trans);
    CKBOOL ComputeVisibility(CK3dEntity *caster, const VxVector &start, const VxVector &dest, VxColor &trans);
    void ComputeLightmapFlat(const CKDWORD *pixelmap, const int twidth, const int theight);
    void ComputeLightmapSmooth(const CKDWORD *pixelmap, const int twidth, const int theight);
    void ComputePointColor(VxColor &lumcolor, const VxVector &worldPosition, const VxVector &worldNormal);
    CKContext *GetCKContext() { return m_Context; }

    ///
    // Members

    CKContext *m_Context;

    // Light mapped objects
    tLightMappedObjectArray m_Objects;

    // Receivers
    XObjectPointerArray m_Receivers;
    XObjectPointerArray m_Occluders;
    XObjectPointerArray m_FilteredOccluders[MAX_LIGHT];
    XObjectPointerArray m_Lights;
    XObjectPointerArray m_FilteredLights;

    // Current Light Index
    int m_CurrentLight;

    // Rendering Method
    CKBOOL m_ShowPolygons;

    // Profiling
    int m_RayBoxPerformed;
    int m_RayIntPerformed;

    // Temporary Data for localized functions
    PolyhedronConstructor *m_PolygonConstructor;
    int m_PolygonIndex;
    int m_MajorX;
    int m_MajorY;
    VxVector m_BoxWorldVertices[4];
    VxVector m_PolygonWorldVertices[4];
    CK3dEntity *m_CurrentEntity;
    CKMesh *m_CurrentMesh;

    VxColor m_AmbientColor;

    int m_SuperSampling;
    float m_InverseSampling;

    VxPlane m_PolygonPlane;

    CKDWORD m_PositionStride;
    CKBYTE *m_PositionsPtr;

    CKDWORD m_NormalStride;
    CKBYTE *m_NormalsPtr;

    CKDWORD m_Opacity;

#ifdef POINTDEBUG
    // Debugging array
    struct DebugPoint
    {
        DebugPoint() {}
        DebugPoint(const VxVector &iPos, CKBOOL iHit) : pos(iPos), hit(iHit) {}
        VxVector pos;
        CKBOOL hit;
    };

    XArray<DebugPoint> m_DebugPoints;
#endif
};
