/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT RealShadowMapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorRealShadowMappingDecl();
CKERROR CreateRealShadowMappingProto(CKBehaviorPrototype **pproto);
int RealShadowMapping(const CKBehaviorContext &behcontext);
CKERROR RealShadowMappingCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRealShadowMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT RealShadowMapping");
    od->SetDescription("calculate the real postion of shadow");
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
    CKContext *context = behcontext.Context;
}

CKERROR RealShadowMappingCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    default:
        break;
    }

    return CKBR_OK;
}
