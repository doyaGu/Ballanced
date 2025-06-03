// PortalsManager.cpp : Defines the entry point for the DLL application.
//

#include "CKAll.h"

#if defined(macintosh)
#include "qhull.h"
using namespace QHULL;
#endif

#if !defined(macintosh) && defined(__cplusplus)
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>

#include "qhull/qhull.h"
#include "qhull/mem.h"
#include "qhull/qset.h"
#include "qhull/geom.h"
#include "qhull/merge.h"
#include "qhull/poly.h"
#include "qhull/io.h"
#include "qhull/stat.h"

#if defined(macintosh)
    namespace QHULL
    {
#endif
        char qh_version[] = "qad 2000/8/1"; /* used for error messages */
#if defined(macintosh)
    } // end namespace QHULL
#endif

#if !defined(macintosh) && defined(__cplusplus)
}
#endif

#include "PortalsManager.h"

// This is the constructor of a class that has been exported.
// see PortalsManager.h for the class definition
PortalsManager::PortalsManager(CKContext *context) : CKBaseManager(context, PORTALS_MANAGER_GUID, "Portals Manager")
{
    context->RegisterNewManager(this);

    m_Active = 0;
    m_RecursivityLevel = 0;
    m_DebugCameras = NULL;
    m_Behavior = NULL;
    m_TransPlaceObj = NULL;
}

CKERROR PortalsManager::OnCKInit()
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    // Occluder Attribute
    m_OccludersAttribute = attman->RegisterNewAttributeType("Occluder", CKPGUID_MESH, CKCID_3DENTITY);
    attman->SetAttributeCategory(m_OccludersAttribute, "Optimizations");

    m_HidedLastFrame.Resize(0);

    // we clear the HashTable of occluders
    ClearOccluders();

    return CK_OK;
}

void PortalsManager::ClearOccluders()
{
    HMesh2Occluder::Iterator it = m_Mesh2Occluder.Begin();
    HMesh2Occluder::Iterator itend = m_Mesh2Occluder.End();
    while (it != itend)
    {
        delete *it;
        *it = NULL;

        ++it;
    }

    m_Mesh2Occluder.Clear();
}

CKERROR PortalsManager::OnCKReset()
{
    // if it was active
    if (m_Active)
    {
        // Show all places
        OnCKPause();
    }

    m_Active = 0;
    m_RecursivityLevel = 0;
    m_DebugCameras = NULL;
    m_Behavior = NULL;
    m_TransPlaceObj = NULL;

    m_HidedLastFrame.Resize(0);

    return CK_OK;
}

CKERROR PortalsManager::OnCKPause()
{
    if (!m_Active)
        return CK_OK;

    // Show all places
    int count = m_Context->GetObjectsCountByClassID(CKCID_PLACE);
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PLACE);

    int i;
    for (i = 0; i < count; ++i)
    {
        CKPlace *place = (CKPlace *)CKGetObject(ids[i]);
        if (place && place->GetPortalCount())
        {
            place->Show(CKSHOW);
            place->ViewportClip().Clear();
        }
    }

    // and show all given trans place objects
    if (m_TransPlaceObj)
    {
        int transPlaceObjCount = m_TransPlaceObj->GetObjectCount();
        for (i = 0; i < transPlaceObjCount; ++i)
        {
            CK3dEntity *tpObj = (CK3dEntity *)m_TransPlaceObj->GetObject(i);
            tpObj->Show();
        }
    }

    return CK_OK;
}

CKERROR PortalsManager::PostLoad()
{
    CKAttributeManager *am = m_Context->GetAttributeManager();
    XASSERT(am);

    const XObjectPointerArray occludersList = am->GetGlobalAttributeListPtr(m_OccludersAttribute);
    for (CKObject **it = occludersList.Begin(); it != occludersList.End(); ++it)
    {
        CK3dEntity *ent = (CK3dEntity *)*it;
        if (!ent)
            continue;

        // we force the creation of all the occluders present
        RegisterOccluder(ent);
    }

    return CK_OK;
}

void PortalsManager::ShowHidedLastFrame()
{
    for (CKObject **it = m_HidedLastFrame.Begin(); it != m_HidedLastFrame.End(); ++it)
    {
        CK3dEntity *ent = (CK3dEntity *)*it;

        ent->Show(CKSHOW);
    }
}

