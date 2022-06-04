/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT_LensFlare
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorLensFlareDecl();
CKERROR CreateLensFlareProto(CKBehaviorPrototype **);
int LensFlare(const CKBehaviorContext &behcontext);
CKERROR LensFlareCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLensFlareDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_LensFlare");
    od->SetDescription("Creates camera reflections");
    od->SetCategory("TT Toolbox/Object");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x241B32A5, 0x71FE3357));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLensFlareProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateLensFlareProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_LensFlare");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Flares Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Flares Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Multiplier", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Src Blend", CKPGUID_BLENDFACTOR, "One");
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR, "Zero");

    proto->DeclareLocalParameter(NULL, CKPGUID_POINTER);
    proto->DeclareSetting("Read Array Each Frame", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LensFlare);

    proto->SetBehaviorCallbackFct(LensFlareCallBack);
	proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int LensFlare(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}

CKERROR LensFlareCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    default:
        break;
    }

    return CKBR_OK;
}
