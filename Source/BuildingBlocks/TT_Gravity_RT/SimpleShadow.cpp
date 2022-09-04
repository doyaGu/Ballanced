/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT Simple Shadow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorSimpleShadowDecl();
CKERROR CreateSimpleShadowProto(CKBehaviorPrototype **pproto);
int SimpleShadow(const CKBehaviorContext &behcontext);
CKERROR SimpleShadowCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSimpleShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Simple Shadow");
    od->SetDescription("Display a little soft shadow on floors beneath the object.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7f0517c2, 0x52cc76bb));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSimpleShadowProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateSimpleShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Simple Shadow");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Scale", CKPGUID_FLOAT, "2.0");
    proto->DeclareInParameter("Maximum Height", CKPGUID_FLOAT, "100.0");

    proto->DeclareLocalParameter("Data", CKPGUID_POINTER, "NULL");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SimpleShadow);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SimpleShadowCallBack);

    *pproto = proto;
    return CK_OK;
}

int SimpleShadow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}

CKERROR SimpleShadowCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    default:
        break;
    }

    return CKBR_OK;
}
