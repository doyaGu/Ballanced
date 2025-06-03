// CollisionManager.cpp : Defines the entry point for the DLL application.
//
#include "CKAll.h"
#include "CollisionManager.h"

// {secret}
extern const char *CollisionManagerName;

// {secret}
const char *FixedObstacleName = "Fixed Obstacle";

// {secret}
const char *MovingObstacleName = "Moving Obstacle";

// {secret}
void MovingObstacleAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg)
{
    CollisionManager *cm = (CollisionManager *)arg;
    BoundariesManager &bm = cm->GetBoundariesManager();
    if (Set)
    {
        // we test if the BeObject had the Fixed Attribute
        if (obj->HasAttribute(cm->m_FixedAttribute))
        {
            obj->RemoveAttribute(cm->m_FixedAttribute);
            // and We add it as a moving obstacle
            bm.Add((CK3dEntity *)obj, TRUE);
        }
        else
        {
            // We add simply the object to the CollisionManager
            bm.Add((CK3dEntity *)obj, TRUE);
        }
    }
    else
    {
        CKContext *ctx = obj->GetCKContext();
        if (!ctx->IsInClearAll())
            bm.Remove((CK3dEntity *)obj);
    }
}

// {secret}
void FixedObstacleAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg)
{
    CollisionManager *cm = (CollisionManager *)arg;
    BoundariesManager &bm = cm->GetBoundariesManager();
    if (Set)
    {
        cm->m_ObstaclesCopied = TRUE;

        // we test if the BeObject had the Fixed Attribute
        if (obj->HasAttribute(cm->m_MovingAttribute))
        {
            obj->RemoveAttribute(cm->m_MovingAttribute);
            // and We add it as a moving obstacle
            bm.Add((CK3dEntity *)obj, FALSE);
        }
        else
        {
            // We add simply the object to the Boundaries Manager
            bm.Add((CK3dEntity *)obj, FALSE);
        }
    }
    else
    {
        CKContext *ctx = obj->GetCKContext();
        if (!ctx->IsInClearAll())
            bm.Remove((CK3dEntity *)obj);
    }
}

// {secret}
CollisionManager::CollisionManager(CKContext *Context) : CKCollisionManager(Context, (CKSTRING)CollisionManagerName)
{
    m_MovingEntitiesMatrix = NULL;
    m_Context->RegisterNewManager(this);
}

CKERROR CollisionManager::OnCKInit()
{
    // We create the attributes
    // we register all the attributes types related to the Collision Manager
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    // first we create the attribute category "Collision"
    attman->AddCategory((CKSTRING)CollisionManagerName);

    int att;
    // Fixed Obstacle
    att = attman->RegisterNewAttributeType((CKSTRING)FixedObstacleName, CKPGUID_OBSTACLE, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, (CKSTRING)CollisionManagerName);
    attman->SetAttributeDefaultValue(att, "2;TRUE");
    m_FixedAttribute = att;

    // Moving Obstacle
    att = attman->RegisterNewAttributeType((CKSTRING)MovingObstacleName, CKPGUID_OBSTACLE, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, (CKSTRING)CollisionManagerName);
    attman->SetAttributeDefaultValue(att, "2;TRUE");
    m_MovingAttribute = att;

    attman->SetAttributeCallbackFunction(m_FixedAttribute, FixedObstacleAttributeCallback, this);
    attman->SetAttributeCallbackFunction(m_MovingAttribute, MovingObstacleAttributeCallback, this);

    // Initialization des tableaux
    if (m_MovingEntitiesMatrix)
        delete[] m_MovingEntitiesMatrix;
    m_MovingEntitiesMaxSize = 8;
    m_MovingEntitiesMatrix = new SavedMovingEntity[m_MovingEntitiesMaxSize];
    m_MovingEntitiesCount = 0;

    m_BoundariesManager.Init();
    m_ObstaclesCopied = FALSE;

    return CK_OK;
}

CollisionManager::~CollisionManager()
{
    delete[] m_MovingEntitiesMatrix;
}

///////////////////////////////////////////////////////
//-----------------------------------------------------
// Obstacles Management
//-----------------------------------------------------
///////////////////////////////////////////////////////

/*************************************************
    Obstacle adding
*************************************************/

