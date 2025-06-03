/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Volumetric Fog
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorVolumetricFogDecl();
CKERROR CreateVolumetricFogProto(CKBehaviorPrototype **);
int VolumetricFog(const CKBehaviorContext &behcontext);
int VolumetricFogPreRender(CKRenderContext *rc, CKRenderObject *rent, void *arg);

/***********************************************/
//	Declaration Function
/***********************************************/
CKObjectDeclaration *FillBehaviorVolumetricFogDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Volumetric Fog");
    od->SetDescription("Volumetric Fog.");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xb0f1ca98, 0xe9dabe5f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateVolumetricFogProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/FX");
    return od;
}

/***********************************************/
//	Proto Function
/***********************************************/
CKERROR CreateVolumetricFogProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Volumetric Fog");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("FogDensity", CKPGUID_PERCENTAGE, "1");
    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(VolumetricFog);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

/***********************************************/
//	Main Function
/***********************************************/
int VolumetricFog(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    {
        // enter by OFF
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        // enter by ON
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    // get Group of objects on which fog applies to
    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(2);
    if (!group)
        return CKBR_OK;

    int objCount = group->GetObjectCount();
    for (int a = 0; a < objCount; ++a)
    {

        CK3dEntity *obj = (CK3dObject *)group->GetObject(a);
        if (!obj)
            continue;

        obj->AddPreRenderCallBack(VolumetricFogPreRender, beh, TRUE);
    }
    return CKBR_ACTIVATENEXTFRAME;
}

/***********************************************/
//	PreRender Callback
/***********************************************/
int VolumetricFogPreRender(CKRenderContext *rc, CKRenderObject *rent, void *arg)
{
    CK3dEntity *obj = (CK3dEntity *)rent;
    if (!obj)
        return 0;

    CKMesh *mesh = obj->GetCurrentMesh();
    if (!mesh)
        return 0;

    if (!obj->IsVisible() || obj->IsAllOutsideFrustrum())
        return 0;
    // rem: IsAllOutsideFrustrum() function will result in a 1 frame delayed appearence
    // if the floor was totally outside the viewing furstum during the previous rendering,
    // and becomes very visible now... (if this is the case, the IsAllOutsideFrustrum()
    // function can be removed ).

    CKDWORD uvStride, posStride;
    CKBehavior *beh = (CKBehavior *)arg;

    // get Material
    CKMaterial *fogMat = (CKMaterial *)beh->GetInputParameterObject(0);

    // fog Referential
    CK3dEntity *fogRef = (CK3dEntity *)beh->GetTarget();
    if (!fogRef)
        return CKBR_OWNERERROR;

    // get fog density
    float density = 0.01f;
    beh->GetInputParameterValue(1, &density);

    // get current point of view
    CK3dEntity *cam = rc->GetViewpoint();

    // if there's no fog material channel, add one
    int channel = mesh->GetChannelByMaterial(fogMat);
    if (channel < 0)
    {
        channel = mesh->AddChannel(fogMat, FALSE);
        mesh->SetChannelSourceBlend(channel, VXBLEND_ONE);
        mesh->SetChannelDestBlend(channel, VXBLEND_INVSRCCOLOR);
    }

    Vx2DVector *uv = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride, channel);
    if (!uv)
        return 0;

    VxVector *pos = (VxVector *)mesh->GetPositionsPtr(&posStride);
    if (!pos)
        return 0;

    int count = mesh->GetVertexCount();

    //--- pre-calcul part
    VxVector fogRefUp; // vector up of fogRef expressed in obj referential
    obj->InverseTransformVector(&fogRefUp, &VxVector::axisY(), fogRef);

    VxVector camPos; // camera position in obj referential (for further dist. calc.)
    cam->GetPosition(&camPos, obj);

    VxVector fogRefPos; // fogRef position in obj referential (for camHeight calc.)
    fogRef->GetPosition(&fogRefPos, obj);

    // camera height above fog plane
    float camHeight = DotProduct(camPos - fogRefPos, fogRefUp);

    // we now multiply camHeight by density to save a multiplication in the critical loop
    float camHeightbyDensity = camHeight * density;

    // to have the fog intensity only depend on the CamZ distance
    VxVector camZneg;
    obj->InverseTransformVector(&camZneg, &VxVector::axisZ(), cam);
    camZneg = -camZneg;
    VxVector scale(1, 1, 1);
    obj->GetScale(&scale, TRUE);

    // mult. by scale to get the normalize canZneg in the obj referential
    // and mult. again by scale to have the fog take the world distances into concideration
    camZneg *= scale * scale;

    //--- calcul part
    VxVector distVect;
    float DepthofVertex;

    // switch calc. if camera is beneath the fog plane
    if (camHeight < 0.0f)
    {
        // beneath
        for (int a = 0; a < count; ++a)
        {
            distVect = camPos - *pos;
            DepthofVertex = DotProduct(distVect, fogRefUp);

            if (DepthofVertex + 0.001f > camHeight)
            {
                // we added 0.001f to make sure vertex in the XZ forRef plane
                // will be consider as being inside
                uv->y = DotProduct(distVect, camZneg) * density;
            }
            else
            {
                uv->y = 0.0f;
            }
            uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
            pos = (VxVector *)((CKBYTE *)pos + posStride);
        }
    }
    else
    {
        // above
        for (int a = 0; a < count; ++a)
        {
            distVect = camPos - *pos;
            DepthofVertex = DotProduct(distVect, fogRefUp);

            if (DepthofVertex + 0.001f > camHeight)
            {
                // we added 0.001f to make sure vertex in the XZ forRef plane
                // will be consider as being inside
                uv->y = DotProduct(distVect, camZneg) * (density - camHeightbyDensity / DepthofVertex);
            }
            else
            {
                uv->y = 0.0f;
            }
            uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
            pos = (VxVector *)((CKBYTE *)pos + posStride);
        }
    }

    mesh->UVChanged();

    return 0; // no need to update extents
}
