///////////////////////////////////
///////////////////////////////////
//
//        TT_AnisoMapping
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorAnisoMappingDecl();
CKERROR CreateAnisoMappingProto(CKBehaviorPrototype **pproto);
int AnisoMapping(const CKBehaviorContext &behcontext);
CKERROR AnisoMappingCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAnisoMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_AnisoMapping");
    od->SetDescription("erzeugt lichtabh�ngiges BumpMapping");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x73b30b5e, 0x4fed4c65));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAnisoMappingProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateAnisoMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_AnisoMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Camera", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Texture Size", CKPGUID_INT, "128");
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,128");
    proto->DeclareInParameter("Metal", CKPGUID_BOOL, "false");
    proto->DeclareInParameter("Mapping", CKPGUID_MAPPING, "1");
    proto->DeclareInParameter("Size", CKPGUID_FLOAT, "1.0");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AnisoMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(AnisoMappingCallBack);

    *pproto = proto;
    return CK_OK;
}

int AnisoMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR AnisoMappingCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}