////////////////////////////////
////////////////////////////////
//
//        TT_SaveAlpha
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSaveAlphaDecl();
CKERROR CreateSaveAlphaProto(CKBehaviorPrototype **pproto);
int SaveAlpha(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSaveAlphaDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SaveAlpha");
    od->SetDescription("Kopiert den Alpha-Kanal in die RGB-Werte der Texture und speichert diese in eine TGA-Datei");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ca62d31, 0x5d2031b8));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSaveAlphaProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSaveAlphaProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SaveAlpha");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Filename", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SaveAlpha);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SaveAlpha(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}