CKERROR PortalsManager::OnPreRender(CKRenderContext *dev)
{
    ShowHidedLastFrame();

    ManageOccluders(dev);
    //	RenderOccluders(dev);

    if (!m_Active || !m_Context->IsPlaying())
        return CK_OK;

    m_RenderedPlaces = NULL;
    m_RenderedTransPlaceObjs = NULL;
    m_TransPlaceObj = NULL;

    if (m_Behavior)
    {
        XObjectArray *oarray = *(XObjectArray **)m_Behavior->GetOutputParameterWriteDataPtr(0);
        oarray->Resize(0);

        CKParameterOut *pout;

        // clear rendered places pOut collection
        pout = m_Behavior->GetOutputParameter(1);
        if (pout)
        {
            m_RenderedPlaces = *(XObjectArray **)pout->GetReadDataPtr(FALSE);
            m_RenderedPlaces->Resize(0);
        }
        // clear rendered trans-places pOut collection
        pout = m_Behavior->GetOutputParameter(2);
        if (pout)
        {
            m_RenderedTransPlaceObjs = *(XObjectArray **)pout->GetReadDataPtr(FALSE);
            m_RenderedTransPlaceObjs->Resize(0);
        }

        m_TransPlaceObj = (CKGroup *)m_Behavior->GetInputParameterObject(2);
        if (m_TransPlaceObj)
        {
            int transPlaceObjCount = m_TransPlaceObj->GetObjectCount();
            for (int i = 0; i < transPlaceObjCount; ++i)
            {
                CK3dEntity *tpObj = (CK3dEntity *)m_TransPlaceObj->GetObject(i);
                if (!CKIsChildClassOf(tpObj, CKCID_3DENTITY))
                {
                    m_Context->OutputToConsoleEx("The TransPlace Group passed to the building block \"Portal Management\" contain invalid objects");
                    m_TransPlaceObj = NULL;
                }
            }
        }
    }

    // we first hide all portals
    int count = m_Context->GetObjectsCountByClassID(CKCID_PLACE);
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PLACE);

    int i;
    for (i = 0; i < count; ++i)
    {
        CKPlace *place = (CKPlace *)CKGetObject(ids[i]);
        if (place && place->GetPortalCount())
        {
            place->Show(CKHIERARCHICALHIDE);
            place->ViewportClip().Clear();
        }
    }

    // and we hide all the given trans place objects
    if (m_TransPlaceObj)
    {
        int transPlaceObjCount = m_TransPlaceObj->GetObjectCount();
        for (i = 0; i < transPlaceObjCount; ++i)
        {
            CK3dEntity *tpObj = (CK3dEntity *)m_TransPlaceObj->GetObject(i);
            tpObj->Show(CKHIERARCHICALHIDE);
        }
    }

    // we clear the caller stack
    m_CallerStack.Resize(0);

    // we now iterate on the cameras to render from
    if (m_DebugCameras)
    {
        for (int i = 0; i < m_DebugCameras->GetObjectCount(); ++i)
        {
            ShowPlacesFrom((CKCamera *)m_DebugCameras->GetObject(i));
        }
    }
    else
    {
        CKRenderContext *rc = m_Context->GetPlayerRenderContext();
        CKCamera *cam = rc->GetAttachedCamera();

        if (cam)
            ShowPlacesFrom(cam);
    }

    return CK_OK;
}

void PortalsManager::ShowPlacesFrom(CKCamera *cam)
{
    // we first find the starting portals including the current viewer position
    int count = m_Context->GetObjectsCountByClassID(CKCID_PLACE);
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PLACE);

    // We take the position of the near plane of the camera
    VxVector pos;
    VxVector lpos(0, 0, cam->GetFrontPlane());
    cam->Transform(&pos, &lpos);

    // We calculate the projection matrix
    VxMatrix projmatrix;
    cam->ComputeProjectionMatrix(projmatrix);

    Vx3DMultiplyMatrix4(m_ViewProjectionMatrix, projmatrix, cam->GetInverseWorldMatrix());

    VxBbox box;
    VxRect screen(0, 0, 1, 1);
    CKBOOL inplace = FALSE;

    XObjectArray *oarray = NULL;
    if (m_Behavior)
    {
        oarray = *(XObjectArray **)m_Behavior->GetOutputParameterWriteDataPtr(0);
    }

    int i, j;
    for (i = 0; i < count; ++i)
    {
        CKPlace *place = (CKPlace *)CKGetObject(ids[i]);
        if (!place)
            continue;
        if (!place->GetPortalCount())
            continue;

        box = place->GetHierarchicalBox();

        if (box.VectorIn(pos))
        {
            // we add the place to the current places collection
            if (oarray)
                oarray->PushBack(place->GetID());

            inplace = TRUE;
            RecursePlacesFrom(0, place, screen);
        }
    }

    // The viewer is in no place : we show them all
    if (!inplace)
    {

        // show all Places
        for (i = 0; i < count; ++i)
        {
            CKPlace *place = (CKPlace *)CKGetObject(ids[i]);
            if (place && place->GetPortalCount())
            {
                place->Show(CKSHOW);
                if (m_RenderedPlaces)
                    m_RenderedPlaces->PushBack(CKOBJID(place));
            }
        }

        // and we show all the TransPlace objects
        if (m_TransPlaceObj)
        {
            int transPlaceObjCount = m_TransPlaceObj->GetObjectCount();
            for (j = 0; j < transPlaceObjCount; ++j)
            {
                CK3dEntity *tpObj = (CK3dEntity *)m_TransPlaceObj->GetObject(j);
                tpObj->Show(CKSHOW);
                if (m_RenderedTransPlaceObjs)
                    m_RenderedTransPlaceObjs->PushBack(CKOBJID(tpObj));
            }
        }
    }

    if (m_Behavior)
    {
        m_Behavior->GetOutputParameter(0)->DataChanged();
        CKParameterOut *pout = m_Behavior->GetOutputParameter(1);
        if (pout)
            pout->DataChanged();
    }
}

