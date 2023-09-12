/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT RealShadowMapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

#define A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT 50

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
    CK_ID matID;                                     // material to be projected
    CK_ID texID;                                     // texture to be associated with the projected material
    CK_ID floor[A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT]; // floors under the object ...
    int nb_floors_under;                             // number of floors being under the object...
} SimpleShadowStruct;

void A_GetFloors(SimpleShadowStruct *tss, CK3dEntity *ent, CKBehavior *beh, VxVector *pos_rel, VxVector *scale, float maxHeight);
void A_Delete_SoftShadow_From_Floors(CK_ID *floor, CKMaterial *cmat, int nb_floors);

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
    if (!proto) return CKERR_OUTOFMEMORY;

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
            SimpleShadowStruct tss;

            tss.matID = CKOBJID(mat);
            tss.texID = -1;
            tss.nb_floors_under = 0;
            memset(tss.floor, 0, A_MAX_NUMBER_OF_FLOOR_UNDER_OBJECT * sizeof(CK_ID));

            beh->SetLocalParameterValue(0, &tss, sizeof(SimpleShadowStruct));
        }
            break;
        case CKM_BEHAVIORDETACH:
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
                A_Delete_SoftShadow_From_Floors(tss->floor, mat, tss->nb_floors_under);
            else
                return 0;
        }
            break;
    }
    return CKBR_OK;
}

void RealShadowMappingRenderCallBack(CKRenderContext *dev, void *arg)
{
    CKBehavior* beh = (CKBehavior*)arg;
}

