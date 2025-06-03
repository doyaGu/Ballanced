/*************************************************************************/
/*	File : FloorManager.cpp											 	 */
/*																		 */
/*************************************************************************/
#include "CKAll.h"
#include "FloorManager.h"

#include <float.h>

extern const char *FloorManagerName;

// {secret}
const char *FloorName = "Floor";

// {secret}
FloorManager::FloorManager(CKContext *Context) : CKFloorManager(Context, (CKSTRING)FloorManagerName)
{
    m_LimitAngle = 80.0f * PI / 180.0f;
    m_CosAngle = (float)cos(m_LimitAngle);

    m_Context->RegisterNewManager(this);
}

FloorManager::~FloorManager()
{
}

CKERROR FloorManager::PreClearAll()
{
    // Angle Limits
    m_LimitAngle = 80.0f * PI / 180.0f;
    m_CosAngle = (float)cos(m_LimitAngle);

    // Destroy the computed edges
    m_FloorEdges.Clear();

    return CK_OK;
}

CKERROR FloorManager::OnCKInit()
{
    m_Context->GetParameterManager()->RegisterNewEnum(CKPGUID_FLOORGEOMETRY, "Geometry", "Faces=0,Bounding Box=1");
    m_Context->GetParameterManager()->RegisterNewStructure(CKPGUID_FLOOR, "Floor", "Floor Geometry,Moving Floor,Floor Type,Use Hierarchy?,First Contact?", CKPGUID_FLOORGEOMETRY, CKPGUID_BOOL, CKPGUID_INT, CKPGUID_BOOL, CKPGUID_BOOL);

    // We create the attributes
    // we register all the attributes types related to the Collision Manager
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    // first we create the attribute category "Particle Systems"
    attman->AddCategory((CKSTRING)FloorManagerName);

    int att;
    // Floor Attribute
    att = attman->RegisterNewAttributeType((CKSTRING)FloorName, CKPGUID_FLOOR, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, (CKSTRING)FloorManagerName);
    attman->SetAttributeDefaultValue(att, "0;TRUE;0;FALSE;TRUE");
    m_FloorAttribute = att;

    return CK_OK;
}

CKERROR FloorManager::OnCKPlay()
{
    /* Sharing of meshes sucks !
    CKAttributeManager* attman = m_Context->GetAttributeManager();
    const XObjectPointerArray& floors = attman->GetAttributeListPtr(m_FloorAttribute);

    for (CKObject** it = floors.Begin(); it != floors.End(); ++it) {
        CK3dEntity *mov = (CK3dEntity *)*it;
        // we test the entity
        if(!mov) continue;

        // we have to test if it is fixed
        CKBOOL Moving;
        ReadAttributeValues(mov,NULL,&Moving,NULL,NULL,NULL);

        if (!Moving) {
            // we test if the floor isn't WorldTransformed
            if (!(mov->GetMoveableFlags() &  VX_MOVEABLE_WORLDALIGNED)) {
                // we have to transform it again
                mov->ChangeReferential();
            }
        }
    }
    */
    return CK_OK;
}