/*************************************************
Name: AddObstacle

Summary: Declare a 3dEntity as an obstacle for the collision manager

Arguments:
    ent: the 3dEntity to declare as an obstacle
    moving: whether or not the declared 3dEntity will move during the processing
    precision: the geometric precision we want to use for defining this entity during the collision test
    hiera: whether or not we want to consider the entire hierarchy of entity

Remarks:
    This function in fact only set the FixedObstacle or MovingObstacle attribute, depending on
    the value of the moving CKBOOL. It is provided for ease of use in declaring fastly obstacles.

{Group:Obstacles Management}

See also: CollisionManager, CollisionManager::DetectCollision
*************************************************/
void CollisionManager::AddObstacle(CK3dEntity *ent, CKBOOL moving, CK_GEOMETRICPRECISION precision, CKBOOL hiera)
{
    if (ent)
    {
        if (moving)
        {
            ent->SetAttribute(m_MovingAttribute);
        }
        else
        {
            ent->SetAttribute(m_FixedAttribute);
        }
        WriteAttributeValues(ent, moving, precision, hiera);
    }
}

/*************************************************
Name: AddObstacles

Summary: Declare as obstacles every 3dEntity whose the name contains a certain substring

Arguments:
    level: the level in which the manager will search the object to define
    substring: substring to search in the 3dEntities name (case sensitive)
    moving: whether or not the declared 3dEntities will move during the processing
    precision: the geometric precision we want to use for defining these entities during the collision test
    hiera: whether or not we want to consider the entire hierarchy of all the entities

Return Value: Number of object defined as obstacles

Remarks:
    This function calls the CollisionManager::AddObstacle function for each 3dEntity contained in the
    array.

{Group:Obstacles Management}

See also: CollisionManager, CollisionManager::AddObstacle, CollisionManager::DetectCollision
*************************************************/
int CollisionManager::AddObstaclesByName(CKLevel *level, CKSTRING substring, CKBOOL moving, CK_GEOMETRICPRECISION precision, CKBOOL hiera)
{
    if (!level)
        return 0;
    int count = 0;
    XObjectPointerArray myarray = level->ComputeObjectList(CKCID_3DENTITY);
    for (CKObject **it = myarray.Begin(); it != myarray.End(); ++it)
    {
        CKObject *obj = *it;
        if (obj)
        {
            if (obj->GetName())
            {
                if (strstr(obj->GetName(), substring))
                {
                    AddObstacle((CK3dEntity *)obj, moving, precision, hiera);
                    count++;
                }
            }
        }
    }
    return count;
}

/*************************************************
    Obstacle removing
*************************************************/

/*************************************************
Name: RemoveObstacle

Summary: Remove a 3dEntity from the collision manager

Arguments:
    ent: the 3dEntity to declare as an obstacle

Remarks:
    This function in fact only remove the FixedObstacle or MovingObstacle attribute if it was
    present, otherwise it does nothing.

{Group:Obstacles Management}

See also: CollisionManager, CollisionManager::DetectCollision
*************************************************/
void CollisionManager::RemoveObstacle(CK3dEntity *ent)
{
    if (ent)
    {
        if (ent->HasAttribute(m_FixedAttribute))
        {
            ent->RemoveAttribute(m_FixedAttribute);
        }
        else
        {
            if (ent->HasAttribute(m_MovingAttribute))
            {
                ent->RemoveAttribute(m_MovingAttribute);
            }
        }
    }
}

/*************************************************
Name: RemoveAllObstacles

Summary: Remove all the entities declared so far as obstacles from the collision manager

Arguments:
    level: whether or not you want to remove the obstacles from the current scene or from the entire level

Remarks:

{Group:Obstacles Management}

See also: CollisionManager, CollisionManager::DetectCollision
*************************************************/
void CollisionManager::RemoveAllObstacles(CKBOOL level)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    int atts[2] = {m_FixedAttribute, m_MovingAttribute};
    XObjectPointerArray array;
    if (!level)
        array = attman->FillListByAttributes(atts, 2);
    else
        array = attman->FillListByGlobalAttributes(atts, 2);

    for (CKObject **it = array.Begin(); it != array.End(); ++it)
    {
        RemoveObstacle((CK3dEntity *)*it);
    }
}

/*************************************************
    Obstacle Count access
*************************************************/