void PortalsManager::RecursePlacesFrom(int level, CKPlace *p, VxRect currentextents)
{
    // we test if the place was already visited
    if (m_CallerStack.IsHere(p))
        return;

    // We get the caller
    CKPlace *placefrom = m_CallerStack.Size() ? m_CallerStack.Back() : 0;

    // We push the caller
    m_CallerStack.PushBack(p);

    // we update the viewport clipping of the portal
    VxRect &vc = p->ViewportClip();
    if (!m_DebugCameras)
    { // only if with the current camera
        if (vc.IsEmpty())
            vc = currentextents;
        else
            vc.Merge(currentextents);
    }

    // if not, we show it
    p->Show(CKSHOW);
    if (m_RenderedPlaces)
        m_RenderedPlaces->AddIfNotHere(CKOBJID(p));

    VxBbox box;
    VxMatrix worldproj;
    VxRect screen(0, 0, 1, 1);
    VXCLIP_FLAGS dummy, dummy2;
    VxRect extents;
    CKBOOL inside;

    // and we show all the trans place objects include in this place
    if (m_TransPlaceObj)
    {
        int transPlaceObjCount = m_TransPlaceObj->GetObjectCount();
        for (int i = 0; i < transPlaceObjCount; ++i)
        {
            CK3dEntity *tpObj = (CK3dEntity *)m_TransPlaceObj->GetObject(i);
            if (tpObj->IsVisible())
                continue; // will be shown anyway so skip further test
            box = tpObj->GetHierarchicalBox();

            // test if object is in the place
            if (VxIntersect::AABBAABB(box, p->GetHierarchicalBox()))
            {

                // test if object extends are visible through the current extends
                inside = VxTransformBox2D(m_ViewProjectionMatrix, box, &screen, &extents, dummy, dummy2);
                if (inside)
                {
                    if (extents.Clip(currentextents))
                    {
                        tpObj->Show();
                        if (m_RenderedTransPlaceObjs)
                            m_RenderedTransPlaceObjs->PushBack(CKOBJID(tpObj));
                    }
                }
            }
        }
    }

    // TODO : this should be a method of CKPlace for efficiency
    if (m_RecursivityLevel && (level == m_RecursivityLevel - 1))
        return;

    for (int i = 0; i < p->GetPortalCount(); ++i)
    {
        CK3dEntity *portal = NULL;
        CKPlace *next = p->GetPortal(i, &portal);

        // we would go back from where we came
        if (next == placefrom)
            continue;

        // the next place is already processed, we stop right here
        //	if (next->CKObject::IsVisible()) continue;

        if (portal)
        { // portal : we must check if the portal is in view frustrum
            if (portal->IsActiveInCurrentScene())
            { // portal must be active
                if (!portal->GetCurrentMesh())
                    box.SetCenter(VxVector::axis0(), VxVector::axis1());
                else
                    box = portal->GetBoundingBox(TRUE);

                Vx3DMultiplyMatrix4(worldproj, m_ViewProjectionMatrix, portal->GetWorldMatrix());

                // The 2D Projection and extents calculation
                inside = VxTransformBox2D(worldproj, box, &screen, &extents, dummy, dummy2);

                if (inside)
                { // If the object is enerily culled, we skip it

                    // we must see if the extents are currently visibles
                    if (extents.Clip(currentextents))
                    {
                        RecursePlacesFrom(level + 1, next, extents);
                    }
                }
            }
        }
        else
        { // No portal : the place is visible for sure
            RecursePlacesFrom(level + 1, next, currentextents);
        }
    }

    // We pop the caller
    m_CallerStack.PopBack();
}

void PortalsManager::Activate(CKBOOL a)
{
    m_Active = a;

    if (!a)
    {
        // Show all places
        int count = m_Context->GetObjectsCountByClassID(CKCID_PLACE);
        CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PLACE);

        for (int i = 0; i < count; ++i)
        {
            CKPlace *place = (CKPlace *)m_Context->GetObject(ids[i]);
            if (place && place->GetPortalCount())
                place->Show(CKSHOW);
        }
    }
}

// Automatically Generates All Portals
void PortalsManager::AutomaticallyGeneratePortals()
{

    int placeCount = m_Context->GetObjectsCountByClassID(CKCID_PLACE);
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PLACE);

    CKLevel *level = m_Context->GetCurrentLevel();
    CKScene *scene = m_Context->GetCurrentScene();

    CKPlace *p1, *p2;
    VxBbox p1box, p2box;
    VxMatrix fitmat;
    XString prefix = "Portal ";
    int portalCount;

    float t = 0.1f; // Threshold to decide that 2 places are neighbours

    // Parse all places against each other
    for (int i = 0; i < placeCount; ++i)
    {
        p1 = (CKPlace *)m_Context->GetObject(ids[i]);
        if (!p1)
            continue;
        p1box = p1->GetHierarchicalBox();
        for (int j = 0; j < placeCount; ++j)
        {
            if (j == i)
                continue;
            p2 = (CKPlace *)m_Context->GetObject(ids[j]);
            if (!p2)
                continue;

            p2box = p2->GetHierarchicalBox();
            if (p1box.Max.x + t < p2box.Min.x)
                continue;
            if (p2box.Max.x + t < p1box.Min.x)
                continue;
            if (p1box.Max.z + t < p2box.Min.z)
                continue;
            if (p2box.Max.z + t < p1box.Min.z)
                continue;
            if (p1box.Max.y + t < p2box.Min.y)
                continue;
            if (p2box.Max.y + t < p1box.Min.y)
                continue;

            // ok, the 2 places are neighbours
            // now look if there were not already a portal between them
            portalCount = p2->GetPortalCount();

            int pindex;
            for (pindex = 0; pindex < portalCount; ++pindex)
            {
                if (p2->GetPortal(pindex, NULL) == p1)
                    break;
            }
            if (pindex < portalCount)
                continue;

            // Compute best fitting matrix
            if (p1->ComputeBestFitBBox(p2, fitmat))
            {

                // if there's common points then create a portal
                // at correct position
                XString portalName;
                portalName << prefix << p1->GetName() << "-" << p2->GetName();
                CK3dEntity *portal = (CK3dEntity *)m_Context->CreateObject(CKCID_3DENTITY, portalName.Str());
                if (portal)
                {
                    portal->SetFlags(portal->GetFlags() | CK_3DENTITY_PORTAL);
                    portal->SetWorldMatrix(fitmat);
                    p1->AddPortal(p2, portal);
                    level->AddObject(portal);
                    scene->AddObjectToScene(portal);
                    scene->Activate(portal, TRUE);
                }
            }
        }
    }
}

