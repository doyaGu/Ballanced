///////////////////////////////////
///////////////////////////////////
//
//        TT FollowCamera
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorFollowCameraDecl();
CKERROR CreateFollowCameraProto(CKBehaviorPrototype **pproto);
int FollowCamera(const CKBehaviorContext &behcontext);
CKERROR FollowCameraCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFollowCameraDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT FollowCamera");
    od->SetDescription("Camera follows an object");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x595b4723, 0x576c6749));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFollowCameraProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateFollowCameraProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT FollowCamera");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Stop");

    proto->DeclareInParameter("Target Entity", CKPGUID_3DENTITY);
    proto->DeclareInParameter("View Offset", CKPGUID_VECTOR);
    proto->DeclareInParameter("View Attenuation", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Camera Offset", CKPGUID_VECTOR);
    proto->DeclareInParameter("Camera Attenuation", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("FOV Angle", CKPGUID_ANGLE, "0:50");
    proto->DeclareInParameter("Back Attenuation", CKPGUID_PERCENTAGE, "25");

    proto->DeclareLocalParameter("ViewPosOld", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("CamPosOld", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("oldDeltaTime", CKPGUID_FLOAT);
    proto->DeclareLocalParameter("init", CKPGUID_BOOL, "true");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Init actPos", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FollowCamera);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(FollowCameraCallBack);

    *pproto = proto;
    return CK_OK;
}

int FollowCamera(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR FollowCameraCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}