/*************************************************
Name: GetObstacleCount

Summary: Return the total numbers of objects declared as obstacles

Arguments:
    level: whether or not you want to consider the obstacles of the entire level or just the ones from the current scene

Remarks:
    This function return the total count of obstacles, either they are defined as moving or fixed
    obstacles

{Group:Obstacles Access}

See also: CollisionManager, CollisionManager::GetObstacle
*************************************************/
int CollisionManager::GetObstacleCount(CKBOOL level)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    if (level)
    {
        const XObjectPointerArray &farray = attman->GetGlobalAttributeListPtr(m_FixedAttribute);
        const XObjectPointerArray &marray = attman->GetGlobalAttributeListPtr(m_MovingAttribute);
        return farray.Size() + marray.Size();
    }
    else
    {
        const XObjectPointerArray &farray = attman->GetAttributeListPtr(m_FixedAttribute);
        const XObjectPointerArray &marray = attman->GetAttributeListPtr(m_MovingAttribute);
        return farray.Size() + marray.Size();
    }
}

/*************************************************
Name: GetFixedObstacleCount

Summary: Return the total numbers of objects declared as fixed obstacles

Arguments:
    level: whether or not you want to consider the obstacles of the entire level or just the ones from the current scene

{Group:Obstacles Access}

See also: CollisionManager, CollisionManager::GetFixedObstacle
*************************************************/
int CollisionManager::GetFixedObstacleCount(CKBOOL level)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    if (level)
    {
        const XObjectPointerArray &array = attman->GetGlobalAttributeListPtr(m_FixedAttribute);
        return array.Size();
    }
    else
    {
        const XObjectPointerArray &array = attman->GetAttributeListPtr(m_FixedAttribute);
        return array.Size();
    }
    return 0;
}

/*************************************************
Name: GetMovingObstacleCount

Summary: Return the total numbers of objects declared as moving obstacles

Arguments:
    level: whether or not you want to consider the obstacles of the entire level or just the ones from the current scene

{Group:Obstacles Access}

See also: CollisionManager, CollisionManager::GetMovingObstacle
*************************************************/
int CollisionManager::GetMovingObstacleCount(CKBOOL level)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    if (level)
    {
        const XObjectPointerArray &array = attman->GetGlobalAttributeListPtr(m_MovingAttribute);
        return array.Size();
    }
    else
    {
        const XObjectPointerArray &array = attman->GetAttributeListPtr(m_MovingAttribute);
        return array.Size();
    }
    return 0;
}

/*************************************************
    Obstacles access
*************************************************/

/*************************************************
Name: GetObstacle

Summary: Return the 'pos'th object declared as obstacle, wheter moving or not

Arguments:
    pos: The index of the obstacle you want
    level: whether or not you want to consider the obstacles of the entire level or just the ones from the current scene

Return Value:
    The 'pos'th obstacle or null if it doesn't exist

{Group:Obstacles Access}

See also: CollisionManager, CollisionManager::GetObstacleCount
*************************************************/
CK3dEntity *CollisionManager::GetObstacle(int pos, CKBOOL level)
{
    if (pos < 0)
        return NULL;
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    if (level)
    {
        const XObjectPointerArray &array = attman->GetGlobalAttributeListPtr(m_FixedAttribute);
        int total = array.Size();
        if (pos >= total)
        {
            pos -= total;
            const XObjectPointerArray &array2 = attman->GetGlobalAttributeListPtr(m_MovingAttribute);
            return (CK3dEntity *)array2[pos];
        }
        return (CK3dEntity *)array[pos];
    }
    else
    {
        const XObjectPointerArray &array = attman->GetAttributeListPtr(m_FixedAttribute);
        int total = array.Size();
        if (pos >= total)
        {
            pos -= total;
            const XObjectPointerArray &array2 = attman->GetAttributeListPtr(m_MovingAttribute);
            return (CK3dEntity *)array2[pos];
        }
        return (CK3dEntity *)array[pos];
    }
    return NULL;
}

/*************************************************
Name: GetFixedObstacle

Summary: Return the 'pos'th object declared as fixed obstacle

Arguments:
    pos: The index of the fixed obstacle you want
    level: whether or not you want to consider the obstacles of the entire level or just the ones from the current scene

Return Value:
    The 'pos'th fixed obstacle or null if it doesn't exist

{Group:Obstacles Access}

See also: CollisionManager, CollisionManager::GetFixedObstacleCount
*************************************************/
CK3dEntity *CollisionManager::GetFixedObstacle(int pos, CKBOOL level)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    if (level)
    {
        const XObjectPointerArray &array = attman->GetGlobalAttributeListPtr(m_FixedAttribute);
        return (CK3dEntity *)array[pos];
    }
    else
    {
        const XObjectPointerArray &array = attman->GetAttributeListPtr(m_FixedAttribute);
        return (CK3dEntity *)array[pos];
    }
    return NULL;
}

