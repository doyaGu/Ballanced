/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//        TT_FontCoordinatesToDataArray
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorFontCoordinatesToDataArrayDecl();
CKERROR CreateFontCoordinatesToDataArrayProto(CKBehaviorPrototype **pproto);
int FontCoordinatesToDataArray(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFontCoordinatesToDataArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_FontCoordinatesToDataArray");
    od->SetDescription("FontCoordinates To DataArray");
    od->SetCategory("TT Toolbox/Font");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x73e256e5, 0x6552118e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFontCoordinatesToDataArrayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateFontCoordinatesToDataArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_FontCoordinatesToDataArray");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("ArrayName", CKPGUID_STRING, "MyFontData");
    proto->DeclareInParameter("Font", CKPGUID_FONT);

    proto->DeclareOutParameter("FontCoordinatesData", CKPGUID_DATAARRAY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FontCoordinatesToDataArray);

    *pproto = proto;
    return CK_OK;
}

int FontCoordinatesToDataArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}