CKERROR FloorManager::PostLoad()
{
    /// Patch for old enums
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PARAMETEROUT);
    int idcount = m_Context->GetObjectsCountByClassID(CKCID_PARAMETEROUT);

    for (int j = 0; j < idcount; ++j)
    {
        CKParameterOut *pl = (CKParameterOut *)m_Context->GetObject(ids[j]);
        if (!pl)
            continue;

        if (pl->GetGUID() == CKPGUID_FLOORGEOMETRY)
        {
            int *value = (int *)pl->GetWriteDataPtr();
            if (*value == 2)
                *value = 1;
        }
    }

    // Si ya un pb su rles set atributes sur des ancinnes version de fichier,
    // le faire aussi pour les locaux

    ///
    // After a load, we destroy all the floor edges previously created

    if (m_FloorEdges.Size())
    {
        HashFloor2Edges::Iterator it = m_FloorEdges.Begin();
        HashFloor2Edges::Iterator itend = m_FloorEdges.End();

        while (it != itend)
        {
            (*it).Clear();
            ++it;
        }

        m_FloorEdges.Clear();
    }

    return CK_OK;
}
/*************************************************
Name:

Summary:

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
void FloorManager::SetLimitAngle(float angle)
{
    if (angle > PI / 2.0f)
        angle = PI / 2.0f;
    if (angle < 0.0f)
        angle = 0.0f;
    m_LimitAngle = angle;
    m_CosAngle = (float)cos(m_LimitAngle);
}

float FloorManager::GetLimitAngle()
{
    return m_LimitAngle;
}

CKBOOL FloorManager::ReadAttributeValues(CK3dEntity *ent, CKDWORD *geo, CKBOOL *moving, int *type, CKBOOL *hiera, CKBOOL *first)
{
    if (!ent)
        return FALSE;

    CKParameterOut *pout = ent->GetAttributeParameter(m_FloorAttribute);
    if (!pout)
        return FALSE;

    if (pout->GetGUID() == CKPGUID_FLOOR)
    {
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        if (geo)
        {
            pout = (CKParameterOut *)CKGetObject(paramids[0]);
            if (pout)
                pout->GetValue(geo);
        }
        if (moving)
        {
            pout = (CKParameterOut *)CKGetObject(paramids[1]);
            if (pout)
                pout->GetValue(moving);
        }
        if (type)
        {
            pout = (CKParameterOut *)CKGetObject(paramids[2]);
            if (pout)
                pout->GetValue(type);
        }
        if (hiera)
        {
            pout = (CKParameterOut *)CKGetObject(paramids[3]);
            if (pout)
                pout->GetValue(hiera);
        }
        if (first)
        {
            pout = (CKParameterOut *)CKGetObject(paramids[4]);
            if (pout)
                pout->GetValue(first);
        }
    }
    return TRUE;
}

// {secret}
CKBOOL FloorManager::TestFixedFloor(CK3dEntity *mov, const VxVector &pos, VxVector &inter, VxVector &normal, CKBOOL first, int &face, int facetotestfirst)
{
    CKMesh *mesh = mov->GetCurrentMesh();

    // we test if the floor isn't WorldTransformed
    if (!(mov->GetMoveableFlags() & VX_MOVEABLE_WORLDALIGNED))
    {
        // we have to transform it again
        mov->ChangeReferential();
    }

    float x = pos.x;
    float z = pos.z;

    // we prepare for face  travesal
    CKDWORD StridePos;
    CKBYTE *m_VertexArray = (CKBYTE *)mesh->GetPositionsPtr(&StridePos);

    CKWORD *faces = mesh->GetFacesIndices();
    int i, m_NbFaces = mesh->GetFaceCount();
    VxVector *pts[3];

    float xmin, xmax, zmin, zmax;

    CKBOOL Collision = FALSE;

    float tmax = 100000000.0f;

    VxVector p1 = pos, p2, twup(0.0f, 1.0f, 0.0f), tempres;
    float t;
    p2 = p1 + twup;

    // we test every single face
    CKWORD *currentface = faces + (facetotestfirst) * 3;
    for (i = facetotestfirst; i < m_NbFaces; i++, currentface += 3)
    {
        pts[0] = (VxVector *)&m_VertexArray[*currentface * StridePos];
        pts[1] = (VxVector *)&m_VertexArray[*(currentface + 1) * StridePos];
        pts[2] = (VxVector *)&m_VertexArray[*(currentface + 2) * StridePos];

        if (pts[0]->x < pts[1]->x)
        {
            xmin = pts[0]->x;
            xmax = pts[1]->x;
        }
        else
        {
            xmin = pts[1]->x;
            xmax = pts[0]->x;
        }
        if (xmin > pts[2]->x)
        {
            xmin = pts[2]->x;
        }
        else
        {
            if (xmax < pts[2]->x)
            {
                xmax = pts[2]->x;
            }
        }
        if (x < xmin)
            continue;
        if (x > xmax)
            continue;

        if (pts[0]->z < pts[1]->z)
        {
            zmin = pts[0]->z;
            zmax = pts[1]->z;
        }
        else
        {
            zmin = pts[1]->z;
            zmax = pts[0]->z;
        }
        if (zmin > pts[2]->z)
        {
            zmin = pts[2]->z;
        }
        else
        {
            if (zmax < pts[2]->z)
            {
                zmax = pts[2]->z;
            }
        }

        if (z < zmin)
            continue;
        if (z > zmax)
            continue;

        const VxVector &facenormal = mesh->GetFaceNormal(i);

        // we now have to check if the face is not too inclined
        if (DotProduct(twup, facenormal) < m_CosAngle)
            continue;

        // see if floorfaceint couldn't take a point and a dir...
        VxRay ray(p1, p2);
        if (VxIntersect::LineFace(ray, *pts[0], *pts[1], *pts[2], facenormal, tempres, t))
        {
            // we test we need to get the nearest face or one is enough
            if (first)
            {
                normal = facenormal;
                inter = tempres;
                face = i;
                return TRUE;
            }
            else
            {
                if (fabs(t) < tmax)
                {
                    tmax = (float)fabs(t);
                    normal = facenormal;
                    inter = tempres;
                    face = i;
                    Collision = TRUE;
                }
            }
        }
    } // end of the for(faces)

    currentface = faces + (facetotestfirst - 1) * 3;
    for (i = facetotestfirst - 1; i >= 0; i--, currentface -= 3)
    {
        pts[0] = (VxVector *)&m_VertexArray[*currentface * StridePos];
        pts[1] = (VxVector *)&m_VertexArray[*(currentface + 1) * StridePos];
        pts[2] = (VxVector *)&m_VertexArray[*(currentface + 2) * StridePos];

        if (pts[0]->x < pts[1]->x)
        {
            xmin = pts[0]->x;
            xmax = pts[1]->x;
        }
        else
        {
            xmin = pts[1]->x;
            xmax = pts[0]->x;
        }
        if (xmin > pts[2]->x)
        {
            xmin = pts[2]->x;
        }
        else
        {
            if (xmax < pts[2]->x)
            {
                xmax = pts[2]->x;
            }
        }
        if (x < xmin)
            continue;
        if (x > xmax)
            continue;

        if (pts[0]->z < pts[1]->z)
        {
            zmin = pts[0]->z;
            zmax = pts[1]->z;
        }
        else
        {
            zmin = pts[1]->z;
            zmax = pts[0]->z;
        }
        if (zmin > pts[2]->z)
        {
            zmin = pts[2]->z;
        }
        else
        {
            if (zmax < pts[2]->z)
            {
                zmax = pts[2]->z;
            }
        }

        if (z < zmin)
            continue;
        if (z > zmax)
            continue;

        const VxVector &facenormal = mesh->GetFaceNormal(i);

        // we now have to check if the face is not too inclined
        if (DotProduct(twup, facenormal) < m_CosAngle)
            continue;

        // see if floorfaceint couldn't take a point and a dir...
        VxRay ray(p1, p2);
        if (VxIntersect::LineFace(ray, *pts[0], *pts[1], *pts[2], facenormal, tempres, t))
        {
            // we test we need to get the nearest face or one is enough
            if (first)
            {
                normal = facenormal;
                inter = tempres;
                face = i;
                return TRUE;
            }
            else
            {
                if (fabs(t) < tmax)
                {
                    tmax = (float)fabs(t);
                    normal = facenormal;
                    inter = tempres;
                    face = i;
                    Collision = TRUE;
                }
            }
        }
    } // end of the for(faces)

    return Collision;
}

CKBOOL FloorManager::FloorRayIntersect(CK3dEntity *mov, CKMesh *mesh, const VxBbox &rayextents, CKBOOL first, int x, int z, const VxRay &ray, int facetotestfirst, int lastfacetotest, float &tmax, int &face, VxVector &inter)
{
    int step = 1;
    int stepface = 3;
    if (lastfacetotest < facetotestfirst)
    {
        step = -1;
        stepface = -3;
    }

    CKDWORD StridePos;
    CKBYTE *m_VertexArray = (CKBYTE *)mesh->GetPositionsPtr(&StridePos);

    float t;
    VxVector tempres;

    CKWORD *faces = mesh->GetFacesIndices();

    CKBOOL Collision = FALSE;

    CKWORD *currentface = faces + (facetotestfirst) * 3;
    for (int i = facetotestfirst; i != lastfacetotest; i += step, currentface += stepface)
    {
        const VxVector &pt0 = *(VxVector *)&m_VertexArray[*currentface * StridePos];
        const VxVector &pt1 = *(VxVector *)&m_VertexArray[*(currentface + 1) * StridePos];
        const VxVector &pt2 = *(VxVector *)&m_VertexArray[*(currentface + 2) * StridePos];

        // face rejection by Bbox 2D
        float min, max;
        XMinMax(pt0.v[x], pt1.v[x], pt2.v[x], min, max);
        if (rayextents.Max.x < min)
            continue;
        if (rayextents.Min.x > max)
            continue;

        XMinMax(pt0.v[z], pt1.v[z], pt2.v[z], min, max);
        if (rayextents.Max.z < min)
            continue;
        if (rayextents.Min.z > max)
            continue;

        // we now have to check if the face is not too inclined
        const VxVector &facenormal = mesh->GetFaceNormal(i);
        if (DotProduct(ray.GetDirection(), facenormal) < m_CosAngle)
            continue;

        // see if floorfaceint couldn't take a point and a dir...
        if (VxIntersect::LineFace(ray, pt0, pt1, pt2, facenormal, tempres, t))
        {
            if (first)
            {
                face = i;
                inter = tempres;
                return TRUE;
            }
            else
            {
                if (fabs(t) < tmax)
                {
                    tmax = (float)fabs(t);
                    face = i;
                    inter = tempres;
                    Collision = TRUE;
                }
            }
        }
    }

    return Collision;
}

// {secret}
CKBOOL FloorManager::TestMovingFloor(CK3dEntity *mov, const VxVector &pos, VxVector &inter, VxVector &normal, CKBOOL first, int &face, int facetotestfirst)
{
    CKMesh *mesh = mov->GetCurrentMesh();

    VxVector p1, twup(0, 1.0f, 0), tempres;

    mov->InverseTransform(&p1, &pos);
    mov->InverseTransformVector(&twup, &twup);
    twup.Normalize();
    VxVector p2 = p1 + twup;

    // we first need to know the orientation
    int x;
    int z;

    VxVector planenormal;

    VxVector worldup(0.0f, 1.0f, 0.0f);
    float smcp, min;
    // Suppose first that X axis
    // Is aligned with world Up axis
    min = smcp = twup.y * twup.y + twup.z * twup.z;
    planenormal.Set(1.0f, 0.0f, 0.0f);
    min = smcp;
    x = 1;
    z = 2;
    // Is Y axis aligned with world Up axis
    smcp = twup.x * twup.x + twup.z * twup.z;
    if (smcp < min)
    {
        planenormal.Set(0.0f, 1.0f, 0.0f);
        min = smcp;
        x = 0;
        z = 2;
    }
    // Is Z axis aligned with world Up axis
    smcp = twup.x * twup.x + twup.y * twup.y;
    if (smcp < min)
    {
        planenormal.Set(0.0f, 0.0f, 1.0f);
        x = 0;
        z = 1;
    }

    // we now calulate the 2D extents of the ray
    VxVector int1, int2;
    const VxBbox &box = mesh->GetLocalBox();

    VxRay ray(p1, twup, NULL);
    float t;
    {
        VxPlane plane(planenormal, box.Max);

        VxIntersect::LinePlane(ray, plane, int1, t);
        plane.Create(planenormal, box.Min);
        VxIntersect::LinePlane(ray, plane, int2, t);
    }

    VxBbox rayextents;
    XMinMax(int1.v[x], int2.v[x], rayextents.Min.x, rayextents.Max.x);
    XMinMax(int1.v[z], int2.v[z], rayextents.Min.z, rayextents.Max.z);

    int nbfaces = mesh->GetFaceCount();

    float tmax = 100000000.0f;

    CKBOOL Collision = FloorRayIntersect(mov, mesh, rayextents, first, x, z, ray, facetotestfirst, nbfaces, tmax, face, inter);
    if (!first)
    {
        Collision |= FloorRayIntersect(mov, mesh, rayextents, first, x, z, ray, facetotestfirst - 1, -1, tmax, face, inter);
    }

    if (Collision)
    {
        const VxVector &facenormal = mesh->GetFaceNormal(face);
        mov->TransformVector(&normal, &facenormal);
        mov->Transform(&inter, &inter);
    }

    return Collision;
}

// {secret}
CKBOOL FloorManager::TestBoxFloor(CK3dEntity *mov, const VxVector &pos, VxVector &inter, VxVector &normal)
{
    CKMesh *mesh = mov->GetCurrentMesh();
    const VxBbox &box = mesh->GetLocalBox();

    VxVector p1 = pos, dir(0.0f, 1.0f, 0.0f);
    mov->InverseTransform(&p1, &p1);
    mov->InverseTransformVector(&dir, &dir);

    VxRay ray(p1, dir, NULL);
    CKBOOL collision = VxIntersect::LineBox(ray, box, inter, NULL, &normal);
    mov->Transform(&inter, &inter);
    mov->Transform(&normal, &normal);
    return collision;
}

// {secret}
CKBOOL FloorManager::FloorFastRejection(CK3dEntity *mov, const VxVector &pos, float nearestdown, float nearestup)
{
    CKMesh *mesh = mov->GetCurrentMesh();
    // if the floor have no mesh, we skip it
    if (!mesh)
        return TRUE;
    // if the floor mesh is in line mode, we skip it
    if (!mesh->GetFaceCount())
        return TRUE;

    const VxBbox &box = mov->GetBoundingBox();
    if (pos.x < box.Min.x)
        return TRUE;
    if (pos.x > box.Max.x)
        return TRUE;
    if (pos.z < box.Min.z)
        return TRUE;
    if (pos.z > box.Max.z)
        return TRUE;
    float distmax = pos.y - box.Max.y;
    if (distmax > 0.0)
    { // the pos is up the floor
        if (distmax > -nearestdown)
            return TRUE;
    }
    else
    { // the pos is down or in the floor
        float distmin = pos.y - box.Min.y;
        if (distmin > 0.0)
        { // the point is in the floor
            return FALSE;
        }
        else
        { // the point is beneath the floor
            // we test if it is not too up
            if (-distmin > nearestup)
                return TRUE;
        }
    }
    return FALSE;
}

CK_FLOORNEAREST FloorManager::GetNearestFloors(const VxVector &iPosition, CKFloorPoint *oFP, CK3dEntity *iExcludeFloor)
{
    if (_isnan(iPosition.x) ||
        _isnan(iPosition.y) ||
        _isnan(iPosition.z))
        return CKFLOOR_NOFLOOR;

    oFP->Clear();

    StartProfile();

    CKBOOL Collision = FALSE;
    CKBOOL Moving = FALSE;
    CKBOOL First = FALSE;
    CKDWORD FloorGeometry = CKFLOOR_FACES;

    VxVector res, norm;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &floors = attman->GetAttributeListPtr(m_FloorAttribute);

    int faceindex = 0;

    for (CKObject **it = floors.Begin(); it != floors.End(); ++it)
    {
        CK3dEntity *mov = (CK3dEntity *)*it;
        // we test the entity
        if (!mov)
            continue;

        // if the floor is to be exclude
        if (mov == iExcludeFloor)
            continue;

        // we rapidly test if the floor might interest us
        if (FloorFastRejection(mov, iPosition, oFP->m_DownDistance, oFP->m_UpDistance))
            continue;

        // we have to test if it is moving
        ReadAttributeValues(mov, &FloorGeometry, &Moving, NULL, NULL, &First);
        switch (FloorGeometry)
        {
        case CKFLOOR_FACES:
            // if (!Moving) {
            //	Collision = TestFixedFloor(mov,pos,res,norm,First,faceindex);
            //} else {
            Collision = TestMovingFloor(mov, iPosition, res, norm, First, faceindex);
            //}
            break;
        case CKFLOOR_BOX:
            Collision = TestBoxFloor(mov, iPosition, res, norm);
            faceindex = 0;
            break;
        default: // most probably faces geometry, but who knows....
                 // if (!Moving) {
                 //	Collision = TestFixedFloor(mov,pos,res,norm,First,faceindex);
                 //} else {
            Collision = TestMovingFloor(mov, iPosition, res, norm, First, faceindex);
            //}
            break;
        }
        if (Collision)
        {
            float dist = res.y - iPosition.y;
            if (dist > 0.0f)
            {
                if (dist < oFP->m_UpDistance)
                {
                    oFP->m_UpFloor = CKOBJID(mov);
                    oFP->m_UpDistance = dist;
                    oFP->m_UpNormal = norm;
                    oFP->m_UpFaceIndex = faceindex;
                }
            }
            else
            {
                if (dist > oFP->m_DownDistance)
                {
                    oFP->m_DownFloor = CKOBJID(mov);
                    oFP->m_DownDistance = dist;
                    oFP->m_DownNormal = norm;
                    oFP->m_DownFaceIndex = faceindex;
                }
            }
        }

    } // end of the for(mov)

    StopProfile();

    if (oFP->m_DownFloor)
    {
        if (oFP->m_UpFloor)
        {
            if (-oFP->m_DownDistance < oFP->m_UpDistance)
            {
                return CKFLOOR_DOWN; // the down floor is closer
            }
            else
            {
                return CKFLOOR_UP; // the up floor is closer
            }
        }
        else
            return CKFLOOR_DOWN; // There is only a down floor : Choose the down floor
    }
    else
    {
        if (oFP->m_UpFloor)
            return CKFLOOR_UP; // There is only an up floor : Choose the up floor
    }

    return CKFLOOR_NOFLOOR; // No Floors Found
}

/*************************************************
Name:

Summary: Not Yet Documented

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
int FloorManager::AddFloorObjectsByName(CKLevel *level, CKSTRING floorname, CK_FLOORGEOMETRY geo, CKBOOL moving, int type, CKBOOL hiera, CKBOOL first)
{
    if (!level)
        return 0;
    int count = 0;
    XObjectPointerArray myarray = level->ComputeObjectList(CKCID_3DENTITY);
    for (CKObject **it = myarray.Begin(); it != myarray.End(); ++it)
    {
        CKObject *obj = *it;
        if (obj)
            if (obj->GetName())
                if (strstr(obj->GetName(), floorname))
                {
                    AddFloorObject((CK3dEntity *)obj, geo, moving, type, hiera, first);
                    count++;
                }
    }
    return count;
}

//-----------------------------------------------------
// Floors Objects

/*************************************************
Name:

Summary: Not Yet Documented

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
void FloorManager::AddFloorObject(CK3dEntity *ent, CK_FLOORGEOMETRY geo, CKBOOL moving, int type, CKBOOL hiera, CKBOOL first)
{
    ent->SetAttribute(m_FloorAttribute);

    CKParameterOut *pout = ent->GetAttributeParameter(m_FloorAttribute);
    if (!pout)
        return;

    CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
    pout = (CKParameterOut *)CKGetObject(paramids[0]);
    if (pout)
        pout->SetValue(&geo);

    pout = (CKParameterOut *)CKGetObject(paramids[1]);
    if (pout)
        pout->SetValue(&moving);

    pout = (CKParameterOut *)CKGetObject(paramids[2]);
    if (pout)
        pout->SetValue(&type);

    pout = (CKParameterOut *)CKGetObject(paramids[3]);
    if (pout)
        pout->SetValue(&hiera);

    pout = (CKParameterOut *)CKGetObject(paramids[4]);
    if (pout)
        pout->SetValue(&first);
}

/*************************************************
Name:

Summary: Not Yet Documented

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
void FloorManager::RemoveFloorObject(CK3dEntity *ent)
{
    if (ent)
        ent->RemoveAttribute(m_FloorAttribute);
}

/*************************************************
Name:

Summary: Not Yet Documented

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
int FloorManager::GetFloorObjectCount()
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &floors = attman->GetAttributeListPtr(m_FloorAttribute);

    return floors.Size();
}

/*************************************************
Name:

Summary: Not Yet Documented

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
CK3dEntity *FloorManager::GetFloorObject(int pos)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &floors = attman->GetAttributeListPtr(m_FloorAttribute);

    return (CK3dEntity *)floors[pos];
}

/*************************************************
Name: GetCacheThreshold

Summary: Return the current threshold use by the cache system of the FloorManager

Return Value:
    A floating point value representing the threshold in all the 2 axis, x and z.

Remarks:
    Return the current threshold use by the cache system of the FloorManager. A cached point is
    reused if a new asked point fell in the threshold proximity of it.

{Group:Floors Cache Management}

See also: FloorManager, FloorManager::GetNearestFloors
*************************************************/
float FloorManager::GetCacheThreshold()
{
    return m_CacheThreshold;
}

