/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              SimpleShadow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT 50

CKObjectDeclaration *FillBehaviorSimpleShadowDecl();
CKERROR CreateSimpleShadowProto(CKBehaviorPrototype **pproto);
int SimpleShadow(const CKBehaviorContext &behcontext);
CKERROR SimpleShadowCallBackObject(const CKBehaviorContext &behcontext);

////////////////////////
//   Local Structure
////////////////////////
typedef struct
{
    CK_ID matID;                                     // material to be projected
    CK_ID texID;                                     // texture to be associated with the projected material
    float zoom;                                      // zoom of the soft_shadow
    CK_ID floor[A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT]; // floors under the object ...
    int nb_floors_under;                             // number of floors beeing under the object...
} SimpleShadowStruct;

void A_GetFloors(SimpleShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh, VxVector *pos_rel, VxVector *scale, float maxHeight);
void A_Delete_SoftShadow_From_Floors(CK_ID *floor, CKMaterial *cmat, int nb_floors);

CKObjectDeclaration *FillBehaviorSimpleShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Simple Shadow");
    od->SetDescription("Display a little soft shadow on floors beneath the object.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Texture: </SPAN>the shadow's texture of your object. This texture should be symmetrical and from black to white (e.g.: just the under given texture : 'soft_shadow.jpg')<BR>
    <SPAN CLASS=pin>Size Scale: </SPAN>the shadow's size according to the object basis size.<BR>
    <SPAN CLASS=pin>Maximum Height: </SPAN>the height above which the shadow stops being projected on the floor(s).
    It means that if the distance between the bottom plane of the object's world bounding box, and the top plane of the floor(s) bounding box is greater than "Maximum Height",
    the shadow won't be rendered anymore.<BR>
    If you design your content in such a way that 2 floors can stand one up on the other (ex: a bridge above a terrain) then,
    giving a proper value to the "Maximum Height" parameter could solve the shadow to appear twice
    (on the bridge but also on the beneath terrain).<BR>
    <BR>
    */
    /* warning:
    - The "Simple Shadow" BB needs declared FLOORS to apply the soft shadow texture.(use "Declare Floors" or set the Floor Attribute to the objects you want to cast the shadow)<BR>
    - The Y axis of your floors referential should always be oriented in the world UP direction.<BR>
    - The shadow will be applied on all the floors just under the object.<BR>
    - The shadow won't be applied on floors that stand above the object.
    In fact, for performance purpose, a floor will be considered as being above the object if the top plane of the
    object's world bounding box is below the bottom plane of the floor's world bounding box.<BR>
    - If you want two objects to use the same texture shadow, you'd better use two different materials.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x79b95ed0, 0x3e4a01f5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSimpleShadowProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Visuals/Shadows");
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateSimpleShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Simple Shadow");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Scale", CKPGUID_FLOAT, "2.0");
    proto->DeclareInParameter("Maximum Height", CKPGUID_FLOAT, "100.0");

    proto->DeclareLocalParameter("Data", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SimpleShadow);
    proto->SetBehaviorCallbackFct(SimpleShadowCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int SimpleShadow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    CKContext *ctx = behcontext.Context;

    int a, b;

    CK3dEntity *ent = (CK3dEntity *)beh->GetOwner();

    SimpleShadowStruct *tss = (SimpleShadowStruct *)beh->GetLocalParameterWriteDataPtr(0);
    CKMaterial *cmat = (CKMaterial *)ctx->GetObject(tss->matID);

    CKMesh *dMesh;
    int MatChannel;

    //////////////////////////////////////////////////
    //  We get the Floors that WERE under the object
    //////////////////////////////////////////////////
    int old_nb_floors_under = tss->nb_floors_under;

    CK_ID *old_floor = new CK_ID[old_nb_floors_under];

    memcpy(old_floor, tss->floor, old_nb_floors_under * sizeof(CK_ID));

    CK3dEntity *o;

    ///////////////////////////
    //  If We enter by 'OFF'
    ///////////////////////////
    if (beh->IsInputActive(1))
    {

        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput((beh->GetOutputCount() < 2) ? 0 : 1, TRUE);

        A_Delete_SoftShadow_From_Floors(old_floor, cmat, tss->nb_floors_under);

        delete[] old_floor;
        return CKBR_OK;
    }

    ///////////////////////////
    // We DON'T enter by 'OFF'
    ///////////////////////////
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }
    // we get the input texture id
    CK_ID newtexID;
    beh->GetInputParameterValue(0, &newtexID);

    // if the texture change ( or at start )
    if (newtexID != tss->texID)
    {
        tss->texID = newtexID;
        CKTexture *tex = (CKTexture *)ctx->GetObject(tss->texID);
        cmat->SetTexture0(tex);
    }

    // we get the ZOOM factor
    float zoom = 2.0f;
    beh->GetInputParameterValue(1, &zoom);
    tss->zoom = zoom;

    float maxHeight = 100.0f;
    beh->GetInputParameterValue(2, &maxHeight);

    // bbox local multiply by world scale
    const VxBbox &Bbox_obj = ent->GetBoundingBox(TRUE);
    float inv_zoom = 1.0f / (zoom * (Bbox_obj.Max.x - Bbox_obj.Min.x) * Magnitude(ent->GetWorldMatrix()[0]));

    //////////////////////////////////////////////////
    //  We get the Floors that ARE under the object
    //////////////////////////////////////////////////
    CK_ID *floor;

    VxVector *pos_rel = new VxVector[A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT * 2];
    VxVector *scale = pos_rel + A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT;

    A_GetFloors(tss, ent, beh, pos_rel, scale, maxHeight);
    floor = tss->floor;

    //////////////////////////////////////////////////
    //  We REMOVE the material from floors
    //  that are no longer under the object
    //////////////////////////////////////////////////
    for (a = 0; a < old_nb_floors_under; a++)
    { // list of OLD floors
        CKBOOL out = TRUE;
        for (b = 0; b < tss->nb_floors_under; b++)
        { // list of NEW floors
            if (old_floor[a] == floor[b])
            {
                out = FALSE;
                break;
            }
        }
        if (out)
        {
            o = (CK3dEntity *)ctx->GetObject(old_floor[a]);
            if (CKIsChildClassOf(o, CKCID_3DENTITY))
            {
                dMesh = (CKMesh *)o->GetCurrentMesh();
                if (dMesh)
                {
                    MatChannel = dMesh->GetChannelByMaterial(cmat);
                    if (MatChannel >= 0)
                        dMesh->RemoveChannel(MatChannel);
                }
            }
        }
    }

    //////////////////////////////////////////////////
    //  We ADD the material to the floors
    //  that are under the object
    //////////////////////////////////////////////////

    Vx2DVector *uvarray;
    CKDWORD Stride, cStride;
    VxVector *VertexArray;

    int i, VertexCount = 0;

    VxVector vPos;
    VxVector vz(0.0f, 0.0f, 1.0f);

    float tmp_x, tmp_z;

    for (a = 0; a < tss->nb_floors_under; a++)
    {

        o = (CK3dEntity *)ctx->GetObject(floor[a]);
        if (CKIsChildClassOf(o, CKCID_3DENTITY))
        {
            dMesh = (CKMesh *)o->GetCurrentMesh();
            if (dMesh)
            {
                MatChannel = dMesh->GetChannelByMaterial(cmat);

                // There were no shadow channel applied to the mesh
                if (MatChannel < 0)
                {
                    MatChannel = dMesh->AddChannel(cmat, FALSE);
                    dMesh->SetChannelSourceBlend(MatChannel, VXBLEND_ZERO);
                    dMesh->SetChannelDestBlend(MatChannel, VXBLEND_SRCCOLOR);
                }

                ////////////////////////////////////////////////////////
                //  We now change the U,V coords of the shadow channel
                ////////////////////////////////////////////////////////
                uvarray = (Vx2DVector *)dMesh->GetTextureCoordinatesPtr(&cStride, MatChannel);
                VertexArray = (VxVector *)dMesh->GetPositionsPtr(&Stride);

                VertexCount = dMesh->GetVertexCount();

                tmp_x = inv_zoom * scale[a].x;
                tmp_z = inv_zoom * scale[a].z;

                vPos.x = pos_rel[a].x * tmp_x + 0.5f;
                vPos.z = pos_rel[a].z * tmp_z + 0.5f;

                for (i = 0; i < VertexCount; i++, VertexArray = (VxVector *)((CKBYTE *)VertexArray + Stride),
                    uvarray = (Vx2DVector *)((CKBYTE *)uvarray + cStride))
                {
                    // Tranform Vertex Position from mesh coordinates system to projector coordinate system

                    uvarray->x = vPos.x - VertexArray->x * tmp_x;
                    uvarray->y = vPos.z - VertexArray->z * tmp_z;
                }

                dMesh->UVChanged();
            }
        }
    }

    delete[] old_floor;
    delete[] pos_rel;

    return CKBR_ACTIVATENEXTFRAME;
}

//////////////////////////////////////////////////
//            CALLBACK
//////////////////////////////////////////////////
CKERROR SimpleShadowCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        // creation of a CkMaterial
        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "SimpleShadow Material", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);

        mat->SetEmissive(VxColor(255, 255, 255));
        mat->SetDiffuse(VxColor(255, 255, 255, 255));
        mat->SetSpecular(VxColor(0, 0, 0));
        mat->SetAmbient(VxColor(0, 0, 0));
        mat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
        mat->SetTextureBlendMode(VXTEXTUREBLEND_COPY);

        // save of the IDs
        SimpleShadowStruct tss;

        tss.matID = CKOBJID(mat);
        tss.texID = -1;
        tss.zoom = 2.0f;
        tss.nb_floors_under = 0;
        memset(tss.floor, 0, A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT * sizeof(CK_ID));

        beh->SetLocalParameterValue(0, &tss, sizeof(SimpleShadowStruct));
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        SimpleShadowStruct *tss = (SimpleShadowStruct *)beh->GetLocalParameterWriteDataPtr(0);

        CKMaterial *mat;
        if (mat = (CKMaterial *)ctx->GetObject(tss->matID))
            ;
        else
            return 0;

        A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);

        ctx->DestroyObject(mat); // we destroy the soft_shadow material
    }
    break;

    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORNEWSCENE:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        SimpleShadowStruct *tss = (SimpleShadowStruct *)beh->GetLocalParameterWriteDataPtr(0);

        CKMaterial *mat;
        if (mat = (CKMaterial *)ctx->GetObject(tss->matID))
            ;
        else
            return 0;

        A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);
    }
    break;
    }
    return CKBR_OK;
}

/*****************************************************/
/*                  A_GetFloors                      */
/*****************************************************/
//  This function puts in the local structures, the floors under the object
void A_GetFloors(SimpleShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh, VxVector *pos_rel, VxVector *scale_array, float maxHeight)
{

    CKContext *ctx = beh->GetCKContext();

    const VxBbox &Bbox_obj = ent->GetBoundingBox();
    float inv_zoom = 1.0f / (tss->zoom * (Bbox_obj.Max.x - Bbox_obj.Min.x));

    float tmp_x, tmp_z;

    CKFloorManager *FloorManager = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);
    int floorAttribute = FloorManager->GetFloorAttribute();

    CKAttributeManager *attman = beh->m_Context->GetAttributeManager();
    const XObjectPointerArray &floor_objects = attman->GetGlobalAttributeListPtr(floorAttribute);

    VxVector vPos, scale;
    Vx2DVector uvmax, uvmin;

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
            if (Bbox_floorWorld.Min.y > Bbox_obj.Max.y ||          // floor is above the object
                Bbox_obj.Min.y - Bbox_floorWorld.Max.y > maxHeight // floor is too below
            )
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

    CKContext *ctx = cmat->GetCKContext();

    CK3dEntity *o;
    CKMesh *dMesh;
    int MatChannel;

    for (int a = 0; a < nb_floors; a++)
    {
        o = (CK3dEntity *)ctx->GetObject(floor[a]);
        if (CKIsChildClassOf(o, CKCID_3DENTITY))
        {
            dMesh = o->GetCurrentMesh();
            if (dMesh)
            {
                MatChannel = dMesh->GetChannelByMaterial(cmat);
                if (MatChannel >= 0)
                    dMesh->RemoveChannel(MatChannel);
            }
        }
    }
}
