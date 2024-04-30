/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT Simple Shadow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

#define A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT 50

CKObjectDeclaration *FillBehaviorSimpleShadowDecl();
CKERROR CreateSimpleShadowProto(CKBehaviorPrototype **pproto);
int SimpleShadow(const CKBehaviorContext &behcontext);
CKERROR SimpleShadowCallBack(const CKBehaviorContext &behcontext);
void SimpleShadowRenderCallback(CKRenderContext *dev, void *arg);

////////////////////////
//   Local Structure
////////////////////////
typedef struct
{
    CK_ID matID;                                     // material to be projected
    CK_ID texID;                                     // texture to be associated with the projected material
    float zoom;                                      // zoom of the soft_shadow
    CK_ID floor[A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT]; // floors under the object ...
    int nb_floors_under;                             // number of floors being under the object...
} SimpleShadowStruct;

void A_GetFloors(SimpleShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh, VxVector *pos_rel, VxVector *scale, float maxHeight);
void A_Delete_SoftShadow_From_Floors(CK_ID *floor, CKMaterial *cmat, int nb_floors);

CKObjectDeclaration *FillBehaviorSimpleShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Simple Shadow");
    od->SetDescription("Display a little soft shadow on floors beneath the object.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7f0517c2, 0x52cc76bb));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSimpleShadowProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateSimpleShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Simple Shadow");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Scale", CKPGUID_FLOAT, "2.0");
    proto->DeclareInParameter("Maximum Height", CKPGUID_FLOAT, "100.0");

    proto->DeclareLocalParameter("Data", CKPGUID_POINTER, "NULL");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SimpleShadow);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SimpleShadowCallBack);

    *pproto = proto;
    return CK_OK;
}

int SimpleShadow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKRenderContext *dev = behcontext.CurrentRenderContext;
    CKContext *context = behcontext.Context;

    if (!beh->GetTarget())
        return CKBR_OWNERERROR;

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);

        SimpleShadowStruct *tss = NULL;
        beh->GetLocalParameterValue(0, &tss);
        if (tss)
        {
            CKMaterial *shadow = (CKMaterial *)context->GetObjectByNameAndClass("TT_SimpleShadow Material", CKCID_MATERIAL);
            CKMaterial *mat = (CKMaterial *)context->GetObject(tss->matID);
            if (!mat)
                return CKBR_PARAMETERERROR;
            if (shadow == mat)
            {
                A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);
                mat->SetTexture0(NULL);
                context->DestroyObject(mat);
            }
        }

        tss = NULL;
        beh->SetLocalParameterValue(0, &tss);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        CKMaterial *shadow = (CKMaterial *)context->GetObjectByNameAndClass("TT_SimpleShadow Material", CKCID_MATERIAL);
        if (shadow)
        {
            shadow->SetTexture0(NULL);
            context->DestroyObject(shadow);
        }

        // creation of a CkMaterial
        CKMaterial *mat = (CKMaterial *)context->CreateObject(CKCID_MATERIAL, "TT_SimpleShadow Material",
                                                              beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        mat->SetEmissive(VxColor(255, 255, 255));
        mat->SetDiffuse(VxColor(255, 255, 255, 255));
        mat->SetSpecular(VxColor(0, 0, 0));
        mat->SetAmbient(VxColor(0, 0, 0));
        mat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
        mat->SetTextureBlendMode(VXTEXTUREBLEND_COPY);

        SimpleShadowStruct *tss = NULL;
        beh->GetLocalParameterValue(0, &tss);
        if (tss)
        {
            A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);
            tss = NULL;
        }

        tss = new SimpleShadowStruct;
        tss->matID = CKOBJID(mat);
        tss->texID = -1;
        tss->zoom = 2.0f;
        tss->nb_floors_under = 0;
        memset(tss->floor, 0, A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT * sizeof(CK_ID));

        beh->SetLocalParameterValue(0, &tss);
    }

    dev->AddPreRenderCallBack(SimpleShadowRenderCallback, beh, TRUE);
    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR SimpleShadowCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKRenderContext *dev = behcontext.CurrentRenderContext;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORPAUSE:
        dev->RemovePreRenderCallBack(SimpleShadowRenderCallback, beh);
        break;
    case CKM_BEHAVIORRESUME:
        dev->AddPreRenderCallBack(SimpleShadowRenderCallback, beh, TRUE);
        break;
    }

    return CKBR_OK;
}