/*************************************************
Name: SetCacheThreshold

Summary: Set the current threshold use by the cache system of the FloorManager

Arguments:
    t: A floating point value representing the threshold in all the 2 axis, x and z.

Remarks:
    Set the current threshold use by the cache system of the FloorManager. When a call to
    GetNearestFloor is performed, the position given is kept with the nearest face found from the
    nearest floor found. When GetNearestFloors is called again, if first test all the kept point,
    with a fixed threshold, to accelerate the detection when the object has not moved, or just a little.

{Group:Floors Cache Management}

See also: FloorManager, FloorManager::GetNearestFloors
*************************************************/
void FloorManager::SetCacheThreshold(float t)
{
    m_CacheThreshold = t;
}

/*************************************************
Name: GetCacheSize

Summary: Get the current number of positions kept by the cache.

Return Value:
    The current number of positions kept by the cache.

Remarks:
    Get the current number of positions kept by the cache.

{Group:Floors Cache Management}

See also: FloorManager, FloorManager::GetNearestFloors
*************************************************/
int FloorManager::GetCacheSize()
{
    return m_CacheSize;
}

/*************************************************
Name: SetCacheSize

Summary: Get the current number of positions kept by the cache.

Arguments:
    size: The current number of positions kept by the cache.

Remarks:
    Set the current number of positions kept by the cache. Warning :
    It clear the current cache as well...

{Group:Floors Cache Management}

See also: FloorManager, FloorManager::GetNearestFloors
*************************************************/
void FloorManager::SetCacheSize(int size)
{
    m_CacheSize = size;
}

