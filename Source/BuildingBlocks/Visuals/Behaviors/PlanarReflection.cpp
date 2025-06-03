/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PlanarReflection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPlanarReflectionDecl();
CKERROR CreatePlanarReflectionProto(CKBehaviorPrototype **pproto);
int PlanarReflection(const CKBehaviorContext &behcontext);
int PlanarReflectionPreRenderCallBack(CKRenderContext *rc, CKRenderObject *ent, void *arg);

#define CKPGUID_REFLECTEDGROUP CKGUID(0xaa22c2e, 0x265e3801)

CKObjectDeclaration *FillBehaviorPlanarReflectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Planar Reflection");
    od->SetDescription("Creates a reflection on a plane of objects in a group.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Reflection Group(s): </SPAN>Groups of objects effectively reflected.<BR>
    <SPAN CLASS=pin>Plane Normal: </SPAN>Normal of the plane to reflect from.<BR>
    <BR>
    The objects must have the "Reflected Object" attributes to be taken into account
    by this behavior. The group will determine on which plane the objects will reflect
    and the mesh will be used for reflection, if provided. Otherwise, it will reflect
    the entire hierarchy with their current meshes.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x44fe1b3c, 0x29ba3445));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlanarReflectionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreatePlanarReflectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Planar Reflection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Reflection Group(s)", CKPGUID_FILTER, "1");
    proto->DeclareInParameter("Plane Normal", CKPGUID_VECTOR, "0,1,0");

    proto->DeclareSetting("Discard Off Mirror Reflection", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(PlanarReflection);

    *pproto = proto;
    return CK_OK;
}

