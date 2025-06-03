/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SolidTrail
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSolidTrailDecl();
CKERROR CreateSolidTrailProto(CKBehaviorPrototype **pproto);
int SolidTrail(const CKBehaviorContext &behcontext);
CKERROR SolidTrailCallBackObject(const CKBehaviorContext &behcontext);

// specific types
#define TRACKS_NUMBER 40

CKObjectDeclaration *FillBehaviorSolidTrailDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Solid Trail");
    od->SetDescription("Create a Solid Trail following the 3D Entity when it's moving.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Width Attenuation: </SPAN>percentage determining how fast the trail shrinks (10 or 20 are nice value).<BR>
    <SPAN CLASS=pin>Length: </SPAN>length of the trail (10% is a short trail, 100% is the longest trail supported).<BR>
    <SPAN CLASS=pin>Color: </SPAN>color of the trail expressed in RGBA.<BR>
    <SPAN CLASS=pin>Frame Delay: </SPAN>frame delay between trail polygons generation.<BR>
    <BR>
    Useful for swords or pools moving fast. Use "flashy" colors for better results.<BR>
    The trail is composed of dynamically created polygons. The frame delay parameter determines how often these polygons are created. Therefore a 1 frame delay will result in a short smooth trail, whereas a 5 frame delays creates a long rough trail.<BR>
    Concerning 'Attenuation', negative value makes the trail expand instead of shrinking (typical values are -10 or -20). High value makes the trail cross over and over (typically 100 or 110).
    This behavior doesn't need looping.
    */
    od->SetCategory("Visuals/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xd00d010a, 0xe00d010a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSolidTrailProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateSolidTrailProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Solid Trail");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("ExitOn");
    proto->DeclareOutput("ExitOff");

    proto->DeclareInParameter("Width attenuation", CKPGUID_PERCENTAGE, "2");
    proto->DeclareInParameter("Length", CKPGUID_PERCENTAGE, "90");
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,0,0,0");
    proto->DeclareInParameter("Frame delay", CKPGUID_INT, "1");
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);

    proto->DeclareLocalParameter("Object id", CKPGUID_3DENTITY);
    proto->DeclareLocalParameter("First point index", CKPGUID_INT);
    proto->DeclareLocalParameter("Second point index", CKPGUID_INT);
    proto->DeclareLocalParameter("Framedelay count", CKPGUID_INT, 0);
    proto->DeclareLocalParameter("Active", CKPGUID_BOOL, 0);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SolidTrail);

    proto->SetBehaviorCallbackFct(SolidTrailCallBackObject);

    *pproto = proto;
    return CK_OK;
}

void ResetMesh(CKBehavior *beh)
{
    // we get the trail object
    CK3dEntity *ent = (CK3dEntity *)beh->GetLocalParameterObject(0);
    if (!ent)
        return;

    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return;

    // we get the 3d entity
    CK3dEntity *object = (CK3dEntity *)beh->GetOwner();
    if (!object)
        return; // no owner

    CKMesh *omesh = object->GetCurrentMesh();
    if (!omesh)
        return;

    VxVector a, b;

    // we get the two extreme points index
    int li, lj;
    beh->GetLocalParameterValue(1, &li);
    beh->GetLocalParameterValue(2, &lj);

    // we get the two new firsts points
    omesh->GetVertexPosition(li, &a);
    omesh->GetVertexPosition(lj, &b);

    VxVector resa, resb;
    const VxMatrix &matrix = object->GetWorldMatrix();

    // We transform the points in the world
    Vx3DMultiplyMatrixVector(&resa, matrix, &a);
    Vx3DMultiplyMatrixVector(&resb, matrix, &b);

    for (int i = 0; i < TRACKS_NUMBER + 1; i++)
    {
        mesh->SetVertexPosition(i * 2, &resa);
        mesh->SetVertexPosition(i * 2 + 1, &resb);
    }
}

