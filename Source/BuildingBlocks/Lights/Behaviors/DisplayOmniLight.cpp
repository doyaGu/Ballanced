/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DisplayOmniLight
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDisplayOmniLightDecl();
CKERROR CreateDisplayOmniLightProto(CKBehaviorPrototype **);
int DisplayOmniLight(const CKBehaviorContext &behcontext);
CKERROR CallbackDisp(const CKBehaviorContext &behcontext);

typedef struct
{
    CK_ID spr_id;
    CK_ID mat_id;
    CKBOOL is_lighting;
} AML;

CKObjectDeclaration *FillBehaviorDisplayOmniLightDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Light Sprite");
    od->SetDescription("Renders the Light with a specific 'Light Texture'.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Out (Loop): </SPAN>is activated when one building block process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Light Texture: </SPAN>name of the texture used as the sprite.<BR>
    <SPAN CLASS=pin>Size Factor: </SPAN>as a percentage, with the Sprite having the same size as the light's range at 100%.<BR>
    */
    /* warning:
    - This building block only needs to be looped if the <SPAN CLASS=pin>Light Texture</SPAN> or the <SPAN CLASS=pin>Size Factor</SPAN>
    need to be changed at each frame.<BR>
    To loop it just link the 'Out (Loop)' output to the 'On' input.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4def3319, 0x075b5cc8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDisplayOmniLightProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/FX");
    return od;
}

CKERROR CreateDisplayOmniLightProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Light Sprite");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out (Loop)");

    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Scale", CKPGUID_PERCENTAGE, "100");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); //"Data AML"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DisplayOmniLight);

    proto->SetBehaviorCallbackFct(CallbackDisp);

    *pproto = proto;
    return CK_OK;
}

int DisplayOmniLight(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKLight *ent = (CKLight *)beh->GetOwner();
    AML *aml = (AML *)beh->GetLocalParameterWriteDataPtr(0);
    CKSprite3D *cksprite3d = (CKSprite3D *)ctx->GetObject(aml->spr_id);

    if (!(aml->is_lighting))
    {
        aml->is_lighting = TRUE;
    }

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        CKMaterial *ckmat = (CKMaterial *)ctx->GetObject(aml->mat_id);

        CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(0);
        ckmat->SetTexture0(tex);

        CKDWORD col = ent->GetColor().GetRGB();
        col |= 0xfe000000;
        ckmat->SetDiffuse(VxColor(col));

        float size_factor = 0.1f; // we get the size factor
        beh->GetInputParameterValue(1, &size_factor);

        float size = ent->GetRange() * size_factor; // we set so the sprite's size
        Vx2DVector v(size, size);
        cksprite3d->SetSize(v);
    }

    if (aml->is_lighting)
    {
        // we set position of the sprite each frame
        VxVector V;
        ent->GetPosition(&V);
        cksprite3d->SetPosition(&V);

        cksprite3d->Show();

        if (beh->IsInputActive(1))
        { // We enter by 'OFF'
            beh->ActivateInput(1, FALSE);
            beh->ActivateOutput(0, TRUE);

            cksprite3d->Show(CKHIDE);

            aml->is_lighting = FALSE;

            return CKBR_OK;
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR CallbackDisp(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        AML aml;

        aml.is_lighting = FALSE;

        // We create the 3dsprite
        CKSprite3D *cksprite3d = (CKSprite3D *)ctx->CreateObject(CKCID_SPRITE3D, "DOL Sprite3D", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        cksprite3d->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        // Add 3d Sprite to the Level
        behcontext.CurrentLevel->AddObject(cksprite3d);

        // and save it in the inner structure
        aml.spr_id = cksprite3d->GetID();

        // We create the Material of the sprite
        CKMaterial *ckmat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "DOL Material", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        ckmat->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        aml.mat_id = ckmat->GetID();

        beh->SetLocalParameterValue(0, &aml, sizeof(AML));

        VxColor col;
        col.a = 0.9f;

        ckmat->EnableAlphaBlend();
        ckmat->EnableZWrite(FALSE);
        ckmat->SetSourceBlend(VXBLEND_ONE);
        ckmat->SetDestBlend(VXBLEND_ONE);
        ckmat->SetDiffuse(col);
        col.Set(0, 0, 0);
        ckmat->SetSpecular(col);

        // We apply the Material to the sprite
        cksprite3d->SetMaterial(ckmat);

        // We tell the sprite not to be pickable
        cksprite3d->SetPickable(FALSE);

        // we don't want the sprite to be visible  ( we wait for the 'ON' entry )
        cksprite3d->Show(CKHIDE);
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        AML *aml = (AML *)beh->GetLocalParameterWriteDataPtr(0);
        CKSprite3D *cksprite3d = (CKSprite3D *)ctx->GetObject(aml->spr_id);
        CKDestroyObject(cksprite3d);
    }
    break;

    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        AML *aml = (AML *)beh->GetLocalParameterWriteDataPtr(0);
        CKSprite3D *cksprite3d = (CKSprite3D *)ctx->GetObject(aml->spr_id);

        cksprite3d->Show(CKHIDE);
    }
    break;

    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
    {
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
        AML *aml = (AML *)beh->GetLocalParameterWriteDataPtr(0);
        CKSprite3D *cksprite3d = (CKSprite3D *)ctx->GetObject(aml->spr_id);

        // remove the object from all scene it was in
        for (int i = 0; i < cksprite3d->GetSceneInCount(); i++)
        {
            CKScene *scene = cksprite3d->GetSceneIn(i);
            if (scene)
                scene->RemoveObjectFromScene(cksprite3d);
        }

        // if script actif in scene, then add the object to the new scene.
        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsParentScriptActiveInScene(scene))
        {
            scene->AddObjectToScene(cksprite3d);
        }
    }
    break;
    }
    return CKBR_OK;
}