/*************************************************
Name: GetMovingObstacle

Summary: Return the 'pos'th object declared as moving obstacle

Arguments:
    pos: The index of the moving obstacle you want
    level: whether or not you want to consider the obstacles of the entire level or just the ones from the current scene

Return Value:
    The 'pos'th moving obstacle or null if it doesn't exist

{Group:Obstacles Access}

See also: CollisionManager, CollisionManager::GetMovingObstacleCount
*************************************************/
CK3dEntity *CollisionManager::GetMovingObstacle(int pos, CKBOOL level)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    if (level)
    {
        const XObjectPointerArray &array = attman->GetGlobalAttributeListPtr(m_MovingAttribute);
        return (CK3dEntity *)array[pos];
    }
    else
    {
        const XObjectPointerArray &array = attman->GetAttributeListPtr(m_MovingAttribute);
        return (CK3dEntity *)array[pos];
    }
    return NULL;
}

CKBOOL CollisionManager::WriteAttributeValues(CK3dEntity *ent, CKBOOL moving, CK_GEOMETRICPRECISION geomtype, CKBOOL hiera)
{
    CKParameterOut *pout;
    CKParameterOut *geompout;
    CKParameterOut *hierapout;

    if (moving)
    {
        pout = ent->GetAttributeParameter(m_MovingAttribute);
    }
    else
    {
        pout = ent->GetAttributeParameter(m_FixedAttribute);
    }
    if (!pout)
        return FALSE;
    if (pout->GetGUID() == CKPGUID_OBSTACLE)
    {
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        geompout = (CKParameterOut *)CKGetObject(paramids[0]);
        hierapout = (CKParameterOut *)CKGetObject(paramids[1]);

        if (geompout)
            geompout->SetValue(&geomtype);
        if (hierapout)
            hierapout->SetValue(&hiera);
    }

    return TRUE;
}

CKBOOL CollisionManager::ReadAttributeValues(CK3dEntity *ent, CK_GEOMETRICPRECISION &geomtype, CKBOOL &hiera)
{
    CKParameterOut *pout;
    CKParameterOut *geompout;
    CKParameterOut *hierapout;

    pout = ent->GetAttributeParameter(m_MovingAttribute);
    if (!pout)
        pout = ent->GetAttributeParameter(m_FixedAttribute);
    if (!pout)
        return FALSE;
    if (pout->GetGUID() == CKPGUID_OBSTACLE)
    {
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        geompout = (CKParameterOut *)CKGetObject(paramids[0]);
        hierapout = (CKParameterOut *)CKGetObject(paramids[1]);

        /*
        paramstruct[0]->GetValue(&geomtype);
        paramstruct[1]->GetValue(&hiera);
        */

        if (geompout)
            geompout->GetValue(&geomtype);
        if (hierapout)
            hierapout->GetValue(&hiera);
    }
    else
    {
        // we set box level by default
        geomtype = CKCOLLISION_BOX;
        hiera = 0;
    }
    return TRUE;
}

const XArray<ImpactDesc> &CollisionManager::GetImpactArray()
{
    return m_ImpactArray;
}

CKERROR CollisionManager::PostProcess()
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &array = attman->GetAttributeListPtr(m_MovingAttribute);

    m_MovingEntitiesCount = array.Size();
    // we check if the number of moving isn't greater to the free spaces in the array
    TestAndReallocateMovingMatrix(m_MovingEntitiesCount);
    int ind = 0;

    for (CKObject **it = array.Begin(); it != array.End(); ++it)
    {
        CK3dEntity *ent = (CK3dEntity *)*it;

        m_MovingEntitiesMatrix[ind].m_Entity = CKOBJID(ent);
        m_MovingEntitiesMatrix[ind].m_WorldMatrix = ent->GetWorldMatrix();
        m_MovingEntitiesMatrix[ind].m_LastWorldBox = ent->GetHierarchicalBox();
        ind++;
    }

    if (m_ObstaclesCopied)
    {
        m_ObstaclesCopied = FALSE;
        m_BoundariesManager.UpdateAll();
    }

    m_BoundariesManager.UpdateStartMovings();
    return CK_OK;
}