CKERROR FloorManager::SequenceToBeDeleted(CK_ID *iIDs, int iCount)
{
    if (!m_FloorEdges.Size())
        return CK_OK;

    HashFloor2Edges::Iterator it = m_FloorEdges.Begin();
    while (it != m_FloorEdges.End())
    {

        if (it.GetKey()->IsToBeDeleted())
        {
            it = m_FloorEdges.Remove(it);
        }
        else
        {
            ++it;
        }
    }

    return CK_OK;
}

void FloorManager::ComputeEdges(CK3dEntity *iFloor, XArray<GeomEdge> &oEdges)
{
    oEdges.Resize(0);

    // first determine the edges of the floor
    CKMesh *mesh = iFloor->GetCurrentMesh();
    if (!mesh)
        return;

    // Gestion des matrices indirectes
    const VxMatrix &wm = iFloor->GetWorldMatrix();
    VxVector dx = wm[0];
    VxVector dy = wm[1];
    VxVector cp = CrossProduct(dx, dy);

    bool direct = true;
    if (DotProduct(cp, wm[2]) < 0.0f)
        direct = false;

    XHashTable<int, Edge> hash;

    CKDWORD fStride = 0;
    CKBYTE *p = mesh->GetFaceNormalsPtr(&fStride);
    XPtrStrided<VxVector> normals(p, fStride);

    // first add all the edges
    int faceCount = mesh->GetFaceCount();
    CKWORD *facePtr = mesh->GetFacesIndices();

    hash.Reserve(faceCount * 3);

    XHashTable<int, Edge>::Pair edgePair(hash.End(), 0);
    Edge newEdge;

    for (int f = 0; f < faceCount; ++f)
    {

        CKWORD va = facePtr[f * 3 + 0];
        CKWORD vb = facePtr[f * 3 + 1];
        CKWORD vc = facePtr[f * 3 + 2];

        // add the 3 edges of the face
        {
            newEdge.a = va;
            newEdge.b = vb;
            edgePair = hash.TestInsert(newEdge, 1);
            if (!edgePair.m_New)
            {
                (*edgePair.m_Iterator)++;
                // edgePair.m_Iterator.GetKey().fb = f;
            }
            else
            {
                edgePair.m_Iterator.GetKey().fa = f;
            }
        }

        {
            newEdge.a = vb;
            newEdge.b = vc;
            edgePair = hash.TestInsert(newEdge, 1);
            if (!edgePair.m_New)
            {
                (*edgePair.m_Iterator)++;
                // edgePair.m_Iterator.GetKey().fb = f;
            }
            else
            {
                edgePair.m_Iterator.GetKey().fa = f;
            }
        }

        {
            newEdge.a = vc;
            newEdge.b = va;
            edgePair = hash.TestInsert(newEdge, 1);
            if (!edgePair.m_New)
            {
                (*edgePair.m_Iterator)++;
                // edgePair.m_Iterator.GetKey().fb = f;
            }
            else
            {
                edgePair.m_Iterator.GetKey().fa = f;
            }
        }
    }

    // then keep the single edges
    // by creating geometric edges

    XHashTable<int, Edge>::Iterator it = hash.Begin();
    XHashTable<int, Edge>::Iterator itend = hash.End();

    // get the mesh positions
    CKDWORD stride;
    void *ptr = mesh->GetPositionsPtr(&stride);
    XPtrStrided<VxVector> positions(ptr, stride);

    while (it != itend)
    {
        if (*it == 1)
        {
            const Edge &e = it.GetKey();

            // add a new geom edge
            oEdges.Expand();
            GeomEdge &ge = oEdges.Back();

            VxVector va;
            iFloor->Transform(&va, &positions[e.a]);
            VxVector vb;
            iFloor->Transform(&vb, &positions[e.b]);
            VxVector fnormal;
            iFloor->TransformVector(&fnormal, &normals[e.fa]);

            if (fnormal.y <= EPSILON)
            { // this face is not facing the up direction

                // skip this edge
                oEdges.Expand(-1);
                ++it;
                continue;
            }

            // store the positions
            ge.a.Set(va.x, va.z);
            ge.b.Set(vb.x, vb.z);

            // create the plane

            VxVector vedge = vb - va;
            vedge.y = 0.0f;

            // calculate the outward normal
            VxVector norm = CrossProduct(fnormal, vedge);

            if (!direct)
                norm = -norm;

            norm.y = 0.0f;
            norm.Normalize();

            ge.length = Magnitude(vedge);
            ge.invLength = 1.0f / ge.length;

            // the plane itself
            ge.normal.Set(norm.x, norm.z);
            ge.D = -ge.normal.Dot(ge.a);
        }

        ++it;
    }
}

