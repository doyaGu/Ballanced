/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	Load a movie using or not Threads
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMovieLoaderDecl();
CKERROR CreateMovieLoaderProto(CKBehaviorPrototype **);
int MovieLoader(const CKBehaviorContext &behcontext);
unsigned int ThreadFunctionTest(void *arg);
CKERROR MovieLoaderCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMovieLoaderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Movie Load");
    od->SetDescription("Loads a movie file into a texture or a Sprite.");
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x798e26a9, 0x34e4169e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMovieLoaderProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateMovieLoaderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Movie Load");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Loaded");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Movie File", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(MovieLoader);

    *pproto = proto;
    return CK_OK;
}

int MovieLoader(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
    {
        beh->ActivateOutput(1);
        return CKBR_PARAMETERERROR;
    }

    beh->ActivateInput(0, FALSE);

    XString fname((CKSTRING)beh->GetInputParameterReadDataPtr(0));
    ctx->GetPathManager()->ResolveFileName(fname, BITMAP_PATH_IDX);

    // Loading
    CKERROR res = CKERR_INVALIDOBJECT;
    if (tex)
    {
        if (!tex->LoadMovie(fname.Str()))
            res = CKERR_INVALIDFILE;
        else
            res = CK_OK;
    }

    if (res != CK_OK)
    {
        beh->ActivateOutput(1);
    }
    else
    {
        beh->SetOutputParameterObject(0, tex);
        beh->ActivateOutput(0);
    }
    return CKBR_OK;
}
