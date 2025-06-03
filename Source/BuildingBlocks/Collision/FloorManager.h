/*************************************************************************/
/*	File : CKFloorManager.h					 		 					 */
/*	Author :  Aymeric Bard												 */
/*																	     */
/*	Virtools SDK 												         */
/*	Copyright (c) Virtools 2001, All Rights Reserved.					 */
/*************************************************************************/
#ifndef FLOORMANAGER_H
#define FLOORMANAGER_H "$Id:$"

#include "CKBaseManager.h"
#include "CKDefines.h"
#include "VxMath.h"

// {partOf:Pre-Registred Parameter Types}
#ifndef CKPGUID_FLOORGEOMETRY
#define CKPGUID_FLOORGEOMETRY CKDEFINEGUID(0x7c1930a1, 0x42361a45)
#endif
// {partOf:Pre-Registred Parameter Types}
#ifndef CKPGUID_FLOOR
#define CKPGUID_FLOOR CKDEFINEGUID(0x7b447672, 0x5798572a)
#endif

/*************************************************
Name: CKFloorManager

Summary: Floors management

Remarks:

The unique instance of CKFloorManager can be retrieved by calling CKContext::GetManagerByGUID(FLOOR_MANAGER_GUID).

{Group:Managers classes}
See also: CKCollisionManager, CKAttributeManager
*************************************************/
class FloorManager : public CKFloorManager
{
public:
    //------------------------------------------------------
    // returns the nearest floors below and above the point pos
    // and sets the offset between pos and floors in DownOffset & Upoffset
    // Normals at floor intersection can also be calculated
    virtual CK_FLOORNEAREST GetNearestFloors(const VxVector &iPosition, CKFloorPoint *oFP, CK3dEntity *iExcludeFloor = NULL);

    // constrain a point to a floor
    virtual CKBOOL ConstrainToFloor(const VxVector &iPosition, float iRadius, VxVector *oPosition, CKAttributeType iExcludeAttribute = -1);

    //-----------------------------------------------------
    // Angle(in degrees) above which faces are not considered as floors
    // by default 45 degrees
    virtual float GetLimitAngle();
    virtual void SetLimitAngle(float angle);

    //-----------------------------------------------------
    // Adding / Removing Floor Objects

    //----------------------------------------------
    // add objets as floor is their name contain floorname
    virtual int AddFloorObjectsByName(CKLevel *level, CKSTRING floorname, CK_FLOORGEOMETRY geo, CKBOOL moving, int type, CKBOOL hiera, CKBOOL first);
    virtual void AddFloorObject(CK3dEntity *ent, CK_FLOORGEOMETRY geo, CKBOOL moving, int type, CKBOOL hiera, CKBOOL first);
    virtual void RemoveFloorObject(CK3dEntity *ent);

    virtual int GetFloorObjectCount();
    virtual CK3dEntity *GetFloorObject(int pos);

    // Cache management
    virtual float GetCacheThreshold();
    virtual void SetCacheThreshold(float t);

    virtual int GetCacheSize();
    virtual void SetCacheSize(int size);

    //{secret}
    virtual CKBOOL ReadAttributeValues(CK3dEntity *ent, CKDWORD *geo = NULL, CKBOOL *moving = NULL, int *type = NULL, CKBOOL *hiera = NULL, CKBOOL *first = NULL);

    // floor attribute reading
    virtual int GetFloorAttribute() { return m_FloorAttribute; }

    // Constrain a moving point to a floor
    virtual CKBOOL ConstrainToFloor(const VxVector &iOldPosition, const VxVector &iPosition, float iRadius, VxVector *oPosition, CKAttributeType iExcludeAttribute = -1);

    virtual CKERROR PreClearAll();
    virtual CKERROR OnCKInit();
    virtual CKERROR OnCKPlay();
    virtual CKERROR PostLoad();
    virtual CKERROR SequenceToBeDeleted(CK_ID *iIDs, int iCount);
    virtual CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_PreClearAll |
               CKMANAGER_FUNC_PostLoad |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnSequenceToBeDeleted |
               CKMANAGER_FUNC_OnCKPlay;
    }

    virtual CK_FLOORNEAREST GetNearestFloor(const VxVector &iPosition, CK3dEntity **oFloor, int *oFaceIndex = NULL, VxVector *oNormal = NULL, float *oDistance = NULL, CK3dEntity *iExcludeFloor = NULL);

    //{secret}
    static FloorManager *Cast(CKBaseManager *iM) { return (FloorManager *)iM; }

    //{secret}
    FloorManager(CKContext *Context);
    //{secret}
    ~FloorManager();

protected:
    // temporary edge structure to use for the construction
    struct Edge
    {
        CKWORD a;
        CKWORD b;
        CKWORD fa;
        CKWORD fb;

        Edge() : a(-1), b(-1) {}

        Edge(CKWORD iA, CKWORD iB) : a(iA), b(iB) {}

        operator int() const
        {
            return a * b;
        }

        bool operator==(const Edge &iEdge) const
        {
            return ((a == iEdge.a) && (b == iEdge.b)) ||
                   ((a == iEdge.b) && (b == iEdge.a));
        }
    };

    // geometric edge
    struct GeomEdge
    {
        Vx2DVector normal;
        float D;
        Vx2DVector a;
        Vx2DVector b;
        float length;
        float invLength;
    };

    // array of edges
    typedef XArray<GeomEdge> EdgesArray;

    typedef XHashTable<EdgesArray, CKMesh *, XHashFun<void *>> HashFloor2Edges;

    // Constraint functions
    void ComputeEdges(CK3dEntity *iFloor, XArray<GeomEdge> &oEdges);
    CKBOOL SlideOnEdges(VxVector &ioPosition, float iRadius, const XArray<GeomEdge> &iEdges, CK3dEntity *iFloor, CKAttributeType iExcludeAttribute = -1);

    // detect floors functions
    CKBOOL FloorRayIntersect(CK3dEntity *mov, CKMesh *mesh, const VxBbox &rayextents, CKBOOL first, int x, int z, const VxRay &ray, int facetotestfirst, int lastfacetotest, float &tmax, int &face, VxVector &inter);
    CKBOOL TestFixedFloor(CK3dEntity *mov, const VxVector &pos, VxVector &inter, VxVector &normal, CKBOOL first, int &facetouched, int facetotestfirst = 0);
    CKBOOL TestMovingFloor(CK3dEntity *mov, const VxVector &pos, VxVector &inter, VxVector &normal, CKBOOL first, int &facetouched, int facetotestfirst = 0);
    CKBOOL TestBoxFloor(CK3dEntity *mov, const VxVector &pos, VxVector &inter, VxVector &normal);
    CKBOOL FloorFastRejection(CK3dEntity *mov, const VxVector &pos, float nearestdown, float nearestup);

    float m_LimitAngle;
    float m_CosAngle;
    int m_FloorAttribute;
    float m_VerticalMatrix[3][3];

    float m_CacheThreshold;
    int m_CacheSize;

    HashFloor2Edges m_FloorEdges;
};
#endif
