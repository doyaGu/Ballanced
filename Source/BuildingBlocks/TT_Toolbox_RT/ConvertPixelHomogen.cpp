///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT ConvertPixel-Homogen
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorConvertPixelHomogenDecl();
CKERROR CreateConvertPixelHomogenProto(CKBehaviorPrototype **pproto);
int ConvertPixelHomogen(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorConvertPixelHomogenDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ConvertPixel-Homogen");
    od->SetDescription("Converts pixel coordinates to homogeneous coordinates and vice versa");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x18f96977, 0x18e20f83));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateConvertPixelHomogenProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateConvertPixelHomogenProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ConvertPixel-Homogen");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Exit On");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");

    proto->DeclareOutParameter("X", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Y", CKPGUID_FLOAT);
    proto->DeclareOutParameter("2D Vector", CKPGUID_2DVECTOR);

    proto->DeclareSetting("Pixel to homogen?:", CKPGUID_BOOL, "true");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ConvertPixelHomogen);

    *pproto = proto;
    return CK_OK;
}

int ConvertPixelHomogen(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}