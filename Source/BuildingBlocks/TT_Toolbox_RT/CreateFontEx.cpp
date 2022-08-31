///////////////////////////////////
///////////////////////////////////
//
//        TT_CreateFontEx
//
///////////////////////////////////
///////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTCreateFontExBehaviorProto(CKBehaviorPrototype **pproto);
int TTCreateFontEx(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTCreateFontExDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CreateFontEx");
    od->SetDescription("Creates a font from a provided texture.");
    od->SetCategory("TT Toolbox/Font");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x260e4eb0,0xe256b90));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTCreateFontExBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTCreateFontExBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT CreateFontEx");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Font Name", CKPGUID_STRING, "MyFont");
    proto->DeclareInParameter("Font Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Horizontal/Vertical Character Number", CKPGUID_2DVECTOR, "16,8");
    proto->DeclareInParameter("Proportionnal", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Font Bounds", CKPGUID_RECT, "(0,0),(0,0)");
    proto->DeclareInParameter("First Character", CKPGUID_INT, "0");
    proto->DeclareInParameter("FontCoordinatesData", CKPGUID_DATAARRAY);

    proto->DeclareOutParameter("Font", );

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTCreateFontEx);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    *pproto = proto;
    return CK_OK;
}

int TTCreateFontEx(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTCreateFontExCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}