PortalsManager::~PortalsManager()
{
    ClearOccluders();
}

///////////////////////////////////////////////////////
// Occluders Part
///////////////////////////////////////////////////////

PortalsManager::Occluder *PortalsManager::RegisterOccluder(CK3dEntity *iEntity)
{
    // the occluding mesh
    CKMesh *mesh = NULL;

    // Get the mesh from the attribute
    CKParameterOut *pout = iEntity->GetAttributeParameter(m_OccludersAttribute);
    if (pout)
        mesh = (CKMesh *)pout->GetValueObject(FALSE);

    // No mesh ? we then get the current one
    if (!mesh)
        mesh = iEntity->GetCurrentMesh();

    // Still no mesh ? : exit
    if (!mesh)
        return NULL;

    // retreive info about the mesh from the hashs
    HMesh2Occluder::Pair p = m_Mesh2Occluder.TestInsert(mesh, NULL);
    if (p.m_New)
    { // not yet in the table
        *(p.m_Iterator) = new Occluder(mesh);
    }
    return *(p.m_Iterator);
}

void PortalsManager::ConstructOccludingVolume(CK3dEntity *iEntity, XArray<VxPlane> &oVolume, const VxFrustum &iFrustum)
{
    // Clear the volumes
    oVolume.Resize(0);

    // we retreive the occluding information for this entity from the hash
    // (Hopefully already calculated)
    Occluder *occluder = RegisterOccluder(iEntity);

    occluder->ComputeSilhouette(iEntity, iFrustum, m_SilhouetteEdges, oVolume);

    //	CKRenderContext* rc = m_Context->GetPlayerRenderContext();
    //	occluder->RenderHull(rc,iEntity);
    //	occluder->RenderSilhouette(rc, m_SilhouetteEdges);
}

bool PortalsManager::IsObjectOccluded(CK3dEntity *iEntity, const XArray<VxPlane> &iVolume)
{
    // first we test the sphere
    VxVector center;
    iEntity->GetBaryCenter(&center);

    float radius = iEntity->GetRadius();

    bool clipped = false;

    XArray<VxPlane>::Iterator it;
    for (it = iVolume.Begin(); it != iVolume.End(); ++it)
    {
        const VxPlane &plane = *it;

        float d = plane.Classify(center);
        if (d > -radius)
            if (d > 0.0f)
                return false;
            else
                clipped = true;
    }

    if (!clipped)
        return true;

    // we should now test the box (if one plane were crossing)
    // maybe create another XArray<VxPlane> of crossing only planes
    // to test with the OBB

    const VxBbox &box = iEntity->GetBoundingBox(TRUE);
    const VxMatrix &wm = iEntity->GetWorldMatrix();

    for (it = iVolume.Begin(); it != iVolume.End(); ++it)
    {
        const VxPlane &plane = *it;

        float d = plane.Classify(box, wm);
        if (d >= 0.0f)
            return false;
    }

    return true;
}

