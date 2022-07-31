/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT SceneStencilShadow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorSceneStencilShadowDecl();
CKERROR CreateSceneStencilShadowProto(CKBehaviorPrototype **);
int SceneStencilShadow(const CKBehaviorContext &behcontext);
CKERROR SceneStencilShadowCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSceneStencilShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SceneStencilShadow");
    od->SetDescription("Creates stencil buffer shadows (32-bit)");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4DCF4CA0, 0x474E25EC));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSceneStencilShadowProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSceneStencilShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SceneStencilShadow");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Stop");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("ShadowColor", CKPGUID_COLOR, "0, 0, 0, 128");

    proto->DeclareLocalParameter(NULL, CKPGUID_POINTER);
    proto->DeclareLocalParameter(NULL, CKPGUID_2DENTITY);
    proto->DeclareLocalParameter(NULL, CKPGUID_MATERIAL);
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
    CKContext *context = behcontext.Context;
}

CKERROR SceneStencilShadowCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    default:
        break;
    }

    return CKBR_OK;
}