CKBOOL FloorManager::SlideOnEdges(VxVector &ioPosition, float iRadius, const XArray<GeomEdge> &iEdges, CK3dEntity *iFloor, CKAttributeType iExcludeAttribute)
{
    CKBOOL touched = FALSE;

    /*
        // Drawing of the floor
        CKRenderManager* rm = m_Context->GetRenderManager();

        {
            for (XArray<GeomEdge>::Iterator it = iEdges.Begin(); it != iEdges.End(); ++it) {
                GeomEdge& ge = *it;

                VxPlane plane;
                plane.Create(VxVector(ge.normal.x,0.0f,ge.normal.y),VxVector(ge.a.x,0.0f,ge.a.y));

                rm->RegisterPlane(plane,VxVector((ge.a.x+ge.b.x)*0.5f,ioPosition.y,(ge.a.y+ge.b.y)*0.5f),0xff00ff00,0.0f);
            }
            rm->RegisterSphere(VxSphere(ioPosition,2.0f),0xff00ff00,0.0f);
        }
    */

    Vx2DVector pos2D(ioPosition.x, ioPosition.z);

    for (XArray<GeomEdge>::Iterator it = iEdges.Begin(); it != iEdges.End(); ++it)
    {

        GeomEdge &ge = *it;

        float dist = ge.normal.Dot(pos2D) + ge.D;

        if (dist >= iRadius) // not in the edge area
            continue;
        if (dist <= 0.0f) // bad side of the edge
            continue;

        Vx2DVector leftEdge = ge.a - ge.b;
        leftEdge *= ge.invLength;

        float distLeft = leftEdge.Dot(pos2D - ge.a);

        if (distLeft >= iRadius)
            continue;

        if (distLeft >= 0)
        { // touching the left corner

            float distanceToMove = iRadius - sqrtf(distLeft * distLeft + dist * dist);
            if (distanceToMove > 0.0f)
            {
                pos2D += (distanceToMove) * (pos2D - ge.a).Normalize();
                touched = TRUE;
            }
            continue;
        }

        float distRight = -(ge.length + distLeft);

        if (distRight >= iRadius)
            continue;

        if (distRight >= 0)
        { // touching the right corner
            float distanceToMove = iRadius - sqrtf(distRight * distRight + dist * dist);
            if (distanceToMove > 0.0f)
            {
                pos2D += (distanceToMove) * (pos2D - ge.b).Normalize();
                touched = TRUE;
            }
            continue;
        }

        // else in the limit of the plane

        ///
        // we need to check if we are not going to another floor...
        {
            Vx2DVector goingPosition2D = pos2D - iRadius * ge.normal;
            VxVector goingPosition(goingPosition2D.x, ioPosition.y, goingPosition2D.y);

            CKFloorPoint fp;
            CK_FLOORNEAREST fn = GetNearestFloors(goingPosition, &fp, iFloor);
            if (fn != CKFLOOR_NOFLOOR)
            {

                CK3dEntity *nextFloor = NULL;
                if (fn == CKFLOOR_DOWN)
                    nextFloor = (CK3dEntity *)CKGetObject(fp.m_DownFloor);
                else
                    nextFloor = (CK3dEntity *)CKGetObject(fp.m_UpFloor);

                // we do not take into account floor with the exclude attribute
                // (if the attribute is != -1)
                if ((iExcludeAttribute == -1) || !nextFloor->HasAttribute(iExcludeAttribute))
                    continue; // we're going somewhere, so do not constrain
            }
        }

        pos2D += (iRadius - dist) * ge.normal;

        // The object has been touched
        touched = TRUE;
    }

    ioPosition.x = pos2D.x;
    ioPosition.z = pos2D.y;

    return touched;
}

