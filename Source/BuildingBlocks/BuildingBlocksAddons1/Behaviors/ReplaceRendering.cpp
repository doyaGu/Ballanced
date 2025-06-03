/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              ReplaceRendering
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorReplaceRenderingDecl();
CKERROR CreateReplaceRenderingProto(CKBehaviorPrototype **);
int ReplaceRendering(const CKBehaviorContext &behcontext);

/**********************************************/
/*				DECLARATION
/**********************************************/
CKObjectDeclaration *FillBehaviorReplaceRenderingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Replace Rendering");
    od->SetDescription("Replace the rendering of a mesh's material just before it's rendering.");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x208f0030, 0x652f088e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReplaceRenderingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

/**********************************************/
/*				PROTO
/**********************************************/
CKERROR CreateReplaceRenderingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Replace Rendering");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("In Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Out Material", CKPGUID_MATERIAL);

    proto->DeclareSetting("Per Object", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReplaceRendering);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

/**********************************************/
/*				SUB-MESH RENDER CALLBACK
/**********************************************/
void ReplaceMaterialPreRender(CKRenderContext *rc, CK3dEntity *Mov, CKMesh *Object, CKMaterial *mat, void *arg)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return;

    CKMaterial *inMat = (CKMaterial *)beh->GetInputParameterObject(0);
    CKMaterial *outMat = (CKMaterial *)beh->GetInputParameterObject(1);
    if (mat == inMat)
    {
        rc->SetCurrentMaterial(outMat);
    }
    else
    {
        rc->SetCurrentMaterial(mat);
    }
}

int PreReplaceMaterialRC(CKRenderContext *rc, CKRenderObject *iEnt, void *arg)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return 0;

    CK3dEntity *ent = (CK3dEntity *)iEnt;
    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return 1;

    CKMaterial *inMat = (CKMaterial *)beh->GetInputParameterObject(0);
    CKMaterial *outMat = (CKMaterial *)beh->GetInputParameterObject(1);

    mesh->ReplaceMaterial(inMat, outMat);

    return 1;
}

int PostReplaceMaterialRC(CKRenderContext *rc, CKRenderObject *iEnt, void *arg)
{

    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return 0;

    CK3dEntity *ent = (CK3dEntity *)iEnt;
    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return 1;

    CKMaterial *inMat = (CKMaterial *)beh->GetInputParameterObject(0);
    CKMaterial *outMat = (CKMaterial *)beh->GetInputParameterObject(1);

    mesh->ReplaceMaterial(outMat, inMat);

    return 1;
}

void PreReplaceMaterialRCMesh(CKRenderContext *Dev, CK3dEntity *Mov, CKMesh *Object, void *Argument)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(Dev->GetCKContext(), (CK_ID)Argument);
    if (!beh)
        return;

    CKMaterial *inMat = (CKMaterial *)beh->GetInputParameterObject(0);
    CKMaterial *outMat = (CKMaterial *)beh->GetInputParameterObject(1);

    Object->ReplaceMaterial(inMat, outMat);
}

void PostReplaceMaterialRCMesh(CKRenderContext *Dev, CK3dEntity *Mov, CKMesh *Object, void *Argument)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(Dev->GetCKContext(), (CK_ID)Argument);
    if (!beh)
        return;

    CKMaterial *inMat = (CKMaterial *)beh->GetInputParameterObject(0);
    CKMaterial *outMat = (CKMaterial *)beh->GetInputParameterObject(1);

    Object->ReplaceMaterial(outMat, inMat);
}

/**********************************************/
/*				FUNCTION
/**********************************************/
int ReplaceRendering(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    if (beh->IsInputActive(1))
    {
        // enter by OFF
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        // enter by ON
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_ACTIVATENEXTFRAME;

    CKBOOL perObject = FALSE;
    beh->GetLocalParameterValue(0, &perObject);

    if (perObject)
    {
        ent->AddPreRenderCallBack(PreReplaceMaterialRC, (void *)beh->GetID(), TRUE);
        ent->AddPostRenderCallBack(PostReplaceMaterialRC, (void *)beh->GetID(), TRUE);
    }
    else
    {
        // every object using this mesh sould have there material replaced
        mesh->AddPreRenderCallBack(PreReplaceMaterialRCMesh, (void *)beh->GetID(), TRUE);
        mesh->AddPostRenderCallBack(PostReplaceMaterialRCMesh, (void *)beh->GetID(), TRUE);
    }

    return CKBR_ACTIVATENEXTFRAME;
}