void PortalsManager::ManageOccluders(CKRenderContext *iRC)
{
    m_HidedLastFrame.Resize(0);

    // we retreive the lisyt of occluders (in the current scene)
    CKAttributeManager *am = m_Context->GetAttributeManager();
    const XObjectPointerArray &occludersList = am->GetAttributeListPtr(m_OccludersAttribute);
    if (!occludersList.Size())
        return;

    CKCamera *cam = iRC->GetAttachedCamera();
    const VxMatrix &worldMatrix = cam->GetWorldMatrix();
    int width, height;
    cam->GetAspectRatio(width, height);
    VxFrustum frustum(worldMatrix[3], worldMatrix[0], worldMatrix[1], worldMatrix[2], cam->GetFrontPlane(), cam->GetBackPlane(), cam->GetFov(),
                      (float)height / width);

    int count = m_Context->GetObjectsCountByClassID(CKCID_3DOBJECT);
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_3DOBJECT);

    // TODO : this occluding code is based on last frame
    // frustum visibility : in a perfect world, it should work on a
    // PVS given by the Hierarchical frustum culling

    // we should sort the occluders by Zdist, occupation of the screen
    // if very small, do not consider it
    // we should test too if an occluder is not occluded itself
    // we should mark the object that are hidden to not iterate on all the
    // list
    // we should make the whole thing scalable with a max number of occluder to consider
    // and a possible simplification of the silhouette if possible (for a sphere very usefull)

    for (CKObject **it = occludersList.Begin(); it != occludersList.End(); ++it)
    {
        CK3dEntity *ent = (CK3dEntity *)*it;
        if (!ent)
            continue;

        if (ent->IsAllOutsideFrustrum()) // this occluder was not visible last frame : not a good potentail occluder
            continue;

        // first construct the volume

        ConstructOccludingVolume(ent, m_OccludingVolume, frustum);

        if (!m_OccludingVolume.Size()) // there is no relevant volume
            continue;

        // we try to hide all the entites that are in the occluding volume
        for (int i = 0; i < count; ++i)
        {
            CK3dEntity *currentEntity = (CK3dEntity *)m_Context->GetObject(ids[i]);
            if (!currentEntity)
                continue;

            if (currentEntity->IsAllOutsideFrustrum()) // this entity was not visible last frame : nothing to occlude
                continue;

            if (currentEntity == (*it))
                continue;

            if (IsObjectOccluded(currentEntity, m_OccludingVolume))
            { // Object occluded

                currentEntity->Show(CKHIDE);
                // TODO : mark the entity that have already been occluded
                // (bitset, hash, etc.)
                m_HidedLastFrame.AddIfNotHere(currentEntity);
            }
        }
    }
}

void PortalsManager::RenderOccluders(CKRenderContext *iRC)
{
    // TODO
}

CKBOOL PortalsManager::Occluder::IsPlanar(XPtrStrided<VxVector> iPositions, const int iCount)
{
    if (iCount < 3)
        return TRUE;

    // we create the normal
    VxPlane normal(iPositions[0], iPositions[1], iPositions[2]);

    for (int i = 3; i < iCount; ++i)
    {
        if (normal.Distance(iPositions[i]) > EPSILON)
            return FALSE;
    }

    return TRUE;
}

void PortalsManager::Occluder::ComputePlanarHull(CKMesh *iMesh)
{
    // TODO : weld

    XArray<Edge> edges;

    // Get mesh topology

    // vertices
    CKDWORD vStride = 0;
    void *tmp = iMesh->GetPositionsPtr(&vStride);
    XPtrStrided<VxVector> positions(tmp, vStride);
    int vcount = iMesh->GetVertexCount();

    // face normals
    CKDWORD nStride = 0;
    CKBYTE *fn = iMesh->GetFaceNormalsPtr(&nStride);
    XPtrStrided<VxVector> faceNormals(fn, nStride);

    // face indices
    CKWORD *indices = iMesh->GetFacesIndices();
    int fcount = iMesh->GetFaceCount();

    XArray<Edge>::Iterator it;
    for (int f = 0; f < fcount; ++f, indices += 3)
    {
        CKDWORD i0 = indices[0];
        CKDWORD i1 = indices[1];
        CKDWORD i2 = indices[2];

        Edge e1, e2, e3;
        XMinMax(i0, i1, e1.vertices[0], e1.vertices[1]);
        XMinMax(i1, i2, e2.vertices[0], e2.vertices[1]);
        XMinMax(i2, i0, e3.vertices[0], e3.vertices[1]);

        it = edges.Find(e1);
        if (it == edges.End())
        {
            e1.normal = CrossProduct((positions[i1] - positions[i0]), faceNormals[f]);
            e1.references = 1;
            edges.PushBack(e1);
        }
        else
            (*it).references++;

        it = edges.Find(e2);
        if (it == edges.End())
        {
            e2.normal = CrossProduct((positions[i2] - positions[i1]), faceNormals[f]);
            e2.references = 1;
            edges.PushBack(e2);
        }
        else
            (*it).references++;

        it = edges.Find(e3);
        if (it == edges.End())
        {
            e3.normal = CrossProduct((positions[i0] - positions[i2]), faceNormals[f]);
            e3.references = 1;
            edges.PushBack(e3);
        }
        else
            (*it).references++;
    }

    XAP<int> id2vertices(new int[vcount]); // Is it sufficient ???
    int i;
    for (i = 0; i < vcount; ++i)
        id2vertices[i] = -1;

    for (XArray<Edge>::Iterator et = edges.Begin(); et != edges.End(); ++et)
    {
        if (et->references == 1)
        {
            // remap the vertex index
            for (i = 0; i < 2; ++i)
            {
                if (id2vertices[et->vertices[i]] == -1)
                {
                    id2vertices[et->vertices[i]] = m_HullVertices.Size();
                    m_HullVertices.PushBack(positions[et->vertices[i]]);
                }
                et->vertices[i] = id2vertices[et->vertices[i]];
            }

            // This edge must be kept
            m_HullEdges.PushBack(*et);
        }
    }

    Face face;
    face.normal = faceNormals[0];
    m_HullFaces.PushBack(face);
}

void FindCommonPoints(CKDWORD *iV1, int iCount1, CKDWORD *iV2, int iCount2, CKDWORD &oC1, CKDWORD &oC2)
{
    oC1 = 0xffffffff;
    for (int i = 0; i < iCount1; ++i)
    {
        for (int j = 0; j < iCount2; ++j)
        {
            if (iV1[i] == iV2[j])
            {
                if (oC1 == -1)
                {
                    oC1 = iV1[i];
                }
                else
                {
                    oC2 = iV1[i];
                    break;
                }
            }
        }
    }
}