CKBOOL FloorManager::ConstrainToFloor(const VxVector &iPosition, float iRadius, VxVector *oPosition, CKAttributeType iExcludeAttribute)
{
    CKBOOL touched = FALSE;

    CKFloorPoint fp;
    CK_FLOORNEAREST fn = GetNearestFloors(iPosition, &fp);
    if (fn != CKFLOOR_NOFLOOR)
    { // we are over a floor

        // get the floor
        CK_ID floorID = NULL;
        switch (fn)
        {
        case CKFLOOR_DOWN:
            floorID = fp.m_DownFloor;
            break;
        case CKFLOOR_UP:
            floorID = fp.m_UpFloor;
            break;
        }

        CK3dEntity *floor = CK3dEntity::Cast(CKGetObject(floorID));
        if (floor)
        {

            // If the floor must not be considered, we exit there
            if ((iExcludeAttribute != -1) && (floor->HasAttribute(iExcludeAttribute)))
            {
                CK_FLOORNEAREST fn = GetNearestFloors(iPosition, &fp, floor);
                if (fn == CKFLOOR_NOFLOOR)
                    return FALSE;

                // we are over a floor
                // get the floor
                CK_ID floorID = NULL;
                switch (fn)
                {
                case CKFLOOR_DOWN:
                    floorID = fp.m_DownFloor;
                    break;
                case CKFLOOR_UP:
                    floorID = fp.m_UpFloor;
                    break;
                }

                floor = CK3dEntity::Cast(CKGetObject(floorID));
                if (!floor)
                    return FALSE;

                // If the floor must not be considered, we exit there
                if ((iExcludeAttribute != -1) && (floor->HasAttribute(iExcludeAttribute)))
                {
                    return FALSE;
                }
            }

            CKMesh *floorMesh = floor->GetCurrentMesh();

            // get the cached edges
            EdgesArray *edges = m_FloorEdges.FindPtr(floorMesh);

            if (!edges)
            { // we need to recalculate the edges
                EdgesArray newEdges;
                HashFloor2Edges::Iterator it = m_FloorEdges.Insert(floorMesh, newEdges);

                edges = &(*it);

                // the computing
                ComputeEdges(floor, *edges);
            }

            ///
            // Now the sliding

            // transform the position in the ref of the floor
            VxVector lpos = iPosition;

            touched = SlideOnEdges(lpos, iRadius, *edges, floor, iExcludeAttribute);

            if (touched)
            { // sets back to the world the new position
                *oPosition = lpos;
            }
        }
    }

    return touched;
}

