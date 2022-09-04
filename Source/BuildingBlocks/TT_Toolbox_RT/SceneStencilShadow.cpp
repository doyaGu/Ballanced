/////////////////////////////////////////
/////////////////////////////////////////
//
//        TT_SceneStencilShadow
//
/////////////////////////////////////////
/////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSceneStencilShadowDecl();
CKERROR CreateSceneStencilShadowProto(CKBehaviorPrototype **pproto);
int SceneStencilShadow(const CKBehaviorContext &behcontext);
CKERROR SceneStencilShadowCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSceneStencilShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SceneStencilShadow");
    od->SetDescription("erzeugt StencilBuffer-Schatten (32Bit n�tig!)");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4dcf4ca0, 0x474e25ec));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSceneStencilShadowProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSceneStencilShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SceneStencilShadow");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("ShadowColor", CKPGUID_COLOR, "0, 0, 0, 128");

    proto->DeclareLocalParameter("", CKPGUID_POINTER);
    proto->DeclareLocalParameter("", CKPGUID_2DENTITY);
    proto->DeclareLocalParameter("", CKPGUID_MATERIAL);
    proto->DeclareLocalParameter("LigthMatrix", CKPGUID_MATRIX);
    proto->DeclareLocalParameter("ObjectCount", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SceneStencilShadow);

    proto->SetBehaviorCallbackFct(SceneStencilShadowCallBack);

    *pproto = proto;
    return CK_OK;
}

int SceneStencilShadow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR SceneStencilShadowCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}