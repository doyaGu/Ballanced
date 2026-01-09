/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT RealShadowMapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

#define A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT 16

CKObjectDeclaration *FillBehaviorRealShadowMappingDecl();
CKERROR CreateRealShadowMappingProto(CKBehaviorPrototype **pproto);
int RealShadowMapping(const CKBehaviorContext &behcontext);
CKERROR RealShadowMappingCallBack(const CKBehaviorContext &behcontext);
void RealShadowMappingRenderCallBack(CKRenderContext *dev, void *arg);

////////////////////////
//   Local Structure
////////////////////////
typedef struct
{
    CK_ID id;
    VxVector min;
    VxVector max;
} RealShadowFloor;

typedef struct
{
    CK_ID matID;                                     // material to be projected
    CK_ID texID;                                     // texture to be associated with the projected material
    RealShadowFloor floor[A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT]; // floors under the object ...
    int nb_floors_under;                             // number of floors being under the object...
} RealShadowStruct;

void A_GetFloors(RealShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh);
void A_Delete_SoftShadow_From_Floors(RealShadowFloor *floor, CKMaterial *cmat, int nb_floors);

CKObjectDeclaration *FillBehaviorRealShadowMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT RealShadowMapping");
    od->SetDescription("Calculate the real postion of shadow");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x452816af, 0x681a3a81));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRealShadowMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateRealShadowMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT RealShadowMapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light-Vector", CKPGUID_VECTOR);
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("SourceBlend", CKPGUID_BLENDFACTOR, "Zero");
    proto->DeclareInParameter("DestBlend", CKPGUID_BLENDFACTOR, "Source Color");

    proto->DeclareOutParameter("Eckpunkt1", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Eckpunkt2", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Eckpunkt3", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Eckpunkt4", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Mittelpunkt", CKPGUID_VECTOR);

    proto->DeclareLocalParameter("Data", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("Active", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RealShadowMapping);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(RealShadowMappingCallBack);

    *pproto = proto;
    return CK_OK;
}

int RealShadowMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKRenderContext *dev = behcontext.CurrentRenderContext;

    if (!beh->GetTarget())
        return CKBR_OWNERERROR;

    dev->AddPreRenderCallBack(RealShadowMappingRenderCallBack, beh, TRUE);

    CKBOOL active = FALSE;
    beh->GetLocalParameterValue(1, &active);
    if (!active)
        return CKBR_ACTIVATENEXTFRAME;

    dev->RemovePreRenderCallBack(RealShadowMappingRenderCallBack, beh);

    return CKBR_OK;
}

CKERROR RealShadowMappingCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    {
        // creation of a CkMaterial
        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "SimpleShadow Material",
                                                          beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        mat->SetEmissive(VxColor(255, 255, 255));
        mat->SetDiffuse(VxColor(255, 255, 255, 255));
        mat->SetSpecular(VxColor(0, 0, 0));
        mat->SetAmbient(VxColor(0, 0, 0));
        mat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
        mat->SetTextureBlendMode(VXTEXTUREBLEND_COPY);

        // save of the IDs
        RealShadowStruct tss;

        tss.matID = CKOBJID(mat);
        tss.texID = -1;
        tss.nb_floors_under = 0;
        memset(tss.floor, 0, A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT * sizeof(CK_ID));

        beh->SetLocalParameterValue(0, &tss, sizeof(RealShadowStruct));
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        RealShadowStruct *tss = (RealShadowStruct *)beh->GetLocalParameterWriteDataPtr(0);

        CKMaterial *mat = (CKMaterial *)ctx->GetObject(tss->matID);
        if (!mat)
            return CK_OK;

        A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);

        ctx->DestroyObject(mat); // we destroy the soft_shadow material
    }
    break;
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORNEWSCENE:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        RealShadowStruct *tss = (RealShadowStruct *)beh->GetLocalParameterWriteDataPtr(0);

        CKMaterial *mat = (CKMaterial *)ctx->GetObject(tss->matID);
        if (mat)
            A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);
    }
    break;
    }

    return CKBR_OK;
}

