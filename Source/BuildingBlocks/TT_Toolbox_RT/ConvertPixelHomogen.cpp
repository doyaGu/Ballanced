///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT ConvertPixel-Homogen
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTConvertPixelHomogenBehaviorProto(CKBehaviorPrototype **pproto);
int TTConvertPixelHomogen(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTConvertPixelHomogenDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ConvertPixel-Homogen");
    od->SetDescription("konvertiert Pixelkoordinaten in homogene Koordinaten und umgekehrt");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x18f96977,0x18e20f83));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTConvertPixelHomogenBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTConvertPixelHomogenBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ConvertPixel-Homogen");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");

    proto->DeclareOutParameter("X", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Y", CKPGUID_FLOAT);
    proto->DeclareOutParameter("2D Vector", CKPGUID_2DVECTOR);

    proto->DeclareSetting("Pixel to homogen?:", CKPGUID_BOOL, "true");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTConvertPixelHomogen);

    *pproto = proto;
    return CK_OK;
}

int TTConvertPixelHomogen(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTConvertPixelHomogenCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}