int PlanarReflection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // We enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (ent)
        ent->AddPreRenderCallBack(PlanarReflectionPreRenderCallBack, (void *)beh->GetID(), TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/****************** RENDER CALLBACK ********************/

#define NONE 0
#define STENCIL 1
#define CLIPPLANES 2

int PlanarReflectionPreRenderCallBack(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{

    CKContext *ctx = rc->GetCKContext();
    CK3dEntity *plane = (CK3dEntity *)obj;
    CKBehavior *beh = (CKBehavior *)ctx->GetObject((CK_ID)arg);
    if (!beh)
        return TRUE;

    CKBOOL stencilWasEnabled = rc->GetState(VXRENDERSTATE_STENCILENABLE);
    CKBOOL clipPlanesWasEnabled = rc->GetState(VXRENDERSTATE_CLIPPLANEENABLE);

    const VxMatrix &invplane = plane->GetInverseWorldMatrix();

    int planeflag = 1;
    beh->GetInputParameterValue(0, &planeflag);

    VxVector localplanenormal(0, 1, 0);
    beh->GetInputParameterValue(1, &localplanenormal);

    ///
    // Check if we are beneath the plane
    VxVector viewposition;
    CK3dEntity *entity = rc->GetViewpoint();
    entity->GetPosition(&viewposition, plane);

    if (DotProduct(viewposition, localplanenormal) < 0)
        return 1;

    ///
    // Compute the reflection matrix

    VxVector planepos, zero(0.0f), planenormal;
    plane->TransformVector(&planenormal, &localplanenormal);
    planenormal.Normalize();
    plane->Transform(&planepos, &zero);

    VxMatrix reflect;
    float dp = DotProduct(planenormal, planepos);
    reflect[0][0] = 1.0f - 2.0f * planenormal.x * planenormal.x;
    reflect[0][1] = -2.0f * planenormal.x * planenormal.y;
    reflect[0][2] = -2.0f * planenormal.x * planenormal.z;
    reflect[0][3] = 0.0f;
    reflect[1][0] = -2.0f * planenormal.x * planenormal.y;
    reflect[1][1] = 1.0f - 2.0f * planenormal.y * planenormal.y;
    reflect[1][2] = -2.0f * planenormal.y * planenormal.z;
    reflect[1][3] = 0.0f;
    reflect[2][0] = -2.0f * planenormal.x * planenormal.z;
    reflect[2][1] = -2.0f * planenormal.y * planenormal.z;
    reflect[2][2] = 1.0f - 2.0f * planenormal.z * planenormal.z;
    reflect[2][3] = 0.0f;
    reflect[3][0] = 2.0f * dp * planenormal.x;
    reflect[3][1] = 2.0f * dp * planenormal.y;
    reflect[3][2] = 2.0f * dp * planenormal.z;
    reflect[3][3] = 1.0f;

    ///
    // We now determine the hardware present
    VxDriverDesc *driver = ctx->GetRenderManager()->GetRenderDriverDescription(rc->GetDriverIndex());
    int mode = NONE;

    CKBOOL maskOffMirror = TRUE;
    beh->GetLocalParameterValue(0, &maskOffMirror);

    if (maskOffMirror)
    {
        int stencilBpp = 0;
        rc->GetPixelFormat(NULL, NULL, &stencilBpp);
        if (stencilBpp >= 1)
            mode = STENCIL;
        else if (driver->Caps3D.MaxClipPlanes >= 4)
            mode = CLIPPLANES;

        ///
        // We draw the plane in the stencil
        if (mode == STENCIL)
        {

            int freeStencilBit = rc->GetFirstFreeStencilBits();
            if (freeStencilBit == -1)
            {
                rc->Clear(CK_RENDER_CLEARSTENCIL);
                freeStencilBit = 0;
            }

            rc->SetState(VXRENDERSTATE_STENCILENABLE, TRUE);

            rc->SetState(VXRENDERSTATE_STENCILREF, 0xff);
            rc->SetState(VXRENDERSTATE_STENCILWRITEMASK, 1 << freeStencilBit);
            rc->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_ALWAYS);

            rc->SetState(VXRENDERSTATE_STENCILFAIL, VXSTENCILOP_KEEP);
            rc->SetState(VXRENDERSTATE_STENCILZFAIL, VXSTENCILOP_KEEP);
            rc->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_REPLACE);

            plane->ModifyMoveableFlags(VX_MOVEABLE_STENCILONLY, 0);
            CKMesh *planeMesh = plane->GetCurrentMesh();
            if (planeMesh)
            {
                planeMesh->Render(rc, plane);
                rc->UsedStencilBits(1 << freeStencilBit);
            }
            plane->ModifyMoveableFlags(0, VX_MOVEABLE_STENCILONLY);

            // We now set rendering to only suceed if stencil >= 1
            rc->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_KEEP);
            rc->SetState(VXRENDERSTATE_STENCILREF, 0xff);
            rc->SetState(VXRENDERSTATE_STENCILMASK, 1 << freeStencilBit);
            rc->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_EQUAL); // GREATEREQUAL);
        }

        ///
        // we set the user clip planes
        if (mode == CLIPPLANES)
        {
            entity->GetPosition(&viewposition);

            const VxBbox &box = plane->GetBoundingBox(TRUE);
            float min = localplanenormal.x;
            int x = 1;
            int y = 2;

            if (min < localplanenormal.y)
            {
                min = localplanenormal.y;
                x = 0;
                y = 2;
            }
            if (min < localplanenormal.z)
            {
                x = 0;
                y = 1;
            }

            // We Calculate the extrema of the box in the world
            VxVector worldPosition[4];
            worldPosition[0] = box.Max;
            worldPosition[0].v[x] = box.Min.v[x];
            worldPosition[0].v[y] = box.Min.v[y];
            worldPosition[1] = box.Max;
            worldPosition[1].v[x] = box.Min.v[x];
            worldPosition[2] = box.Max;
            worldPosition[3] = box.Max;
            worldPosition[3].v[y] = box.Min.v[y];
            Vx3DMultiplyMatrixVectorMany(worldPosition, plane->GetWorldMatrix(), worldPosition, 4, sizeof(VxVector));

            // we define the 4 clipping planes
            VxPlane left(worldPosition[1], worldPosition[0], viewposition);
            VxPlane right(worldPosition[2], worldPosition[1], viewposition);
            VxPlane top(worldPosition[3], worldPosition[2], viewposition);
            VxPlane down(worldPosition[0], worldPosition[3], viewposition);

            // we set them for rendering
            rc->SetUserClipPlane(2, left);
            rc->SetUserClipPlane(0, right);
            rc->SetUserClipPlane(1, top);
            rc->SetUserClipPlane(3, down);
            rc->SetState(VXRENDERSTATE_CLIPPLANEENABLE, 0xf);
        }
    }

    // Get the attributes
    CKAttributeManager *attman = ctx->GetAttributeManager();
    int att = attman->GetAttributeTypeByName("Reflected Object");

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(att);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;
        if (!ent || (ent == plane))
            continue; // we skip recursivity

        CKParameterOut *pout = ent->GetAttributeParameter(att);
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();

        pout = (CKParameterOut *)ctx->GetObject(paramids[0]);
        int objectflag = 1;
        pout->GetValue(&objectflag);

        // We check if the plane should reflect this object
        if (!(planeflag & objectflag))
            continue;

        // We claculate the reflection matrix
        VxMatrix reflectmatrix;
        Vx3DMultiplyMatrix(reflectmatrix, reflect, ent->GetWorldMatrix());

        // Set the reflect matrix
        rc->SetWorldTransformationMatrix(reflectmatrix);

        // If we have a mesh to put, we do it now
        CK_ID meshid = 0;
        pout = (CKParameterOut *)ctx->GetObject(paramids[1]);
        pout->GetValue(&meshid);
        CKMesh *reflectmesh = (CKMesh *)ctx->GetObject(meshid);
        if (reflectmesh)
        {
            CKMesh *oldmesh = ent->GetCurrentMesh();

            ///
            // We render with the new mesh and we restore the old one
            ent->SetCurrentMesh(reflectmesh);
            CKDWORD oldflags = ent->GetMoveableFlags();
            if (oldflags & VX_MOVEABLE_INDIRECTMATRIX)
                ent->ModifyMoveableFlags(0, VX_MOVEABLE_INDIRECTMATRIX);
            else
                ent->ModifyMoveableFlags(VX_MOVEABLE_INDIRECTMATRIX, 0);
            ent->Render(rc, CKRENDER_DONTSETMATRIX);
            ent->RemoveMesh(reflectmesh);
            ent->SetMoveableFlags(oldflags);
            ent->SetCurrentMesh(oldmesh);
        }
        else
        {
            ///
            // Hierarchic rendering
            CKDWORD oldflags = ent->GetMoveableFlags();
            if (oldflags & VX_MOVEABLE_INDIRECTMATRIX)
                ent->ModifyMoveableFlags(0, VX_MOVEABLE_INDIRECTMATRIX);
            else
                ent->ModifyMoveableFlags(VX_MOVEABLE_INDIRECTMATRIX, 0);
            if (ent->IsVisible())
                ent->Render(rc, CKRENDER_DONTSETMATRIX);
            ent->SetMoveableFlags(oldflags);
            CK3dEntity *child = NULL;
            while (child = ent->HierarchyParser(child))
            {

                // Set the reflected matrix
                Vx3DMultiplyMatrix(reflectmatrix, reflect, child->GetWorldMatrix());
                rc->SetWorldTransformationMatrix(reflectmatrix);

                oldflags = child->GetMoveableFlags();
                if (oldflags & VX_MOVEABLE_INDIRECTMATRIX)
                    child->ModifyMoveableFlags(0, VX_MOVEABLE_INDIRECTMATRIX);
                else
                    child->ModifyMoveableFlags(VX_MOVEABLE_INDIRECTMATRIX, 0);
                if (child->IsVisible() && !child->IsHiddenByParent())
                    child->Render(rc, CKRENDER_DONTSETMATRIX);
                child->SetMoveableFlags(oldflags);
            }
        }
    }

    if (mode == STENCIL && !stencilWasEnabled)
        rc->SetState(VXRENDERSTATE_STENCILENABLE, FALSE);
    else if (mode == CLIPPLANES && !clipPlanesWasEnabled)
        rc->SetState(VXRENDERSTATE_CLIPPLANEENABLE, 0);

    rc->SetWorldTransformationMatrix(plane->GetWorldMatrix());

    return 1;
}