CKBOOL FloorManager::ConstrainToFloor(const VxVector &iOldPosition, const VxVector &iPosition, float iRadius, VxVector *oPosition, CKAttributeType iExcludeAttribute)
{
    VxVector dir = iPosition - iOldPosition;

    float dist = Magnitude(dir);

    if (dist < iRadius)
    {
        return ConstrainToFloor(iPosition, iRadius, oPosition, iExcludeAttribute);
    }
    else
    {

        // we have to reduce a little the radius because, if we have been constrained
        // the past frame, the old position surely is at an exact radius distance
        // from the edge, so the test could miss it...
        float reducedRadius = iRadius * 0.99f; // Hard coded reduced factor

        int steps = (int)(dist / reducedRadius);

        // calculate the step vector
        dir *= reducedRadius / dist;

        VxVector pos = iOldPosition;

        // test intermediate positions
        while (steps--)
        {
            pos += dir;

            if (ConstrainToFloor(pos, iRadius, oPosition, iExcludeAttribute))
            { // has been constrained
                return TRUE;
            }
        }

        // we now have to test the last position
        return ConstrainToFloor(iPosition, iRadius, oPosition, iExcludeAttribute);
    }
}

CK_FLOORNEAREST FloorManager::GetNearestFloor(const VxVector &iPosition, CK3dEntity **oFloor, int *oFaceIndex, VxVector *oNormal, float *oDistance, CK3dEntity *iExcludeFloor)
{
    CKFloorPoint fp;

    CK_FLOORNEAREST res = GetNearestFloors(iPosition, &fp, iExcludeFloor);

    switch (res)
    {
    case CKFLOOR_UP:
        *oFloor = (CK3dEntity *)m_Context->GetObject(fp.m_UpFloor);
        *oFaceIndex = fp.m_UpFaceIndex;
        *oNormal = fp.m_UpNormal;
        *oDistance = fp.m_UpDistance;
        break;

    case CKFLOOR_DOWN:
        *oFloor = (CK3dEntity *)m_Context->GetObject(fp.m_DownFloor);
        *oFaceIndex = fp.m_DownFaceIndex;
        *oNormal = fp.m_DownNormal;
        *oDistance = fp.m_DownDistance;
        break;

    case CKFLOOR_NOFLOOR:
        *oFloor = NULL;
        *oFaceIndex = -1;
        oNormal->Set(0, 0, 0);
        *oDistance = 100000000.0f;
        break;
    }

    return res;
}

// {secret}
CKGUID GetFloorManagerGuid()
{
    return FLOOR_MANAGER_GUID;
}