int SolidTrail(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the trail object
    CK3dEntity *ent = (CK3dEntity *)beh->GetLocalParameterObject(0);
    CKMesh *mesh = ent->GetCurrentMesh();

    //	BOOL mustStop;
    // If the Off input is active, we stop looping
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        ent->Show(CKHIDE);
        CKBOOL active = FALSE;
        beh->SetLocalParameterValue(4, &active);
        //		mustStop = TRUE;
        return CK_OK;
    }
    else if (beh->IsInputActive(0))
    {
        ResetMesh(beh);

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        ent->Show(CKSHOW);
        CKBOOL active = TRUE;
        beh->SetLocalParameterValue(4, &active);
        //		mustStop = FALSE;
    }

    // we get the frame delay count (local parameter)
    int framecount;
    beh->GetLocalParameterValue(3, &framecount);

    if (framecount)
    {
        framecount--;
        beh->SetLocalParameterValue(3, &framecount);
        return CKBR_ACTIVATENEXTFRAME;
    }

    // we get the frame delay count (local parameter)
    int framedelay;
    beh->GetInputParameterValue(3, &framedelay);
    if (framedelay == 0)
        framedelay = 1;
    framecount = framedelay - 1;
    beh->SetLocalParameterValue(3, &framecount);

    // we get the width attenuation (input parameter)
    float widthatt;
    beh->GetInputParameterValue(0, &widthatt);

    // we get the length attenuation (input parameter)
    float lengthatt;
    beh->GetInputParameterValue(1, &lengthatt);

    // we get the color (input parameter)
    VxColor color;
    beh->GetInputParameterValue(2, &color);

    // we get the width attenuation (input parameter)
    CKBOOL active;
    beh->GetLocalParameterValue(4, &active);

    // we get the two extreme points index
    int li, lj;
    beh->GetLocalParameterValue(1, &li);
    beh->GetLocalParameterValue(2, &lj);

    // we get the 3d entity
    CK3dEntity *object = (CK3dEntity *)beh->GetOwner();
    CKMesh *omesh = object->GetCurrentMesh();

    CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(4);
    CKMaterial *mat = mesh->GetFaceMaterial(0);
    mat->SetTexture0(tex);

    VxVector a, b;
    /*
    if(mustStop) {
    if(!active) { // we finished to make the points disappear
    ent->Show(FALSE);
    return CKBR_OK;
    }
    } else {
    if(!active) { // we start the trail : we init all the points
    int i;
    for(i=0;i<TRACKS_NUMBER;i++) {
                omesh->GetVertexPosition(li,&a);
                omesh->GetVertexPosition(lj,&b);
                mesh->SetVertexPosition(i*2,&a);
                mesh->SetVertexPosition(i*2+1,&b);
                }
                active=TRUE;
                beh->SetLocalParameterValue(4,&active);
                }
                }
    */
    // we get the texture (input parameter)
    //	CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(4);
    //	if(tex){
    //		CKMaterial* mat = mesh->GetFaceMaterial(0);
    //		mesh->SetLitMode( VX_LITMESH );
    //		mat->SetTexture(tex);
    //		mat->SetSourceBlend( VXBLEND_SRCALPHA);
    //		mat->SetDestBlend( VXBLEND_INVSRCALPHA);
    //	}

    // vector first to second
    VxVector fts;

    // we move all the points
    float la = 0;
    int i;
    for (i = TRACKS_NUMBER; i > 0; i--)
    {
        mesh->GetVertexPosition((i - 1) * 2 + 1, &a);
        mesh->GetVertexPosition((i - 1) * 2, &b);
        fts = (a - b) * widthatt;

        b += fts;
        mesh->SetVertexPosition(i * 2, &b);
        a -= fts;
        mesh->SetVertexPosition(i * 2 + 1, &a);
        /*		if(mustStop) {
        float a;
        if(i==TRACKS_NUMBER) {
        DWORD c=mesh->GetVertexColor((i+1)*2);
        a=(float)(ColorGetAlpha(c))/255.0f;
        } else {
        a=0.0f;
        }
        mesh->SetVertexColor(i*2,RGBAFTOCOLOR(color.r*a,color.g*a,color.b*a,1.0));
        mesh->SetVertexColor(i*2+1,RGBAFTOCOLOR(color.r*a,color.g*a,color.b*a,1.0));
        } else {
        */
        mesh->SetVertexColor(i * 2, RGBAFTOCOLOR(color.r * la, color.g * la, color.b * la, 1.0));
        mesh->SetVertexColor(i * 2 + 1, RGBAFTOCOLOR(color.r * la, color.g * la, color.b * la, 1.0));
        if (i < TRACKS_NUMBER * lengthatt)
        {
            la += 1.0f / (TRACKS_NUMBER * lengthatt);
        }
        //		}
    }
    // we update the bounding extents
    mesh->VertexMove();

    // we get the two new firsts points
    omesh->GetVertexPosition(li, &a);
    omesh->GetVertexPosition(lj, &b);

    VxVector res;
    const VxMatrix &matrix = object->GetWorldMatrix();
    Vx3DMultiplyMatrixVector(&res, matrix, &a);
    mesh->SetVertexPosition(0, &res);
    mesh->SetVertexColor(0, RGBAFTOCOLOR(color.r, color.g, color.b, 1.0));
    Vx3DMultiplyMatrixVector(&res, matrix, &b);
    mesh->SetVertexPosition(1, &res);
    mesh->SetVertexColor(1, RGBAFTOCOLOR(color.r, color.g, color.b, 1.0));

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SolidTrailCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    {
        // here when attached
        // first we find the two extreme points of the attached object
        CK3dEntity *object = (CK3dEntity *)beh->GetOwner();
        if (!object)
            return CKBR_OWNERERROR;

        const VxMatrix &matrix = object->GetWorldMatrix();
        CKMesh *mesh = object->GetCurrentMesh();

        VxVector first, second, a, b;
        float max = 0, mag;
        int vnumber = mesh->GetVertexCount();
        int li, lj;
        int i;
        for (i = 0; i < vnumber - 1; i++)
        {
            int j;
            for (j = i; j < vnumber; j++)
            {
                mesh->GetVertexPosition(i, &a);
                mesh->GetVertexPosition(j, &b);
                mag = SquareMagnitude(a - b);
                if (mag > max)
                {
                    max = mag;
                    Vx3DMultiplyMatrixVector(&first, matrix, &a);
                    li = i;
                    Vx3DMultiplyMatrixVector(&second, matrix, &b);
                    lj = j;
                }
            }
        }

        // now we create the trail
        CKMesh *trailmesh = (CKMesh *)behcontext.Context->CreateObject(CKCID_MESH, "STMesh", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        trailmesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        // we get the color (input parameter)
        VxColor color;
        beh->GetInputParameterValue(2, &color);

        // we create the Vertices
        trailmesh->SetLitMode(VX_PRELITMESH);
        trailmesh->SetTransparent(TRUE);
        trailmesh->SetVertexCount(2 + TRACKS_NUMBER * 2);
        // first two points
        trailmesh->SetVertexSpecularColor(0, RGBAITOCOLOR(0, 0, 0, 0));
        trailmesh->SetVertexColor(0, RGBAITOCOLOR((int)(255 * color.r), (int)(255 * color.g), (int)(255 * color.b), 255));
        trailmesh->SetVertexPosition(0, &first);
        trailmesh->SetVertexTextureCoordinates(0, 0, 0);
        trailmesh->SetVertexSpecularColor(1, RGBAITOCOLOR(0, 0, 0, 0));
        trailmesh->SetVertexColor(1, RGBAITOCOLOR((int)(255 * color.r), (int)(255 * color.g), (int)(255 * color.b), 255));
        trailmesh->SetVertexPosition(1, &second);
        trailmesh->SetVertexTextureCoordinates(1, 1, 0);
        // rest of the trail
        for (i = 0; i < TRACKS_NUMBER; i++)
        {
            trailmesh->SetVertexSpecularColor(2 + i * 2, RGBAITOCOLOR(0, 0, 0, 0));
            trailmesh->SetVertexColor(2 + i * 2, RGBAITOCOLOR((int)(255 * color.r), (int)(255 * color.g), (int)(255 * color.b), 255));
            trailmesh->SetVertexSpecularColor(2 + i * 2 + 1, RGBAITOCOLOR(0, 0, 0, 0));
            trailmesh->SetVertexColor(2 + i * 2 + 1, RGBAITOCOLOR((int)(255 * color.r), (int)(255 * color.g), (int)(255 * color.b), 255));
            trailmesh->SetVertexPosition(2 + i * 2, &first);
            trailmesh->SetVertexPosition(2 + i * 2 + 1, &second);
            trailmesh->SetVertexTextureCoordinates(2 + i * 2, 0.0, (float)((i / 2) % 2));
            trailmesh->SetVertexTextureCoordinates(2 + i * 2 + 1, 1.0, (float)((i / 2) % 2));
        }

        // we create the faces of the trail
        trailmesh->SetFaceCount(TRACKS_NUMBER * 2);
        for (i = 0; i < TRACKS_NUMBER; i++)
        {
            trailmesh->SetFaceVertexIndex(i * 2, i * 2, (i + 1) * 2, i * 2 + 1);
            trailmesh->SetFaceVertexIndex(i * 2 + 1, (i + 1) * 2, (i + 1) * 2 + 1, i * 2 + 1);
        }

        // we calculate the normals
        trailmesh->BuildFaceNormals();

        // we create a material
        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "STMaterial", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        mat->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        VxColor col(1.0f, 1.0f, 1.0f, 0.1f);
        ;
        mat->SetSourceBlend(VXBLEND_ONE);
        mat->SetDestBlend(VXBLEND_ONE);
        mat->EnableAlphaBlend(TRUE);
        mat->EnableZWrite(FALSE);
        mat->SetDiffuse(col);
        mat->SetTwoSided(TRUE);
        trailmesh->ApplyGlobalMaterial(mat);
        CKTexture *tex = (CKTexture *)beh->GetLocalParameterObject(5);
        mat->SetTexture0(tex);

        // we create a dummy entity
        CK3dEntity *ent = (CK3dEntity *)ctx->CreateObject(CKCID_3DENTITY, "STEntity", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        ent->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        ent->SetCurrentMesh(trailmesh);

        beh->SetLocalParameterObject(0, ent);
        beh->SetLocalParameterValue(1, &li);
        beh->SetLocalParameterValue(2, &lj);

        // ?????????????? Test !!!
        CKScene *scene = behcontext.CurrentScene;
        scene->AddObjectToScene(ent);

        // Inactive by default
        CKBOOL active = FALSE;
        beh->SetLocalParameterValue(4, &active);
    }
    break;
    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        CKBOOL active;
        beh->GetLocalParameterValue(4, &active);
        if (active)
        {
            CK3dEntity *entity = (CK3dEntity *)beh->GetLocalParameterObject(0);
            if (entity)
                entity->Show(CKSHOW);
        }
    }
    case CKM_BEHAVIORRESET:
    {
        ResetMesh(beh);
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        CK3dEntity *ent = (CK3dEntity *)beh->GetLocalParameterObject(0);
        CKDestroyObject(ent->GetCurrentMesh());
        CKDestroyObject(ent);
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    {
        CK3dEntity *entity = (CK3dEntity *)beh->GetLocalParameterObject(0);
        if (!entity)
            return CKBR_OWNERERROR;

        // we remove the emitter from all the scene it was in
        int i;
        for (i = 0; i < entity->GetSceneInCount(); i++)
        {
            CKScene *scene = entity->GetSceneIn(i);
            if (scene)
                scene->RemoveObjectFromScene(entity);
        }

        // we now have to add the emitter to the owner scene(s)
        CK3dEntity *owner = (CK3dEntity *)beh->GetOwner();
        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsParentScriptActiveInScene(scene))
        {
            scene->AddObjectToScene(entity);
            CKBOOL active;
            beh->GetLocalParameterValue(4, &active);
            if (active)
            {
                entity->Show(CKSHOW);
                ResetMesh(beh);
            }
        }
        else
        {
            if (owner->IsInScene(scene))
            {
                scene->AddObjectToScene(entity);
                entity->Show(CKSHOW);
            }
        }
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        CKBOOL active;
        beh->GetLocalParameterValue(4, &active);
        if (active)
        {
            CK3dEntity *entity = (CK3dEntity *)beh->GetLocalParameterObject(0);
            if (entity)
                entity->Show(CKHIDE);
        }
    }
    break;
    case CKM_BEHAVIORPAUSE:
    {
        CK3dEntity *entity = (CK3dEntity *)beh->GetLocalParameterObject(0);
        if (entity)
            entity->Show(CKHIDE);
    }
    break;
    }
    return CKBR_OK;
}
