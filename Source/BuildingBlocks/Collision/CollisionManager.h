// CollisionManager.cpp : Defines the entry point for the DLL application.
//
#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "CKCollisionManager.h"

#if _MSC_VER > 1000
#pragma warning(disable : 4786)
#endif

#include "BoundariesManager2.h"

// {secret}
typedef struct
{
    CK_ID m_Entity;
    VxMatrix m_WorldMatrix;
    VxBbox m_LastWorldBox;
} SavedMovingEntity;

/*************************************************
Name: CollisionManager

Summary: Collision management

Remarks:
For its main part, the collision manager lies on the obstacle attributes.
These are of two types :
- Fixed Obstacle
    defining the obstacle which not move from one frame to another
- Moving Obstacle
    defining the objects susceptible to move during the processing
For these two attributes, You can precise the geometry precision
(for now, only Bounding Box and Faces) and if you want to take into account
the children of the obstacle, the hierarchy flags must be checked.

You can add this attributes the normal way, by adding the beobject the attribute
or by using the devoted collision manager functions : AddObstacle, AddObstacleByNames
and remove them the same way.
You can also iterate among the obstacles with the GetObstacle(fixed/moving)() functions.

The main function of the Collision Manager is the DetectCollision()
The goal of this function is to find if the given object (which must be marked
as an obstacle) is in collision with another obstacle. It stops with the first obstacle found
and can provide several impact information if the user wants them.
The ImpactDesc is as follow :

  typedef struct {
    CK_ID	m_OwnerEntity;  // Child of the object tested or the object itself (can be the body part of a character)
    CK_ID	m_EntityTouched; // Precise entity touched, a child of one obstacle, or the obstacle itself (can be the body part of a character)
    CK_ID	m_ObstacleTouched; // Object with an attribute obstacle touched
    int		m_TouchedVertex; // NOT USED YET
    int		m_TouchingVertex; // NOT USED YET
    int		m_TouchedFace; // NOT USED YET
    int		m_TouchingFace; // NOT USED YET
    VxMatrix m_ImpactWorldMatrix; // a World Matrix describing the position of the tested object before it touched an obstacle
    VxVector m_ImpactPoint; // NOT USED YET
    VxVector m_ImpactNormal; // NOT USED YET
} ImpactDesc;

The collision manager keeps all the objects marked as obstacle in sorted arrays
which allow it to find quickly which objects are overlapping with another one.
The DetectCollision() method use this functionnality to restrict the number
of complex intersection tests to perform.

One of the other functions set provided by the collision manager is the RayIntersection set.
These are three functions :
RayIntersection() testing the ray with all the obstacles
FixedRayIntersection() testing the ray with only the fixed obstacles
MovingRayIntersection() testing the ray with only the moving obstacles

The collision manager gives also access to intersection tests between two entities,
at different precision level. These functions are :
CKBOOL BoxBoxIntersection(ent1,ent2);
CKBOOL BoxFaceIntersection(ent1,ent2);
CKBOOL FaceFaceIntersection(ent1,ent2);
These three functions operate at single entity level. For hierarchical detection, use :
IsInCollisionWithHierarchy()
IsHierarchyInCollisionWithHierarchy()
which operate on hierarchy. The precision level is given as argument
(for now, only CKCOLLISION_BOX and CKCOLLISION_FACE)

Finally, the collision manager gives basic geometric tests functions such as :
SegmentBoxIntersection();
SegmentFaceIntersection();
FaceFaceIntersection();
BoxBoxIntersection();

The unique instance of CollisionManager can be retrieved by calling CKGetCollisionManager().


{Group:External Managers classes}

See also: CKFloorManager, CKAttributeManager
*************************************************/
class CollisionManager : public CKCollisionManager
{
public:
    //------------------------------------------------------------
    // Obstacles Management

