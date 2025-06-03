/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SpriteMultiAngle
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSpriteMultiAngleDecl();
CKERROR CreateSpriteMultiAngleProto(CKBehaviorPrototype **pproto);
int SpriteMultiAngle(const CKBehaviorContext &behcontext);
CKERROR spriteinitcallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSpriteMultiAngleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sprite Multi Angle");
    od->SetDescription("Replaces the 3D Entity by a 3D sprite.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scale X : </SPAN>scaling factor along the X axis of the 3D Sprite.<BR>
    <SPAN CLASS=pin>Scale Y : </SPAN>scaling factor along the Y axis of the 3D Sprite.<BR>
    <SPAN CLASS=pin>Translucent : </SPAN>'True' allows color keying.<BR>
    <SPAN CLASS=pin>Color Key : </SPAN>color set to be transparent, expressed in RGBA ('Translucent' must be 'True').<BR>
    <SPAN CLASS=pin>Mirroring : </SPAN>set to 'True' to use multiple sprites (see below).<BR>
    <SPAN CLASS=pin>Sprite Texture : </SPAN>texture to use as a 3D Sprite.<BR>
    <BR>
    Preferably, apply this building block on a frame.<BR>
    You can use this building block to create 3D sprites.<BR>
    You can also simulate a 3D object by using a whole set of textures, representing the object seen from different angles.
    To do so, add input parameters for each new texture. You can use as many textures as needed.
    The first one should be a texture of the object seen from the front, the next been taken from the left, and so on.
    <BR>
    If the 3D object you want to simulate has a simetry, set 'Mirroring' to 'True. All the textures you provide will then be used to simulate the left side of the object, while the right side will use mirrored textures.
    For example, you can provide a FrontTexture, LeftTexture, BackTexture, and set 'Mirrored' to 'True' so that the RightTexture will be automatical
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1ef926bd, 0x1ad167f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSpriteMultiAngleProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/Sprite");
    return od;
}

CKERROR CreateSpriteMultiAngleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sprite Multi Angle");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scale X", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Scale Y", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Transparent", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Color Key", CKPGUID_COLOR, "0,0,0,0");
    proto->DeclareInParameter("Mirroring", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Sprite Texture", CKPGUID_TEXTURE);

    proto->DeclareLocalParameter("Sprite Sprite3D", CKPGUID_SPRITE3D);
    proto->DeclareLocalParameter("Sprite Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SpriteMultiAngle);
    proto->SetBehaviorFlags(CKBEHAVIOR_VARIABLEPARAMETERINPUTS);

    proto->SetBehaviorCallbackFct(spriteinitcallback);

    *pproto = proto;
    return CK_OK;
}

int SpriteMultiAngle(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO States
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKSprite3D *cksprite3d = (CKSprite3D *)beh->GetLocalParameterObject(0);
    // if(!cksprite3d) return 0;

    // Get inputs
    // we get a BOOL (input parameter)
    CKBOOL mirror;
    beh->GetInputParameterValue(4, &mirror);

    CK3dEntity *cam = behcontext.CurrentRenderContext->GetViewpoint();

    VxVector campos;
    cam->GetPosition(&campos);
    VxVector sprpos;
    cksprite3d->GetPosition(&sprpos);
    VxVector cts = campos - sprpos;

    float at = atan2f(cts.z, cts.x);
    at += PI;
    int pinc = beh->GetInputParameterCount() - 5;
    int tx;

    VxRect r(0.0f, 0.0f, 1.0f, 1.0f);

    cksprite3d->SetUVMapping(r);
    if (mirror)
    {
        tx = (int)(at * (float)(2 * pinc - 2) / (2.0f * PI));
        if (tx >= pinc)
        {
            VxRect r(1.0f, 0.0f, 0.0f, 1.0f);
            cksprite3d->SetUVMapping(r);
            tx = (pinc - 1) * 2 - tx;
        }
    }
    else
    {
        tx = (int)(at * (float)pinc / (2.0f * PI));
    }

    CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(5 + tx);

    // we get a BOOL (input parameter)
    CKBOOL t;
    beh->GetInputParameterValue(2, &t);

    tex->SetTransparent(t);
    if (t)
    {
        // we get a int (input parameter)
        VxColor col;
        beh->GetInputParameterValue(3, &col);

        tex->SetTransparentColor(RGBAFTOCOLOR(col.r, col.g, col.b, col.a));
    }

    CKMaterial *ckmat = (CKMaterial *)beh->GetLocalParameterObject(1);

    float sizex = 1.0f;
    beh->GetInputParameterValue(0, &sizex);
    float sizey = 1.0f;
    beh->GetInputParameterValue(1, &sizey);

    // Core
    ckmat->SetTexture0(tex);
    Vx2DVector vect(sizex, sizey);
    cksprite3d->SetSize(vect);

    return CKBR_OK;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR spriteinitcallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        // We create the 3dsprite
        CKSprite3D *cksprite3d = (CKSprite3D *)ctx->CreateObject(CKCID_SPRITE3D, "MultiSprite", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        cksprite3d->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        // if(!cksprite3d) return 0;
        cksprite3d->SetMode(VXSPRITE3D_YROTATE);
        // and save it as a local param
        beh->SetLocalParameterObject(0, cksprite3d);

        // We create the Material of the sprite
        CKMaterial *ckmat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "MultiMaterial", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        ckmat->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        ckmat->SetDiffuse(VxColor(1.0f, 1.0f, 1.0f, 1.0f));
        ckmat->SetSpecular(VxColor(0.0f, 0.0f, 0.0f));

        beh->SetLocalParameterObject(1, ckmat);

        // We apply the entry texture to the Material
        CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(5);

        ckmat->SetTexture0(tex);
        // we get a BOOL (input parameter)
        CKBOOL t;
        beh->GetInputParameterValue(2, &t);

        if (tex)
        {
            tex->SetTransparent(t);
            if (t)
            {
                // we get a int (input parameter)
                VxColor col;
                beh->GetInputParameterValue(3, &col);

                tex->SetTransparentColor(RGBAFTOCOLOR(col.r, col.g, col.b, col.a));
            }
        }

        float sizex;
        beh->GetInputParameterValue(0, &sizex);

        float sizey;
        beh->GetInputParameterValue(1, &sizey);

        Vx2DVector vect(sizex, sizey);
        cksprite3d->SetSize(vect);

        // We apply the Material to the sprite
        cksprite3d->SetMaterial(ckmat);

        // we now have to add the emitter to the owner scene(s)
        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsParentScriptActiveInScene(scene))
        {
            scene->AddObjectToScene(cksprite3d);
        }
    }
    break;

    case CKM_BEHAVIORATTACH:
    {
        CK3dEntity *ent = (CK3dEntity *)beh->GetOwner();
        // if(!ent) return 0;

        VxVector V(0.0, 0.0, 0.0);
        CKSprite3D *cksprite3d = (CKSprite3D *)beh->GetLocalParameterObject(0);
        cksprite3d->SetPosition(&V, ent);
        ent->Show(CKHIDE);
    }
    break;

    case CKM_BEHAVIORDETACH:
    {
        CK3dEntity *ent = (CK3dEntity *)beh->GetOwner();
        if (ent)
        {
            ent->Show(CKSHOW);
        }
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        CKSprite3D *spr = (CKSprite3D *)beh->GetLocalParameterObject(0);
        ctx->DestroyObject(spr);
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
        CKSprite3D *entity = (CKSprite3D *)beh->GetLocalParameterObject(0);
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

        // we now have to add the sprite to the owner scene(s)
        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsParentScriptActiveInScene(scene))
        {
            scene->AddObjectToScene(entity);
            entity->Show(CKSHOW);
        }
        else
        {
            if (beh->GetOwner()->IsInScene(scene))
            {
                scene->AddObjectToScene(entity);
                entity->Show(CKHIDE);
            }
        }
    }
    break;

    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        CKSprite3D *entity = (CKSprite3D *)beh->GetLocalParameterObject(0);
        if (entity)
            entity->Show(CKHIDE);
    }
    break;

    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        CKSprite3D *entity = (CKSprite3D *)beh->GetLocalParameterObject(0);
        if (entity)
            entity->Show(CKSHOW);
    }
    break;
    }
    return CKBR_OK;
}
