////////////////////////////////
////////////////////////////////
//
//        TT_SaveAlpha
//
////////////////////////////////
////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSaveAlphaBehaviorProto(CKBehaviorPrototype **pproto);
int TTSaveAlpha(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSaveAlphaDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SaveAlpha");
    od->SetDescription("Kopiert den Alpha-Kanal in die RGB-Werte der Texture und speichert diese in eine TGA-Datei");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ca62d31,0x5d2031b8));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSaveAlphaBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSaveAlphaBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SaveAlpha");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Filename", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSaveAlpha);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTSaveAlpha(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSaveAlphaCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}