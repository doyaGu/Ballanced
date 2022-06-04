/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		               TT Sky
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorSkyDecl();
CKERROR CreateSkyProto(CKBehaviorPrototype **);
int Sky(const CKBehaviorContext &behcontext);
CKERROR SkyCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSkyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Sky");
    od->SetDescription("Creates a sky object with any number of faces and material transparency.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x36691920, 0x3B261630));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSkyProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSkyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Sky");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Distortion", CKPGUID_PERCENTAGE, "30");
    proto->DeclareInParameter("Vertex Color", CKPGUID_COLOR, "1, 1, 1, 0");

    proto->DeclareInParameter("Orientation Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Radius", CKPGUID_FLOAT, "70.0f");
    proto->DeclareInParameter("Quadratic SideFaces?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("SideFace-Heigth", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Y-Position of Sky", CKPGUID_FLOAT, "0");

    proto->DeclareSetting("Side Materials", CKPGUID_INT, "4");
    proto->DeclareSetting("Top Materials", CKPGUID_BOOL, "True");
    proto->DeclareSetting("Bottom Materials", CKPGUID_BOOL, "True");
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
}

CKERROR SkyCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *eno = beh->GetOwner();
    if (!eno)
    {
        return CKBR_OWNERERROR;
    }

    char buffer[128];
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        if (eno->GetClassID() != CKCID_3DOBJECT)
        {
            return CKBR_OK;
        }

        // TODO
    }
    break;
    }

    return CKBR_OK;
}