    // Obstacle adding
    // for all these functions, moving descripe the nature of the obstacle, precision the nature of the geometry to consider
    // when testing for intersection and hierarchy the nature of the obstacle, entity alone or with all its hierarchy.
    virtual void AddObstacle(CK3dEntity *ent, CKBOOL moving = FALSE, CK_GEOMETRICPRECISION precision = CKCOLLISION_BOX, CKBOOL hiera = FALSE);
    virtual int AddObstaclesByName(CKLevel *level, CKSTRING substring, CKBOOL moving = FALSE, CK_GEOMETRICPRECISION precision = CKCOLLISION_BOX, CKBOOL hiera = FALSE);

    // obstacle removing
    virtual void RemoveObstacle(CK3dEntity *ent);
    virtual void RemoveAllObstacles(CKBOOL level = TRUE);

    virtual CKBOOL IsObstacle(CK3dEntity *ent, CKBOOL moving = FALSE);

    //------------------------------------------------------------
    // Obstacles Access

    // Access to the fixed obstacles of the current scene
    virtual int GetFixedObstacleCount(CKBOOL level = FALSE);
    virtual CK3dEntity *GetFixedObstacle(int pos, CKBOOL level = FALSE);

    // Access to the moving obstacles of the current scene
    virtual int GetMovingObstacleCount(CKBOOL level = FALSE);
    virtual CK3dEntity *GetMovingObstacle(int pos, CKBOOL level = FALSE);

    // Access to all types of obstacles of the current scene
    virtual int GetObstacleCount(CKBOOL level = FALSE);
    virtual CK3dEntity *GetObstacle(int pos, CKBOOL level = FALSE);

    //----------------------------------------------------------------
    // Collision Detection functions
    //------------------------------
    // all these functions use the defined obstacles, fixed or moving
    // or the two at once if not precised in the fuction name

    virtual CKBOOL DetectCollision(CK3dEntity *ent, CK_GEOMETRICPRECISION precis_level, int replacementPrecision, int detectionPrecision, CK_IMPACTINFO inmpactFlags, ImpactDesc *imp);
    // Detection of an obstacle between two points
    virtual CK3dEntity *ObstacleBetween(const VxVector &pos, const VxVector &endpos, CKBOOL iFace = TRUE, CKBOOL iFirstContact = FALSE, CKBOOL iIgnoreAlpha = FALSE, VxIntersectionDesc *oDesc = NULL);
    virtual CKBOOL ObstacleBetween(const VxVector &pos, const VxVector &endpos, float width, float height);

    // Detect all collisions at a specific moment
    const XArray<ImpactDesc> &DetectCollision(CK_GEOMETRICPRECISION precis_level, CK_IMPACTINFO inmpactFlags);
    const XArray<ImpactDesc> &GetImpactArray();

    //-------------------------------------------------------------
    // Advanced intersection test functions
    //----------------------------------
    // These functions test the static intersections between
    // two 3dEntity. They operate at a certain level of
    // geometry precision : Box-box, Box-Face and Face-Face

    // Ultimate collision between two 3d Entity, bounding box precision
    virtual CKBOOL BoxBoxIntersection(CK3dEntity *ent1, CKBOOL hiera1, CKBOOL local1, CK3dEntity *ent2, CKBOOL hiera2, CKBOOL local2);
    // Collision between two 3d Entity, bounding box precision
    virtual CKBOOL BoxFaceIntersection(CK3dEntity *ent1, CKBOOL hiera1, CKBOOL local1, CK3dEntity *ent2);
    // Collision between two 3d Entity ,at faces precision
    virtual CKBOOL FaceFaceIntersection(CK3dEntity *ent1, CK3dEntity *ent2);
    // Collision at fixed precision level between two 3d Entities
    virtual CKBOOL IsInCollision(CK3dEntity *ent, CK_GEOMETRICPRECISION precis_level1, CK3dEntity *ent2, CK_GEOMETRICPRECISION precis_level2);
    // Collision at fixed precision level between a 3dEntity and a hierarchy of 3d Entity
    virtual CK3dEntity *IsInCollisionWithHierarchy(CK3dEntity *ent, CK_GEOMETRICPRECISION precis_level1, CK3dEntity *ent2, CK_GEOMETRICPRECISION precis_level2);
    // Collision at fixed precision level between two hierarchies
    virtual CKBOOL IsHierarchyInCollisionWithHierarchy(CK3dEntity *ent, CK_GEOMETRICPRECISION precis_level1, CK3dEntity *ent2, CK_GEOMETRICPRECISION precis_level2, CK3dEntity **sub, CK3dEntity **subob);

