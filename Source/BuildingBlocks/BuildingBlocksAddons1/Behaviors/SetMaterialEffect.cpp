/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              SetMaterialEffect
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetMaterialEffectDecl();
CKERROR CreateSetMaterialEffectProto(CKBehaviorPrototype **);
int SetMaterialEffect(const CKBehaviorContext &behcontext);
CKERROR SetMaterialEffectCallback(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorSetMaterialEffectDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Material Effect");
    od->SetDescription("Enables a special effect on a material.");
    /* rem:
     */
    od->SetCategory("Materials-Textures/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3bd73009, 0x7aa01dac));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetMaterialEffectProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    return od;
}

CKERROR CreateSetMaterialEffectProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Material Effect");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetMaterialEffect);
    proto->SetBehaviorCallbackFct(SetMaterialEffectCallback, CKCB_BEHAVIOREDITED);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");
    proto->DeclareInParameter("Effect", CKPGUID_MATERIALEFFECT, "0");
    proto->DeclareInParameter("Texture 1", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Texture 2", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Texture 3", CKPGUID_TEXTURE);

    *pproto = proto;
    return CK_OK;
}

int SetMaterialEffect(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();

    int effect;
    beh->GetInputParameterValue(0, &effect);
    mat->SetEffect((VX_EFFECT)effect);

    if (effect)
    {
        CKParameter *p1 = beh->GetInputParameter(1)->GetRealSource();
        CKParameter *p2 = beh->GetInputParameter(2)->GetRealSource();
        CKParameter *p3 = beh->GetInputParameter(3)->GetRealSource();
        if (p1)
        {
            CKTexture *Tex1 = (CKTexture *)behcontext.Context->GetObject(*(CK_ID *)p1->GetReadDataPtr());
            mat->SetTexture(1, Tex1);
        }
        if (p2)
        {
            CKTexture *Tex2 = (CKTexture *)behcontext.Context->GetObject(*(CK_ID *)p2->GetReadDataPtr());
            mat->SetTexture(2, Tex2);
        }

        if (p3)
        {
            CKTexture *Tex3 = (CKTexture *)behcontext.Context->GetObject(*(CK_ID *)p3->GetReadDataPtr());
            mat->SetTexture(3, Tex3);
        }

        //--- Copy parameter value if available...
        CKParameterIn *pIn = beh->GetInputParameter(4);
        if (pIn && mat->GetEffectParameter())
        {
            mat->GetEffectParameter()->CopyValue(pIn->GetRealSource());
        }
    }

    return CKBR_OK;
}

/*****************************************/
/*             Callback
/*****************************************/
CKERROR SetMaterialEffectCallback(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    int effect;
    beh->GetInputParameterValue(0, &effect);
    CKRenderManager *RdMan = context.Context->GetRenderManager();

    if (effect)
    {
        const VxEffectDescription &desc = RdMan->GetEffectDescription(effect);
        if (desc.ParameterType != CKGUID(0, 0))
        {
            // if the parameter already exist we only change its type and name
            // otherwise we must create it
            if (beh->GetInputParameterCount() > 4)
            {
                CKParameterIn *pIn = beh->GetInputParameter(4);
                pIn->SetGUID(desc.ParameterType, TRUE, (char *)desc.ParameterDescription.CStr());
            }
            else
            {
                beh->CreateInputParameter((char *)desc.ParameterDescription.CStr(), desc.ParameterType);
            }
        }
        else
        {
            CKDestroyObject(beh->RemoveInputParameter(4));
        }
    }
    else
    {
        // No effect selected , no parameter is needed
        CKDestroyObject(beh->RemoveInputParameter(4));
    }
    return CKBR_OK;
}