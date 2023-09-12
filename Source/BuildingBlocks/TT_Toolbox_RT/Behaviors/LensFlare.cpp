////////////////////////////////
////////////////////////////////
//
//        TT_LensFlare
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorLensFlareDecl();
CKERROR CreateLensFlareProto(CKBehaviorPrototype **pproto);
int LensFlare(const CKBehaviorContext &behcontext);
CKERROR LensFlareCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLensFlareDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_LensFlare");
    od->SetDescription("Creates camera reflections");
    od->SetCategory("TT Toolbox/Object");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x241b32a5, 0x71fe3357));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLensFlareProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateLensFlareProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_LensFlare");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Flares Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Flares Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Multiplier", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Source Blend", CKPGUID_BLENDFACTOR, "One");
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR, "Zero");

    proto->DeclareLocalParameter("", CKPGUID_POINTER);

    proto->DeclareSetting("Read Array Each Frame", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LensFlare);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(LensFlareCallBack);

    *pproto = proto;
    return CK_OK;
}

int LensFlare(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR LensFlareCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}