CKERROR CollisionManager::PostLaunchScene(CKScene *OldScene, CKScene *NewScene)
{
    ///
    // we are changing of scene

    // We clear the boundaries...
    m_BoundariesManager.Clear();

    ///
    // ...and add the new ones :
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    CK_GEOMETRICPRECISION precision;
    CKBOOL hiera;

    // Fixed ones
    const XObjectPointerArray &fixedArray = attman->GetAttributeListPtr(m_FixedAttribute);
    XObjectPointerArray::Iterator it;
    for (it = fixedArray.Begin(); it != fixedArray.End(); ++it)
    {
        m_BoundariesManager.Add((CK3dEntity *)(*it), FALSE);

        // we flag the entity as hierarchical obstacle if necessary
        ReadAttributeValues((CK3dEntity *)(*it), precision, hiera);
        if (hiera)
            ((CK3dEntity *)(*it))->SetFlags(((CK3dEntity *)(*it))->GetFlags() | CK_3DENTITY_HIERARCHICALOBSTACLE);
        else
            ((CK3dEntity *)(*it))->SetFlags(((CK3dEntity *)(*it))->GetFlags() & ~CK_3DENTITY_HIERARCHICALOBSTACLE);
    }

    // Moving Ones
    const XObjectPointerArray &movingArray = attman->GetAttributeListPtr(m_MovingAttribute);
    for (it = movingArray.Begin(); it != movingArray.End(); ++it)
    {
        m_BoundariesManager.Add((CK3dEntity *)(*it), TRUE);

        // we flag the entity as hierarchical obstacle if necessary
        ReadAttributeValues((CK3dEntity *)(*it), precision, hiera);
        if (hiera)
            ((CK3dEntity *)(*it))->SetFlags(((CK3dEntity *)(*it))->GetFlags() | CK_3DENTITY_HIERARCHICALOBSTACLE);
        else
            ((CK3dEntity *)(*it))->SetFlags(((CK3dEntity *)(*it))->GetFlags() & ~CK_3DENTITY_HIERARCHICALOBSTACLE);
    }

    return CK_OK;
}

CKERROR CollisionManager::OnCKReset()
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &array = attman->GetAttributeListPtr(m_MovingAttribute);

    CKObject **it;
    for (it = array.Begin(); it != array.End(); ++it)
    {
        CK3dEntity *ent = (CK3dEntity *)*it;
        if (!ent)
            continue;

        ent->SetFlags(ent->GetFlags() | CK_3DENTITY_UPDATELASTFRAME);
    }

    return CK_OK;
}

CKERROR CollisionManager::OnCKPlay()
{
    // updating all the boundaries of objects (even fix) that
    // can have been moved during the pause
    m_BoundariesManager.UpdateAll();

    return CK_OK;
}

/*
CKERROR CollisionManager::OnPostCopy(CKDependenciesContext& iContext)
{
    return CK_OK;
}
*/

CKERROR CollisionManager::PostLoad()
{
    /// Patch for old enums
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PARAMETERLOCAL);
    int j, idcount = m_Context->GetObjectsCountByClassID(CKCID_PARAMETERLOCAL);

    for (j = 0; j < idcount; ++j)
    {
        CKParameterLocal *pl = (CKParameterLocal *)m_Context->GetObject(ids[j]);
        if (!pl)
            continue;

        if (pl->GetGUID() == CKPGUID_OBSTACLEPRECISION)
        {
            int *value = (int *)pl->GetWriteDataPtr();
            if (*value == 4)
                *value = 2;
        }

        if (pl->GetGUID() == CKPGUID_OBSTACLEPRECISIONBEH)
        {
            int *value = (int *)pl->GetWriteDataPtr();
            if (*value == 4)
                *value = 2;
        }
    }

    ids = m_Context->GetObjectsListByClassID(CKCID_PARAMETEROUT);
    idcount = m_Context->GetObjectsCountByClassID(CKCID_PARAMETEROUT);

    for (j = 0; j < idcount; ++j)
    {
        CKParameterLocal *pl = (CKParameterLocal *)m_Context->GetObject(ids[j]);
        if (!pl)
            continue;

        if (pl->GetGUID() == CKPGUID_OBSTACLEPRECISION)
        {
            int *value = (int *)pl->GetWriteDataPtr();
            if (*value == 4)
                *value = 2;
        }

        if (pl->GetGUID() == CKPGUID_OBSTACLEPRECISIONBEH)
        {
            int *value = (int *)pl->GetWriteDataPtr();
            if (*value == 4)
                *value = 2;
        }
    }

    return CK_OK;
}