// Ctor : Compute the Convex Hull of the occluder
PortalsManager::Occluder::Occluder(CKMesh *iMesh)
{
    // Get mesh topology
    CKDWORD vStride = 0;
    void *tmp = iMesh->GetPositionsPtr(&vStride);
    XPtrStrided<VxVector> positions(tmp, vStride);
    int numpoints = iMesh->GetVertexCount();

    // Test if the mesh is a plane
    m_Plane = IsPlanar(positions, numpoints);

    if (m_Plane)
    { // Special case of a plane object
        ComputePlanarHull(iMesh);
        return;
    }

    // sprintf (flags, "qhull s Tcv ");
    char flags[250];

    // TODO : see if it is necesssary to "weld" the common vertices
    // before the computation of the hull

    // QHull code
    coordT *points = new coordT[numpoints * 3];

    for (int i = 0; i < numpoints; ++i)
    {
        points[i * 3 + 0] = positions[i].x;
        points[i * 3 + 1] = positions[i].y;
        points[i * 3 + 2] = positions[i].z;
    }

    int numfaces = iMesh->GetFaceCount();

    float randomEps = 1e-12f;
    bool freeMem = false;
    int exitcode = -1;
    while (randomEps < 0.02f)
    {

        if (freeMem)
        {                                                                    // second time
            sprintf(flags, "qhull Qs QJ%G C-0 Pp W1e-6 E1.0e-8", randomEps); // "qhull QbB Pp"

            qh_freeqhull(!qh_ALL); // free memory from precedent call
        }
        else
        { // first time
            sprintf(flags, "qhull Qs Pp C-0 W1e-6 E1.0e-8 Tv");
        }
        exitcode = qh_new_qhull(3, numpoints, points, False, flags, NULL, stderr);
        freeMem = true;

        if (!exitcode) // success
            break;

        // we increment the epsilon
        randomEps = (randomEps + 1e-12f) * 1.2f;
    }

    if (exitcode >= 0)
    { // if no error

        unsigned int maxID = 0;

        facetT *facet;
        FORALLfacets
        {
            if (facet->id > maxID)
            {
                maxID = facet->id;
            }
        }
        ++maxID;

        XAP<int> id2vertices(new int[numpoints]); // Is it sufficient ???
        XAP<int> id2faces(new int[maxID]);        // Is it sufficient ???

        {
            for (unsigned int i = 0; i < maxID; ++i)
            {
                id2faces[i] = -1;
            }
        }

        ///
        // Vertices iteration
        vertexT *vertex; /* set by FORALLvertices */
        int index = 0;
        FORALLvertices
        {
            m_HullVertices.Expand();
            VxVector &point = m_HullVertices.Back();

            point.x = vertex->point[0];
            point.y = vertex->point[1];
            point.z = vertex->point[2];

            id2vertices[vertex->id] = index++;
        }

        ///
        // Facets iteration

        int facetCount = qh num_facets;
        m_HullFaces.Reserve(facetCount + 2);

        FORALLfacets
        {

            id2faces[facet->id] = m_HullFaces.Size();

            // we add a face
            m_HullFaces.PushBack(Face());
            Face &face = *(m_HullFaces.End() - 1);

            // store its normal
            for (int k = 0; k < qh hull_dim; k++)
            {
                face.normal[k] = facet->normal[k];
            }

            vertexT *vertex, **vertexp;
            setT *vertices;

            // retreive the number of vertices in the facet
            int vcount = qh_setsize(facet->vertices);
            face.PrepareVertices(vcount);

            // retreive the number of edges in the facet
            // must be the same as vertices count
            int ecount = vcount; // qh_setsize(facet->neighbors);
            face.PrepareEdges(ecount);

            // Generate a temp set of vertices from a facet
            vertices = qh_facet3vertex(facet);
            int j = 0;
            FOREACHvertex_(vertices)
            {
                int pointIndex = vertex->id;
                face.vertices[j++] = id2vertices[pointIndex];
            }

            ///
            // Edges/Ridges Iteration

            for (j = 0; j < face.ecount; ++j)
            {
                // we mark all the edges as invalid
                face.edges[j] = 0xffffffff;
            }

            /*
            i = 0;

            int edgeCount = qh_setsize(facet->ridges);
            if (edgeCount < 3) {
                int a = 0;
            }

            ridgeT *ridge, **ridgep;
            FOREACHridge_(facet->ridges) {
                if (id2edges[ridge->id] == -1) { // new edge

                    // we store our ID
                    id2edges[ridge->id] = m_HullEdges.Size();

                    Edge e;
                    int k=0;
                    FOREACHvertex_(ridge->vertices) {
                        XASSERT(k < 2);
                        int pointIndex = vertex->id;
                        e.vertices[k++] = id2vertices[pointIndex];
                    }

                    // initialize the ref counter
                    e.references	= 0;
                    e.normal		= VxVector::axis0();

                    // add the edge
                    m_HullEdges.PushBack(e);
                }

                // we add the normal of the participating face
                m_HullEdges[id2edges[ridge->id]].normal += face.normal;

                // we store the edge into the face
                face.edges[i++] = id2edges[ridge->id];
            }
            */
        }

        // we now have to create the edges between the facess
        XArray<Edge> tempEdges;

        FORALLfacets
        {

            // get the corresponding face
            Face &face = m_HullFaces[id2faces[facet->id]];

            tempEdges.Resize(0);

            int i = 0;
            facetT *neighbor, **neighborp;
            FOREACHneighbor_(facet)
            {
                // we mark all the edges as invalid
                if (face.edges[i++] == 0xffffffff)
                {
                    face.edges[i - 1] = m_HullEdges.Size();
                    XASSERT(neighbor->id < maxID);
                    Face &nface = m_HullFaces[id2faces[neighbor->id]];

                    Edge e;
                    e.normal = face.normal;

                    // Find the common indices with neighbor
                    FindCommonPoints(face.vertices, face.vcount, nface.vertices, nface.vcount, e.vertices[0], e.vertices[1]);

                    e.references = 0;
                    // add the edge
                    m_HullEdges.PushBack(e);

                    // update the neighbor
                    int j = 0;
                    facetT *nneighbor, **nneighborp;
                    FOREACHsetelement_(facetT, neighbor->neighbors, nneighbor)
                    {
                        if (nneighbor == facet)
                            break;
                        ++j;
                    }

                    nface.edges[j] = face.edges[i - 1];
                }

                // mark this edge as alreay added for the traingle
                if (face.edges[i - 1] > (CKDWORD)m_HullEdges.Size())
                {
                    int a = 0;
                }
                Edge &edge = m_HullEdges[face.edges[i - 1]];
                tempEdges.PushBack(edge);
            }

            for (; i < face.ecount; ++i)
            {

                CKDWORD i0 = 0;
                CKDWORD i1 = 1;
                int j;
                for (j = 0; j < face.vcount - 1; ++j)
                {
                    Edge e;
                    e.vertices[0] = face.vertices[j];
                    e.vertices[1] = face.vertices[j + 1];

                    if (!tempEdges.IsHere(e))
                    {
                        // this is a valid boundary edge
                        tempEdges.PushBack(e);

                        e.normal = face.normal;
                        face.edges[i] = m_HullEdges.Size();
                        m_HullEdges.PushBack(e);
                        break;
                    }
                }

                if (j == face.vcount - 1)
                { // Not found yet : the last possibility
                    Edge e;
                    e.vertices[0] = face.vertices[0];
                    e.vertices[1] = face.vertices[j];

                    if (!tempEdges.IsHere(e))
                    {
                        // this is a valid boundary edge
                        tempEdges.PushBack(e);

                        e.normal = face.normal;
                        face.edges[i] = m_HullEdges.Size();
                        m_HullEdges.PushBack(e);
                        break;
                    }
                }
            }
        }
    }
    qh_freeqhull(!qh_ALL);

    int curlong, totlong;
    qh_memfreeshort(&curlong, &totlong);

    delete[] points;
}