void SimpleShadowRenderCallback(CKRenderContext *dev, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;
    CKContext *context = dev->GetCKContext();

    CK3dEntity *entity = (CK3dEntity *)beh->GetTarget();

    SimpleShadowStruct *tss = NULL;
    beh->GetLocalParameterValue(0, &tss);
    if (!tss)
        return;

    CKMaterial *mat = (CKMaterial *)context->GetObject(tss->matID);

    //////////////////////////////////////////////////
    //  We get the Floors that WERE under the object
    //////////////////////////////////////////////////
    int old_nb_floors_under = tss->nb_floors_under;

#if CKVERSION == 0x13022002
    CKMemoryPool memoryPool1(context, old_nb_floors_under);
    CK_ID *old_floor = (CK_ID *)memoryPool1.Mem();
#else
    VxScratch mempool1(old_nb_floors_under * sizeof(float));
    CK_ID* old_floor = (CK_ID*)mempool1.Mem();
#endif

    memcpy(old_floor, tss->floor, old_nb_floors_under * sizeof(CK_ID));

    // we get the input texture id
    CK_ID newtexID;
    beh->GetInputParameterValue(0, &newtexID);

    // if the texture change ( or at start )
    if (newtexID != tss->texID)
    {
        tss->texID = newtexID;
        CKTexture *tex = (CKTexture *)context->GetObject(tss->texID);
        mat->SetTexture0(tex);
    }

    // we get the ZOOM factor
    float zoom = 2.0f;
    beh->GetInputParameterValue(1, &zoom);
    tss->zoom = zoom;

    float maxHeight = 100.0f;
    beh->GetInputParameterValue(2, &maxHeight);

    // bbox local multiply by world scale
    const VxBbox &Bbox_obj = entity->GetBoundingBox(TRUE);
    float inv_zoom = 1.0f / (zoom * (Bbox_obj.Max.x - Bbox_obj.Min.x) * Magnitude(entity->GetWorldMatrix()[0]));

    //////////////////////////////////////////////////
    //  We get the Floors that ARE under the object
    //////////////////////////////////////////////////
#if CKVERSION == 0x13022002
    CKMemoryPool memoryPool2(context, A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT);
    CKMemoryPool memoryPool3(context, A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT);
    VxVector* pos_rel = (VxVector*)memoryPool2.Mem();
    VxVector* scale = (VxVector*)memoryPool3.Mem();
#else
    VxScratch mempool2(A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT * sizeof(VxVector) * 2);
    VxVector* pos_rel = (VxVector*)mempool2.Mem();
    VxVector* scale = pos_rel + A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT;
#endif

    A_GetFloors(tss, entity, beh, pos_rel, scale, maxHeight);
    CK_ID *floor = tss->floor;

    //////////////////////////////////////////////////
    //  We REMOVE the material from floors
    //  that are no longer under the object
    //////////////////////////////////////////////////
    int a, b;
    CK3dEntity *o;
    CKMesh *dMesh;
    int matChannel;

    for (a = 0; a < old_nb_floors_under; a++)
    {
        // list of OLD floors
        CKBOOL out = TRUE;
        for (b = 0; b < tss->nb_floors_under; b++)
        {
            // list of NEW floors
            if (old_floor[a] == floor[b])
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

    int i, vertexCount;

    VxVector vPos;
    VxVector vz(0.0f, 0.0f, 1.0f);

    float tmp_x, tmp_z;

    for (a = 0; a < tss->nb_floors_under; a++)
    {
        o = (CK3dEntity *)context->GetObject(floor[a]);
        if (CKIsChildClassOf(o, CKCID_3DENTITY))
        {
            dMesh = (CKMesh *)o->GetCurrentMesh();
            if (dMesh)
            {
                matChannel = dMesh->GetChannelByMaterial(mat);

                // There were no shadow channel applied to the mesh
                if (matChannel < 0)
                {
                    matChannel = dMesh->AddChannel(mat, FALSE);
                    dMesh->SetChannelSourceBlend(matChannel, VXBLEND_ZERO);
                    dMesh->SetChannelDestBlend(matChannel, VXBLEND_SRCCOLOR);
                }

                ////////////////////////////////////////////////////////
                //  We now change the U,V coords of the shadow channel
                ////////////////////////////////////////////////////////
                uvArray = (Vx2DVector *)dMesh->GetTextureCoordinatesPtr(&cStride, matChannel);
                vertexArray = (VxVector *)dMesh->GetPositionsPtr(&stride);

                vertexCount = dMesh->GetVertexCount();

                tmp_x = inv_zoom * scale[a].x;
                tmp_z = inv_zoom * scale[a].z;

                vPos.x = pos_rel[a].x * tmp_x + 0.5f;
                vPos.z = pos_rel[a].z * tmp_z + 0.5f;

                for (i = 0; i < vertexCount; i++,
                        vertexArray = (VxVector *) ((CKBYTE *) vertexArray + stride),
                        uvArray = (Vx2DVector *) ((CKBYTE *) uvArray + cStride))
                {
                    // Transform Vertex Position from mesh coordinates system to projector coordinate system
                    uvArray->x = vPos.x - vertexArray->x * tmp_x;
                    uvArray->y = vPos.z - vertexArray->z * tmp_z;
                }

                dMesh->UVChanged();
            }
        }
    }

    beh->SetLocalParameterValue(0, &tss);
}

/*****************************************************/
/*                  A_GetFloors                      */
/*****************************************************/
//  This function puts in the local structures, the floors under the object
void A_GetFloors(SimpleShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh, VxVector *pos_rel, VxVector *scale_array, float maxHeight)
{
    CKContext *context = beh->GetCKContext();

    const VxBbox &Bbox_obj = ent->GetBoundingBox();
    float inv_zoom = 1.0f / (tss->zoom * (Bbox_obj.Max.x - Bbox_obj.Min.x));

    CKFloorManager *FloorManager = (CKFloorManager *)context->GetManagerByGuid(FLOOR_MANAGER_GUID);
    int floorAttribute = FloorManager->GetFloorAttribute();

    CKAttributeManager *attman = beh->m_Context->GetAttributeManager();
    const XObjectPointerArray &floor_objects = attman->GetGlobalAttributeListPtr(floorAttribute);

    float tmp_x, tmp_z;
    VxVector vPos, scale;

    CKBOOL under;
    int nbf_under = 0; // number of floors under

    CK3dEntity *floor;
    int floor_count = floor_objects.Size();

    for (int n = 0; n < floor_count; ++n)
    {
        floor = (CK3dEntity *)floor_objects[n];

        if (floor->IsVisible() && !floor->IsAllOutsideFrustrum())
        {
            // ... and just the VISIBLE floors
            // rem: IsAllOutsideFrustrum() function will result in a 1 frame delayed appearance
            // if the floor was totally outside the viewing frustum during the previous rendering,
            // and becomes very visible now... (if this is the case, the IsAllOutsideFrustrum()
            // function can be removed ).

            under = FALSE;

            const VxBbox &Bbox_floorWorld = floor->GetBoundingBox();

            // test the two world bbox on XZ plane
            if (Bbox_floorWorld.Min.x > Bbox_obj.Max.x ||
                Bbox_floorWorld.Min.z > Bbox_obj.Max.z ||
                Bbox_floorWorld.Max.x < Bbox_obj.Min.x ||
                Bbox_floorWorld.Max.z < Bbox_obj.Min.z)
                continue;

            // test the two bbox on height
            if (Bbox_floorWorld.Min.y > Bbox_obj.Max.y ||           // floor is above the object
                Bbox_obj.Min.y - Bbox_floorWorld.Max.y > maxHeight) // floor is too below
                continue;

            ent->GetPosition(&vPos, floor);
            const VxBbox &Bbox_floor = floor->GetBoundingBox(TRUE);

            floor->GetScale(&scale);

            tmp_x = inv_zoom * scale.x;
            tmp_z = inv_zoom * scale.z;

            if ((Bbox_floor.Max.x - vPos.x) * tmp_x >= -0.5f)
                if ((Bbox_floor.Min.x - vPos.x) * tmp_x <= 0.5f)
                    if ((Bbox_floor.Max.z - vPos.z) * tmp_z >= -0.5f)
                        if ((Bbox_floor.Min.z - vPos.z) * tmp_z <= 0.5f)
                        {
                            // floors is UNDER if the UV of its Bbox
                            // are possibly in the texture (0-1)
                            under = TRUE;
                        }

            if (under)
            {
                // ... and just floors that should be UNDER the object
                if (nbf_under < A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT)
                {
                    scale_array[nbf_under] = scale;
                    pos_rel[nbf_under] = vPos;
                    tss->floor[nbf_under] = floor->GetID();
                    nbf_under++;
                }
            }
        }
    }

    tss->nb_floors_under = nbf_under;
}

/*****************************************************/
/*        A_Delete_SoftShadow_From_Floors            */
/*****************************************************/
//  This function remove the material channel 'cmat' from all floors stocked in 'floor[]'
void A_Delete_SoftShadow_From_Floors(CK_ID *floor, CKMaterial *cmat, int nb_floors)
{
    CKContext *context = cmat->GetCKContext();

    CK3dEntity *o;
    CKMesh *dMesh;
    int matChannel;

    for (int a = 0; a < nb_floors; a++)
    {
        o = (CK3dEntity *)context->GetObject(floor[a]);
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
    }
}