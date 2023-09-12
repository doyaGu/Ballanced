/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		               TT Sky
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

CKObjectDeclaration *FillBehaviorSkyDecl();
CKERROR CreateSkyProto(CKBehaviorPrototype **pproto);
int Sky(const CKBehaviorContext &behcontext);
CKERROR SkyCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSkyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Sky");
    od->SetDescription("Creates a sky object with any number of faces and changeable material transparency");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x36691920, 0x3b261630));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSkyProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSkyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Sky");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Distortion", CKPGUID_PERCENTAGE, "30");
    proto->DeclareInParameter("Vertex Color", CKPGUID_COLOR, "1, 1, 1, 0");
    proto->DeclareInParameter("Orientation Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Radius", CKPGUID_FLOAT, "70.0f");
    proto->DeclareInParameter("Quadratic SideFaces?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("or SideFace-Heigth", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Y-Position of Sky", CKPGUID_FLOAT, "0");

    proto->DeclareSetting("Side Materials", CKPGUID_INT, "4");
    proto->DeclareSetting("Top Material", CKPGUID_BOOL, "True");
    proto->DeclareSetting("Bottom Material", CKPGUID_BOOL, "True");

    proto->DeclareLocalParameter("Skyaround", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Sky);

    proto->SetBehaviorCallbackFct(SkyCallBack);

    *pproto = proto;
    return CK_OK;
}

int Sky(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

	// TODO

    return CKBR_OK;
}

CKERROR SkyCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

	// TODO
	
    return CKBR_OK;
}