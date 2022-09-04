/////////////////////////////////
/////////////////////////////////
//
//        TT_CelShading
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorCelShadingDecl();
CKERROR CreateCelShadingProto(CKBehaviorPrototype **pproto);
int CelShading(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCelShadingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CelShading");
    od->SetDescription("Cartoon-Shader");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2d2d2a64, 0x5a0501c7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCelShadingProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCelShadingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_CelShading");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,128");
    proto->DeclareInParameter("Outline", CKPGUID_BOOL, "false");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CelShading);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int CelShading(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}