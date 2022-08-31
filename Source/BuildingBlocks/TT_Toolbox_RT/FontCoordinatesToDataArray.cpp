/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//        TT_FontCoordinatesToDataArray
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTFontCoordinatesToDataArrayBehaviorProto(CKBehaviorPrototype **pproto);
int TTFontCoordinatesToDataArray(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTFontCoordinatesToDataArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_FontCoordinatesToDataArray");
    od->SetDescription("FontCoordinates To DataArray");
    od->SetCategory("TT Toolbox/Font");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x73e256e5,0x6552118e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTFontCoordinatesToDataArrayBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTFontCoordinatesToDataArrayBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_FontCoordinatesToDataArray");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("ArrayName", CKPGUID_STRING, "MyFontData");
    proto->DeclareInParameter("Font", );

    proto->DeclareOutParameter("FontCoordinatesData", CKPGUID_DATAARRAY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTFontCoordinatesToDataArray);

    *pproto = proto;
    return CK_OK;
}

int TTFontCoordinatesToDataArray(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTFontCoordinatesToDataArrayCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}