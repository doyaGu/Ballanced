/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
//        TT_FontCoordinatesToDataArray
//
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "CKFontManager.h"
#include "CKTextureFont.h"

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
    CKContext *ctx = behcontext.Context;

    // Get array name
    CKSTRING arrayName = (CKSTRING)beh->GetInputParameterReadDataPtr(0);

    // Create data array
    CKDataArray *array = (CKDataArray *)ctx->CreateObject(CKCID_DATAARRAY, arrayName, CK_OBJECTCREATION_NONAMECHECK);
    CKScene *scene = ctx->GetCurrentScene();

    // Add columns for font coordinates
    array->InsertColumn(-1, CKARRAYTYPE_FLOAT, "ustart");
    array->InsertColumn(-1, CKARRAYTYPE_FLOAT, "vstart");
    array->InsertColumn(-1, CKARRAYTYPE_FLOAT, "uwidth");
    array->InsertColumn(-1, CKARRAYTYPE_FLOAT, "uprewidth");
    array->InsertColumn(-1, CKARRAYTYPE_FLOAT, "upostwidth");
    array->InsertColumn(-1, CKARRAYTYPE_FLOAT, "vwidth");

    scene->AddObjectToScene(array, TRUE);

    // Get font manager
    CKFontManager *fontMgr = (CKFontManager *)ctx->GetManagerByGuid(CKGUID(0x64FB5810, 0x73262D3B));
    if (!fontMgr)
        return CKBR_PARAMETERERROR;

    int fontIndex = 0;
    beh->GetInputParameterValue(1, &fontIndex);

    CKTextureFont *font = fontMgr->GetFont(fontIndex);
    if (!font)
        return CKBR_PARAMETERERROR;

    // Export font coordinates for all 255 characters
    for (int i = 0; i < 255; ++i)
    {
        array->InsertRow(-1);

        const CharacterTextureCoordinates &coords = font->m_FontCoordinates[i];

        float ustart = coords.ustart;
        float vstart = coords.vstart;
        float uwidth = coords.uwidth;
        float uprewidth = coords.uprewidth;
        float upostwidth = coords.upostwidth;
        float vwidth = coords.vwidth;

        array->SetElementValue(i, 0, &ustart);
        array->SetElementValue(i, 1, &vstart);
        array->SetElementValue(i, 2, &uwidth);
        array->SetElementValue(i, 3, &uprewidth);
        array->SetElementValue(i, 4, &upostwidth);
        array->SetElementValue(i, 5, &vwidth);
    }

    beh->SetOutputParameterObject(0, array);
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}