void RealShadowMappingRenderCallBack(CKRenderContext *dev, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;
    CKContext *context = dev->GetCKContext();

    CKBOOL active = FALSE;
    beh->GetLocalParameterValue(1, &active);
    CK3dEntity *ent = (CK3dEntity*)beh->GetTarget();
    if (!ent)
        active = TRUE;

    RealShadowStruct *tss = (RealShadowStruct *)beh->GetLocalParameterWriteDataPtr(0);

    //////////////////////////////////////////////////
    //  We get the Floors that WERE under the object
    //////////////////////////////////////////////////
    int old_nb_floors_under = tss->nb_floors_under;

#if CKVERSION == 0x13022002
    CKMemoryPool memoryPool(context, old_nb_floors_under);
    CK_ID *old_floor = (CK_ID *)memoryPool.Mem();
#else
    VxScratch memoryPool(old_nb_floors_under * sizeof(float));
    CK_ID* old_floor = (CK_ID*)memoryPool.Mem();
#endif

    memcpy(old_floor, tss->floor, old_nb_floors_under * sizeof(CK_ID));

    CKMaterial *mat = (CKMaterial *)context->GetObject(tss->matID);

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);
        active = TRUE;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
        active = FALSE;
    }

    // we get the input texture id
    CK_ID newtexID;
    beh->GetInputParameterValue(1, &newtexID);

    // if the texture change ( or at start )
    if (newtexID != tss->texID)
    {
        tss->texID = newtexID;
        CKTexture *tex = (CKTexture *)context->GetObject(tss->texID);
        mat->SetTexture0(tex);
    }

    VXBLEND_MODE srcBlend = VXBLEND_SRCALPHA;
    beh->GetInputParameterValue(3, &srcBlend);

    VXBLEND_MODE destBlend = VXBLEND_INVSRCALPHA;
    beh->GetInputParameterValue(4, &destBlend);

    A_GetFloors(tss, ent, beh);
    RealShadowFloor *floor = tss->floor;

    //////////////////////////////////////////////////
    //  We REMOVE the material from floors
    //  that are no longer under the object
    //////////////////////////////////////////////////
    CK3dEntity *o;
    CKMesh *dMesh;
    int matChannel;

    for (int a = 0; a < old_nb_floors_under; a++)
    {
        // list of OLD floors
        CKBOOL out = TRUE;
        for (int b = 0; b < tss->nb_floors_under; b++)
        {
            // list of NEW floors
            if (old_floor[a] == floor[b].id)
            {
                out = FALSE;
                break;
            }
        }
        if (out)
        {
            o = (CK3dEntity *)context->GetObject(old_floor[a]);
            if (CKIsChildClassOf(o, CKCID_3DENTITY))
            {
                dMesh = (CKMesh *)o->GetCurrentMesh();
                if (dMesh)
                {
                    matChannel = dMesh->GetChannelByMaterial(mat);
                    if (matChannel >= 0)
                        dMesh->RemoveChannel(matChannel);
                }
            }
        }
    }


    //////////////////////////////////////////////////
    //  We ADD the material to the floors
    //  that are under the object
    //////////////////////////////////////////////////

    Vx2DVector *uvArray;
    CKDWORD stride, cStride;
    VxVector *vertexArray;

    for (int a = 0; a < tss->nb_floors_under; a++)
    {
        o = (CK3dEntity *)context->GetObject(floor[a].id);
        VxVector floorPos;
        o->GetPosition(&floorPos);
        if (CKIsChildClassOf(o, CKCID_3DENTITY))
        {
            dMesh = (CKMesh *)o->GetCurrentMesh();
            if (dMesh)
            {
                matChannel = dMesh->GetChannelByMaterial(mat);

                // There were no shadow channel applied to the mesh
                if (matChannel < 0)
                    matChannel = dMesh->AddChannel(mat, FALSE);
                dMesh->SetChannelSourceBlend(matChannel, srcBlend);
                dMesh->SetChannelDestBlend(matChannel, destBlend);

                ////////////////////////////////////////////////////////
                //  We now change the U,V coords of the shadow channel
                ////////////////////////////////////////////////////////
                uvArray = (Vx2DVector *)dMesh->GetTextureCoordinatesPtr(&cStride, matChannel);
                vertexArray = (VxVector *)dMesh->GetPositionsPtr(&stride);

                int vertexCount = dMesh->GetVertexCount();

                for (int i = 0; i < vertexCount; i++,
                        vertexArray = (VxVector *) ((CKBYTE *) vertexArray + stride),
                        uvArray = (Vx2DVector *) ((CKBYTE *) uvArray + cStride))
                {
                    // Transform Vertex Position from mesh coordinates system to projector coordinate system
                    uvArray->x = (vertexArray->x - floor[a].max.x) / (floor[a].min.x - floor[a].max.x);
                    uvArray->y = (vertexArray->z - floor[a].max.z) / (floor[a].min.z - floor[a].max.z);
                }

                dMesh->UVChanged();
            }
        }
    }

    beh->SetLocalParameterValue(0, &tss);
}