// render the convex hull
void PortalsManager::Occluder::RenderHull(CKRenderContext *iRC, CK3dEntity *iEntity) const
{
    iRC->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);

    // we draw the silhouette
    // States
    iRC->SetTexture(NULL); // No texture

    // Transfo
    iRC->SetWorldTransformationMatrix(iEntity->GetWorldMatrix());

    // draw the faces contour
    Face *it;
    for (it = m_HullFaces.Begin(); it != m_HullFaces.End(); ++it)
    {

        // Data
        VxDrawPrimitiveData *data = iRC->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, it->vcount + 1);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        XPtrStrided<VxVector> positions(data->PositionPtr, data->PositionStride);
#else
        XPtrStrided<VxVector> positions(data->Positions.Ptr, data->Positions.Stride);
#endif

        // positions
        int i;
        for (i = 0; i < it->vcount; ++i)
        {
            positions[i] = m_HullVertices[it->vertices[i]];
        }
        positions[i] = m_HullVertices[it->vertices[0]];

        // Colors
        CKDWORD col = RGBAFTOCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        VxFillStructure(it->vcount + 1, data->ColorPtr, data->ColorStride, 4, &col);
#else
        VxFillStructure(it->vcount + 1, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif

        iRC->DrawPrimitive(VX_LINESTRIP, NULL, it->vcount + 1, data);
    }

    // draw the faces normal
    int facesCount = m_HullFaces.Size();
    VxDrawPrimitiveData *data = iRC->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, facesCount * 2);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    XPtrStrided<VxVector> positions(data->PositionPtr, data->PositionStride);
#else
    XPtrStrided<VxVector> positions(data->Positions.Ptr, data->Positions.Stride);
#endif

    int index = 0;
    for (it = m_HullFaces.Begin(); it != m_HullFaces.End(); ++it)
    {

        VxVector center = VxVector::axis0();
        for (int i = 0; i < it->vcount; ++i)
        {
            center += m_HullVertices[it->vertices[i]];
        }
        center /= (float)it->vcount;

        positions[index++] = center;
        positions[index++] = center + 0.1f * it->normal;
    }
    // Colors
    CKDWORD col = RGBAFTOCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxFillStructure(facesCount * 2, data->ColorPtr, data->ColorStride, 4, &col);
#else
    VxFillStructure(facesCount * 2, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif

    iRC->DrawPrimitive(VX_LINELIST, NULL, facesCount * 2, data);
    iRC->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);
}

