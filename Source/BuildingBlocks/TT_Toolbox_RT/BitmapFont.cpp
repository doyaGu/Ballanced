/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT_BitmapFont
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorBitmapFontDecl();
CKERROR CreateBitmapFontProto(CKBehaviorPrototype **);
int BitmapFont(const CKBehaviorContext &behcontext);
CKERROR BitmapFontCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBitmapFontDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_BitmapFont");
    od->SetDescription("Creates an object text from a texture");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x173E60FF, 0x4DB14BCC));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBitmapFontProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBitmapFontProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_BitmapFont");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Scale", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("cursiv", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Src Blend", CKPGUID_BLENDFACTOR);
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR);
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,255");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BitmapFont);

    proto->SetBehaviorCallbackFct(BitmapFontCallBack);

    *pproto = proto;
    return CK_OK;
}

int BitmapFont(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}

CKERROR BitmapFontCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    default:
        break;
    }

    return CKBR_OK;
}
