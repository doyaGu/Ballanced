/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT ShadowPlane
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorShadowPlaneDecl();
CKERROR CreateShadowPlaneProto(CKBehaviorPrototype **);
int ShadowPlane(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorShadowPlaneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ShadowPlane");
    od->SetDescription("Creates shadow planes without ZBuffer writing");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x137C3D66, 0x3FF1752D));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShadowPlaneProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateShadowPlaneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ShadowPlane");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Stop");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("SourceBlend", CKPGUID_BLENDFACTOR, "Zero");
    proto->DeclareInParameter("DestBlend", CKPGUID_BLENDFACTOR, "Source Color");

    proto->DeclareLocalParameter("ShadowStruct", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShadowPlane);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ShadowPlane(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}