    // Callbacks
    virtual CKERROR OnCKInit();
    virtual CKERROR OnCKEnd()
    {
        Clear();
        return CK_OK;
    }
    virtual CKERROR OnCKReset();
    virtual CKERROR OnCKPlay();
    // virtual CKERROR OnPostCopy(CKDependenciesContext& iContext);
    virtual CKERROR PostProcess();
    virtual CKERROR PostLaunchScene(CKScene *OldScene, CKScene *NewScene);
    virtual CKERROR PreClearAll()
    {
        Clear();
        return OnCKInit();
    }

    virtual CKERROR PostLoad();

    virtual CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_PreClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKReset |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_PostLaunchScene |
               CKMANAGER_FUNC_PostLoad |
               CKMANAGER_FUNC_PostProcess;
    }
    ////////////////////////////////////////////////////////
    ////               Private Part                     ////
    ////////////////////////////////////////////////////////

    //{secret}
    CollisionManager(CKContext *Context);
    //{secret}
    ~CollisionManager();

    //{secret}
    static CollisionManager *Cast(CKBaseManager *iM) { return (CollisionManager *)iM; }

    // must be call in the Resume sequence (in case the user moved the object while in author mode)
    // {secret}
    void Clear();

    //{secret}
    int GetNearestFace(CK3dEntity *ent1, CK3dEntity *ent2);
    //{secret}
    CKBOOL GetNearestVertex(CK3dEntity *ent1, CK3dEntity *ent2, VxVector &pos);
    //{secret}
    int GetNearestVertex(CK3dEntity *ent, VxVector &pos);
    //{secret}
    int GetNearestFace(CK3dEntity *ent, VxVector &pos);

    //{secret}
    BoundariesManager &GetBoundariesManager();

    //{secret}
    CKBOOL TestPair(CK_GEOMETRICPRECISION iPrecision, CK3dEntity *iEntity1, CK3dEntity *iEntity2, CK_IMPACTINFO iImpactFlags, ImpactDesc &oImpact);
    //{secret}
    void FillImpactStructure(CK_IMPACTINFO inmpactFlags, ImpactDesc *imp, CK3dEntity *ent, CK3dEntity *subent, CK3dEntity *ob, CK3dEntity *subob, const VxMatrix &replacementMatrix);

    // Moving Entities Matrix Management
    //{secret}
    void AddMovingMatrix(CK3dEntity *ent);
    //{secret}
    void RemoveMovingMatrix(CK3dEntity *ent);
    //{secret}
    int SeekMovingMatrix(CK3dEntity *ent);
    //{secret}
    void TestAndReallocateMovingMatrix(int newSize);
    //{secret}
    // void	RadiusTest(CK3dEntity* ent,XArray<CKObject*>* array);
    //{secret}
    CKBOOL ReadAttributeValues(CK3dEntity *ent, CK_GEOMETRICPRECISION &geomtype, CKBOOL &hiera);
    //{secret}
    CKBOOL WriteAttributeValues(CK3dEntity *ent, CKBOOL moving, CK_GEOMETRICPRECISION geomtype, CKBOOL hiera);
    //{secret}
    int m_FixedAttribute;
    //{secret}
    int m_MovingAttribute;
    //{secret}
    CKBOOL m_ObstaclesCopied;

protected:
    //{secret}
    BoundariesManager m_BoundariesManager;
    // array of moving entities orientations and positions
    //{secret}
    SavedMovingEntity *m_MovingEntitiesMatrix;
    //{secret}
    int m_MovingEntitiesCount;
    //{secret}
    int m_MovingEntitiesMaxSize;
    // {secret}
    int m_TouchingFaceIndex;
    // {secret}
    int m_TouchedFaceIndex;

    // {secret}
    XArray<ImpactDesc> m_ImpactArray;

    // Temp for profiling
    int m_TestPerformed;
    int m_TestPassed;
};

#endif