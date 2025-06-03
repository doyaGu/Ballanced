#ifndef PortalsMANAGER_H
#define PortalsMANAGER_H

#include "VxMath.h"
#include "XObjectArray.h"
#include "CKBaseManager.h"

#define PORTALS_MANAGER_GUID CKGUID(0x2af20fe2, 0x3c0570a7)

// This class is exported from the RadiosityManager.dll
class PortalsManager : public CKBaseManager
{
public:
    ///
    // Methods

    // Ctor
    PortalsManager(CKContext *ctx);
    // Dtor
    ~PortalsManager();

    CKERROR OnCKInit();
    CKERROR PostClearAll() { return OnCKInit(); }
    CKERROR OnPreRender(CKRenderContext *dev);
    CKERROR OnCKReset();
    CKERROR OnCKPause();
    CKERROR PostLoad();

    CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnPreRender |
               CKMANAGER_FUNC_OnCKReset |
               CKMANAGER_FUNC_OnCKPause |
               CKMANAGER_FUNC_PostLoad |
               CKMANAGER_FUNC_PostClearAll;
    }

    // Cameras
    void SetDebugCameras(CKGroup *c) { m_DebugCameras = c; }
    CKGroup *GetDebugCameras() { return m_DebugCameras; }

    // Behavior
    void SetBehavior(CKBehavior *g) { m_Behavior = g; }
    CKBehavior *GetBehavior() { return m_Behavior; }

    // Recursivity Level
    void SetRecursivityLevel(int r) { m_RecursivityLevel = r; }
    int GetRecursivityLevel() { return m_RecursivityLevel; }

    // Activity
    void Activate(CKBOOL a);
    CKBOOL IsActive() { return m_Active; }

    void ShowPlacesFrom(CKCamera *cam);
    void RecursePlacesFrom(int level, CKPlace *p, VxRect currentextents);

    // Automatically Generates All Portals
    virtual void AutomaticallyGeneratePortals();

protected:
    // Structs
    class Occluder
    {
    public:
        // Ctor : construct the occluder Hull form the mesh
        Occluder(CKMesh *iMesh);

        // Render the convex hull
        void RenderHull(CKRenderContext *iRC, CK3dEntity *iEntity) const;
        // Render the silhouette
        void RenderSilhouette(CKRenderContext *iRC, const XArray<VxVector> &iSilhouetteEdges) const;
        // Construct Silhouette
        void ComputeSilhouette(CK3dEntity *iEntity, const VxFrustum &iFrustum, XArray<VxVector> &oSilhouettesVertices, XArray<VxPlane> &oOccludingVolume);

        // test if a vertices soup is planar
        CKBOOL IsPlanar(XPtrStrided<VxVector> iPositions, const int iCount);
        // Compute the Hull of a plane
        void ComputePlanarHull(CKMesh *iMesh);

    protected:
        // structures
        struct Face
        {
            // Default Ctor
            Face() : vcount(0), vertices(NULL), ecount(0), edges(NULL) {}

            // Copy Ctor
            Face(const Face &iA)
            {
                vertices = NULL;
                edges = NULL;

                PrepareVertices(iA.vcount);
                if (vcount)
                    memcpy(vertices, iA.vertices, vcount * sizeof(CKDWORD));
                PrepareEdges(iA.ecount);
                if (ecount)
                    memcpy(edges, iA.edges, ecount * sizeof(CKDWORD));

                normal = iA.normal;
            }

            // allocation with a vertex count
            void PrepareVertices(int iSize)
            {
                delete[] vertices;

                vcount = iSize;
                if (vcount)
                    vertices = new CKDWORD[iSize];
                else
                    vertices = NULL;
            }

            // allocation with a vertex count
            void PrepareEdges(int iSize)
            {
                delete[] edges;

                ecount = iSize;
                if (ecount)
                    edges = new CKDWORD[iSize];
                else
                    edges = NULL;
            }

            // Dtor
            ~Face()
            {
                delete[] vertices;
                delete[] edges;
            }

            // Copy operator
            Face &operator=(const Face &iA)
            {
                PrepareVertices(iA.vcount);
                if (vcount)
                    memcpy(vertices, iA.vertices, vcount * sizeof(CKDWORD));
                PrepareEdges(iA.ecount);
                if (ecount)
                    memcpy(edges, iA.edges, ecount * sizeof(CKDWORD));
                normal = iA.normal;
                return *this;
            }

            ///
            // Members

            int vcount;
            CKDWORD *vertices;
            int ecount;
            CKDWORD *edges;
            VxVector normal;
        };

        struct Edge
        {
            CKDWORD vertices[2];
            VxVector normal;
            CKDWORD references;

            bool operator==(const Edge &iA)
            {
                return (!operator==(iA));
            }

            bool operator!=(const Edge &iA)
            {
                return ((vertices[0] != iA.vertices[0]) || (vertices[1] != iA.vertices[1]));
            }
        };

        // array of faces
        XClassArray<Face> m_HullFaces;
        // array of vertices
        XArray<VxVector> m_HullVertices;
        // array of edges
        XArray<Edge> m_HullEdges;

        // array of silhouette edges
        XArray<Edge> m_SilhouetteEdges;

        // Is the occluder a plane ?
        CKBOOL m_Plane;
    };

    // types
    typedef XHashTable<Occluder *, CKMesh *, XHashFun<void *>> HMesh2Occluder;

    // Occluders functions
    Occluder *RegisterOccluder(CK3dEntity *iEntity);

    void ShowHidedLastFrame();
    void ConstructOccludingVolume(CK3dEntity *iEntity, XArray<VxPlane> &oVolume, const VxFrustum &iFrustum);
    bool IsObjectOccluded(CK3dEntity *iEntity, const XArray<VxPlane> &iVolume);
    void ComputeConvexHull(Occluder &ioOccluder);
    void RenderOccluders(CKRenderContext *iRC);
    void ManageOccluders(CKRenderContext *iRC);
    void ClearOccluders();

    ///
    // Members

    // Activity
    CKBOOL m_Active;
    // RecursivityLevel (0 means infinity)
    int m_RecursivityLevel;
    // View Points to cull from (if none, use Current ViewPoint)
    CKGroup *m_DebugCameras;
    // Behavior to write the current place
    CKBehavior *m_Behavior;

    // Occluders
    HMesh2Occluder m_Mesh2Occluder;
    int m_OccludersAttribute;
    XObjectPointerArray m_HidedLastFrame;
    XArray<VxVector> m_SilhouetteEdges;
    XArray<VxPlane> m_OccludingVolume;

    // Temp Data
    XArray<CKPlace *> m_CallerStack;

    // Projection Matrix for the Current Camera
    VxMatrix m_ViewProjectionMatrix;

    XObjectArray *m_RenderedPlaces;
    XObjectArray *m_RenderedTransPlaceObjs;

    // group of Trans Place object specified
    // in the building block Portal Management
    CKGroup *m_TransPlaceObj;
};

#endif