// render the silhouette
void PortalsManager::Occluder::RenderSilhouette(CKRenderContext *iRC, const XArray<VxVector> &iSilhouetteEdges) const
{
    // States
    iRC->SetTexture(NULL); // No texture

    // Transfo
    iRC->SetWorldTransformationMatrix(VxMatrix::Identity());

    int count = iSilhouetteEdges.Size();

    // Data
    VxDrawPrimitiveData *data = iRC->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, count);

    // positions
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxCopyStructure(count, data->PositionPtr, data->PositionStride, sizeof(VxVector), iSilhouetteEdges.Begin(), sizeof(VxVector));
#else
    VxCopyStructure(count, data->Positions.Ptr, data->Positions.Stride, sizeof(VxVector), iSilhouetteEdges.Begin(), sizeof(VxVector));
#endif

    // Colors
    CKDWORD col = RGBAFTOCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxFillStructure(count, data->ColorPtr, data->ColorStride, 4, &col);
#else
    VxFillStructure(count, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif

    iRC->DrawPrimitive(VX_LINELIST, NULL, count, data);
}

void PortalsManager::Occluder::ComputeSilhouette(CK3dEntity *iEntity, const VxFrustum &iFrustum, XArray<VxVector> &oSilhouettesVertices, XArray<VxPlane> &oVolume)
{
    //	CKContext* ctx = iEntity->GetCKContext();

    // we clear the existing vertices of the silhouette
    oSilhouettesVertices.Resize(0);
    m_SilhouetteEdges.Resize(0);

    if (!m_HullEdges.Size())
        return;

    VxVector frontNormal = VxVector::axis0();

    if (m_Plane)
    { // special case of a planar occluder

        // must store the normal of the plane direction
        frontNormal = m_HullFaces[0].normal;

        // the planar hull is considered as both sided
        VxVector localCameraPos;
        iEntity->InverseTransform(&localCameraPos, &iFrustum.GetOrigin());

        VxVector localDir = m_HullVertices[0] - localCameraPos;

        if (localDir.Dot(frontNormal) > 0.0f) // same orientation, we thus invert it
            frontNormal = -frontNormal;
    }
    else
    { // Normal convex hull

        VxVector localPos;
        iEntity->InverseTransform(&localPos, &iFrustum.GetOrigin());

        // We iterate on all the faces, adding the edges of the face facing
        // if count == 1, the edge has only been visited once, hence
        // is a contour one (=0 backfacing only, =2 not from contour)
        for (Face *it = m_HullFaces.Begin(); it != m_HullFaces.End(); ++it)
        {

            VxVector center = VxVector::axis0();
            for (int i = 0; i < it->vcount; ++i)
            {
                center += m_HullVertices[it->vertices[i]];
            }
            center /= (float)it->vcount;

            VxVector localDir = center - localPos;
            if (localDir.Dot(it->normal) <= EPSILON)
            { // face is facing the viewer

                // add the normal to the cumulative front normal (last plane to add to the volume
                frontNormal += it->normal;

                // we now process the edges of the polygon
                for (int i = 0; i < it->ecount; ++i)
                {
                    m_HullEdges[it->edges[i]].references++;
                }
            }
        }
    }

    const VxMatrix &worldMatrix = iEntity->GetWorldMatrix();

    // we now compute the planes crossing the edges from the silhouette

    {
        VxVector w0, w1, wn;
        VxPlane plane;
        for (XArray<Edge>::Iterator ei = m_HullEdges.Begin(); ei != m_HullEdges.End(); ++ei)
        {
            Edge &e = *ei;

            if (e.references == 1)
            {

                // we transform the edge into world
                Vx3DMultiplyMatrixVector(&w0, worldMatrix, &(m_HullVertices[e.vertices[0]]));
                Vx3DMultiplyMatrixVector(&w1, worldMatrix, &(m_HullVertices[e.vertices[1]]));

                oSilhouettesVertices.PushBack(w0);
                oSilhouettesVertices.PushBack(w1);

                // we create the plane
                plane.Create(iFrustum.GetOrigin(), w0, w1);

                // we transform the normal edge into the world
                Vx3DRotateVector(&wn, worldMatrix, &e.normal);

                // we orient the plane correctly
                // (in the same direction of the edge normal)
                if (plane.m_Normal.Dot(wn) < 0.0f)
                {
                    plane = -plane;
                }

                // we add the plane passing thru this edge
                oVolume.PushBack(plane);
            }

            if (!m_Plane) // we must not reset the reference count for a plane hull
                e.references = 0;
        }
    }

    // we now calculate the farthest point of the silhouette
    float maxd = -100000.0f;
    VxVector farp;

    for (int i = 0; i < oSilhouettesVertices.Size(); ++i)
    {

        // we search the farthest point of the camera
        float dp = iFrustum.GetDir().Dot(oSilhouettesVertices[i]);
        if (dp > maxd)
        {
            maxd = dp;
            farp = oSilhouettesVertices[i];
        }
    }

    // we then add a plane passing thru the farthest point
    // and oriented as the sum of all front facing faces
    VxVector wn;
    Vx3DRotateVector(&wn, worldMatrix, &frontNormal);
    oVolume.PushBack(VxPlane(Normalize(wn), farp));
}