void A_GetFloors(RealShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh)
{
    CKContext *context = beh->GetCKContext();

    VxVector light;
    beh->GetInputParameterValue(0, &light);

    Vx2DVector size;
    beh->GetInputParameterValue(2, &size);

    CKFloorManager *FloorManager = (CKFloorManager *)context->GetManagerByGuid(FLOOR_MANAGER_GUID);
    int floorAttribute = FloorManager->GetFloorAttribute();

    CKAttributeManager *attman = beh->m_Context->GetAttributeManager();
    const XObjectPointerArray &floor_objects = attman->GetGlobalAttributeListPtr(floorAttribute);

    VxVector objPos;
    ent->GetPosition(&objPos);
    light += objPos;

    CKBOOL under;
    int nbf_under = 0; // number of floors under
    float tmp_y;

    CK3dEntity *floor;
    int floor_count = floor_objects.Size();

    for (int n = 0; n < floor_count; ++n)
    {
        floor = (CK3dEntity *)floor_objects[n];

        if (floor->IsVisible() && !floor->IsAllOutsideFrustrum())
        {
            under = FALSE;

            VxVector vMax(1000.0f, 0, 1000.0f);
            VxVector vMin(-1000.0f, 0, -1000.0f);

            VxVector floorPos;
            floor->GetPosition(&floorPos);

            tmp_y = 0;

            const VxBbox &Bbox_floorWorld = floor->GetBoundingBox();

            VxIntersectionDesc desc;

            if (light.y <= 0.0f || Bbox_floorWorld.Min.y >= objPos.y)
                continue;

            VxVector vector[5];

            for (int i = 0; i < 5; ++i)
            {
                if (i == 0)
                {
                    VxVector pos = objPos;
                    floor->RayIntersection(&light, &pos, &desc, NULL);
                    if (desc.Object == floor)
                    {
                        floor->Transform(&vector[0], &desc.IntersectionPoint);
                        tmp_y = vector[0].y;
                    }
                    else
                    {
                        float y1 = light.y - floorPos.y;
                        float y2 = light.y - objPos.y;
                        vector[0].x = light.x + (objPos.x - floorPos.x) * y1 / y2;
                        vector[0].y = floorPos.y;
                        vector[0].z = light.z + (objPos.z - floorPos.z) * y1 / y2;
                        tmp_y = floorPos.y;
                    }

                    beh->SetOutputParameterValue(0, &vector[0]);

                    if (!under &&
                        vector[0].x > Bbox_floorWorld.Min.x &&
                        vector[0].x < Bbox_floorWorld.Max.x &&
                        vector[0].z > Bbox_floorWorld.Min.z &&
                        vector[0].z < Bbox_floorWorld.Max.z)
                    {
                        under = TRUE;
                    }
                }
                else
                {
                    float y1 = light.y - tmp_y;
                    float y2 = light.y - objPos.y;
                    vector[i].x = light.x + (objPos.x - floorPos.x) * y1 / y2;
                    vector[i].y = tmp_y;
                    vector[i].z = light.z + (objPos.z - floorPos.z) * y1 / y2;

                    beh->SetOutputParameterValue(i, &vector[i]);

                    if (!under &&
                        vector[i].x > Bbox_floorWorld.Min.x &&
                        vector[i].x < Bbox_floorWorld.Max.x &&
                        vector[i].z > Bbox_floorWorld.Min.z &&
                        vector[i].z < Bbox_floorWorld.Max.z)
                    {
                        under = TRUE;
                    }

                    if (vector[i].x < vMax.x)
                        vMax.x = vector[i].x;
                    if (vector[i].x > vMin.x)
                        vMin.x = vector[i].x;
                    if (vector[i].z < vMax.z)
                        vMax.z = vector[i].z;
                    if (vector[i].z > vMin.z)
                        vMin.z = vector[i].z;
                }
            }

            if (under)
            {
                if (nbf_under < A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT)
                {
                    floor->InverseTransform(&vMax, &vMax);
                    floor->InverseTransform(&vMin, &vMin);
                    RealShadowFloor &f = tss->floor[nbf_under];
                    f.max = vMax;
                    f.min = vMin;
                    f.id = floor->GetID();
                    nbf_under++;
                }
            }
        }
    }

    tss->nb_floors_under = nbf_under;
}

void A_Delete_SoftShadow_From_Floors(RealShadowFloor *floor, CKMaterial *cmat, int nb_floors)
{
    CKContext *context = cmat->GetCKContext();

    CK3dEntity *o;
    CKMesh *dMesh;
    int matChannel;

    for (int a = 0; a < nb_floors; a++)
    {
        o = (CK3dEntity *)context->GetObject(floor->id);
        if (CKIsChildClassOf(o, CKCID_3DENTITY))
        {
            dMesh = (CKMesh *)o->GetCurrentMesh();
            if (dMesh)
            {
                matChannel = dMesh->GetChannelByMaterial(cmat);
                if (matChannel >= 0)
                    dMesh->RemoveChannel(matChannel);
            }
        }
        ++floor;
    }
}