BoundariesManager &CollisionManager::GetBoundariesManager()
{
    return m_BoundariesManager;
}

int CollisionManager::SeekMovingMatrix(CK3dEntity *ent)
{
    CK_ID id = CKOBJID(ent);
    int i;
    for (i = 0; i < m_MovingEntitiesCount; i++)
    {
        if (m_MovingEntitiesMatrix[i].m_Entity == id)
            break;
    }
    return i;
}

void CollisionManager::TestAndReallocateMovingMatrix(int newSize)
{
    if (newSize > m_MovingEntitiesMaxSize)
    {
        // we double the size
        while (newSize > m_MovingEntitiesMaxSize)
            m_MovingEntitiesMaxSize *= 2;

        // we delete the old array
        delete[] m_MovingEntitiesMatrix;
        // we realloc a new array
        m_MovingEntitiesMatrix = new SavedMovingEntity[m_MovingEntitiesMaxSize];
    }
}

// void CollisionManager::RadiusTest(CK3dEntity* ent,XArray<CKObject*>* collarray)
// {
// 	VxVector o,e,d,p;
// 	// origin of the movement
// 	int ind = SeekMovingMatrix(ent);
//
// 	VxVector bc;
//
// 	// end of the movement
// 	const VxBbox& box = ent->GetHierarchicalBox();
// 	e = (box.Min+box.Max)*0.5f;
// 	float r1=XMax((box.Max.x-box.Min.x),(box.Max.y-box.Min.y));
// 	r1=XMax(r1,(box.Max.z-box.Min.z))*0.5f;
//
// 	// start of the movement
// 	if(ind == m_MovingEntitiesCount) {
// 		// the object is not moving so the start is "confondu" with the end
// 		o = e;
// 	} else {
// 		const VxBbox& box = ent->GetHierarchicalBox(TRUE);
// 		bc = (box.Min+box.Max)*0.5f;
// 		Vx3DMultiplyMatrixVector(&o,m_MovingEntitiesMatrix[ind].m_WorldMatrix,&bc);
// 	}
//
//
// 	// movement
// 	d = Normalize(e-o);
//
// 	int size = collarray->Size();
// 	int i=0;
// 	do {
// 		CK3dEntity* ob = (CK3dEntity*)(*collarray)[i];
// 		if(!ob) return;
//
// 		const VxBbox& box = ob->GetHierarchicalBox();	 // crappy when hierarchical box too big
// 		p = (box.Min+box.Max)*0.5f;
//
// 		float r2=XMax((box.Max.x-box.Min.x),(box.Max.y-box.Min.y));
// 		r2=XMax(r2,(box.Max.z-box.Min.z))*0.5f;
//
// 		CKRenderManager* rm = m_Context->GetRenderManager();
// 		rm->RegisterSphere(VxSphere(p,r2),0xffff00ff,1000);
//
// 		float sd;
// 		if(ob->HasAttribute(m_MovingAttribute)) {
// 			// TODO
// 			sd = 0;
// 		} else {
// 			VxRay ray(o,d,NULL);
// 			sd = ray.SquareDistance(p);
//
// 			CKRenderManager* rm = m_Context->GetRenderManager();
// 			rm->RegisterSphere(VxSphere(p,sqrtf(sd)),0xffffffff,1000);
// 		}
// 		if(sd > (r1+r2)*(r1+r2)) {
// 			collarray->RemoveAt(i);
//
// 		} else {
// 			i++;
// 		}
// 	} while(--size);
// }

void CollisionManager::Clear()
{
    m_BoundariesManager.Clear();
    m_MovingEntitiesCount = 0;
}

CKBOOL CollisionManager::IsObstacle(CK3dEntity *ent, CKBOOL moving)
{
    if (!ent)
        return FALSE;
    if (moving)
    {
        return ent->HasAttribute(m_MovingAttribute);
    }
    else
    {
        return ent->HasAttribute(m_FixedAttribute);
    }
}

// {secret}
CKGUID GetCollisionManagerGuid()
{
    return COLLISION_MANAGER_GUID;
}