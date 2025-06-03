/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            BlurObject
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define MAX_TRACKS 30

typedef struct
{
    int n;
    VxMatrix tab[MAX_TRACKS];
} VAR_MAT;

CKObjectDeclaration *FillBehaviorBlurObjectDecl();
CKERROR CreateBlurObjectProto(CKBehaviorPrototype **pproto);
int BlurObject(const CKBehaviorContext &behcontext);
CKERROR BlurCallBackObject(const CKBehaviorContext &behcontext);
int rendercallback(CKRenderContext *rc, CKRenderObject *ent, void *arg);

CKObjectDeclaration *FillBehaviorBlurObjectDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Motion Blur");
    od->SetDescription("Performs a Motion Blur when the 3D Entity is moving.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Number of Trails: </SPAN>number of trails that can be seen behind the 3D entity when it's moving.<BR>
    <BR>
    Motion blur is an effect you will see in photographs of scenes where objects are moving. It is mostly noticeable when the exposure is long, or if objects in the scene are moving rapidly.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x06d0b04b, 0x1d4ea106));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBlurObjectProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateBlurObjectProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Motion Blur");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Number of Trails", CKPGUID_INT, "10");

    proto->DeclareLocalParameter("Int and Array of Matrix", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("Is Bluring", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BlurObject);

    proto->SetBehaviorCallbackFct(BlurCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int BlurObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKBOOL is_bluring;
    beh->GetLocalParameterValue(1, &is_bluring);

    if (!is_bluring)
    {
        // Initialisation of the Matrix Array
        VAR_MAT var_mat;
        VxMatrix *tab = var_mat.tab;

        var_mat.n = 10;

        tab[0] = ent->GetWorldMatrix();
        for (int a = 1; a < MAX_TRACKS; a++)
        {
            memcpy(tab[a], tab[0], sizeof(VxMatrix));
        }

        beh->SetLocalParameterValue(0, &var_mat, sizeof(VAR_MAT));

        is_bluring = TRUE;
        beh->SetLocalParameterValue(1, &is_bluring);

        ent->ModifyMoveableFlags(VX_MOVEABLE_RENDERLAST, 0);

        ent->SetRenderCallBack(rendercallback, beh->GetLocalParameterReadDataPtr(0));
    }

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    if (is_bluring)
    {
        VAR_MAT *var_mat = (VAR_MAT *)beh->GetLocalParameterWriteDataPtr(0);
        if (beh->IsInputActive(1))
        { // We enter by 'OFF'

            beh->ActivateInput(1, FALSE);
            beh->ActivateOutput(1, TRUE);

            ent->RemoveRenderCallBack();

            is_bluring = FALSE;
            beh->SetLocalParameterValue(1, &is_bluring);

            ent->ModifyMoveableFlags(0, VX_MOVEABLE_RENDERLAST);

            return CKBR_OK;
        }

        // Nether by 'ON' nor by 'OFF'
        int n = 10;
        beh->GetInputParameterValue(0, &n);
        if (n > MAX_TRACKS)
            n = MAX_TRACKS;
        if (n < 1)
            n = 1;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();

        VxMatrix *tab = var_mat->tab;
        var_mat->n = n;
        for (int a = n - 1; a > 0; a--)
        {
            tab[a] = tab[a - 1];
        }

        tab[0] = ent->GetWorldMatrix();
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/

CKERROR BlurCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        CKBOOL is_bluring = FALSE;
        beh->SetLocalParameterValue(1, &is_bluring);
    }
    break;

    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDETACH:
    {
        CKBOOL is_bluring;
        if (beh->GetTarget())
        {
            beh->GetLocalParameterValue(1, &is_bluring);

            if (is_bluring)
            {
                is_bluring = FALSE;
                beh->SetLocalParameterValue(1, &is_bluring);
                CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
                if (!ent)
                    return 0;
                ent->RemoveRenderCallBack();

                ent->ModifyMoveableFlags(0, VX_MOVEABLE_RENDERLAST);
            }
        }
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
        CKScene *scene = behcontext.CurrentScene;
        if (!(beh->IsParentScriptActiveInScene(scene)))
        {
            CKBOOL is_bluring;
            beh->GetLocalParameterValue(1, &is_bluring);

            if (is_bluring)
            {
                is_bluring = FALSE;
                beh->SetLocalParameterValue(1, &is_bluring);
                CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
                if (!ent)
                    return 0;
                ent->RemoveRenderCallBack();
            }
        }
    }
    break;
    }
    return CKBR_OK;
}

/*******************************************************/
/****************** RENDER CALLBACK ********************/
int rendercallback(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{

    CK3dEntity *ent = (CK3dEntity *)obj;

    VAR_MAT *var_mat = (VAR_MAT *)arg;

    int n = var_mat->n;
    VxMatrix *tab = var_mat->tab;

    CKMesh *mesh = ent->GetCurrentMesh();

    // render motion trails only if the last trail is not
    // at the same position/orientation as the first one (the object itself)
    if (memcmp(tab[0], tab[n - 1], sizeof(VxMatrix)) != 0)
    {

        CKMesh *mesh = ent->GetCurrentMesh();
        if (!mesh)
            return 0;

        CKMaterial *currentMaterial;
        VxColor col;
        int a, b;

        // structure used to store all information
        // about the materials of the object
        typedef struct
        {
            CKMaterial *mat;
            VXBLEND_MODE srcmode, destmode;
            float alpha;
            CKBOOL alphaBlendWasEnabled;
            CKBOOL ZWriteWasEnabled;
        } StockedMaterialInfo;

        // stock materials info
        int matCount = mesh->GetMaterialCount();
        StockedMaterialInfo *smi = new StockedMaterialInfo[matCount];;

        for (a = 0; a < matCount; ++a)
        {
            smi[a].mat = mesh->GetMaterial(a);
            if (currentMaterial = smi[a].mat)
            {
                smi[a].srcmode = currentMaterial->GetSourceBlend();
                smi[a].destmode = currentMaterial->GetDestBlend();
                smi[a].alphaBlendWasEnabled = currentMaterial->AlphaBlendEnabled();
                smi[a].ZWriteWasEnabled = currentMaterial->ZWriteEnabled();
                smi[a].alpha = currentMaterial->GetDiffuse().a;

                // modify material properties just after stocking it
                currentMaterial->SetSourceBlend(VXBLEND_SRCALPHA);
                currentMaterial->SetDestBlend(VXBLEND_INVSRCALPHA);
                currentMaterial->EnableAlphaBlend();
                // currentMaterial->EnableZWrite( FALSE );
            }
        }

        // we save the old transfo matrix
        VxMatrix oldwtm = rc->GetWorldTransformationMatrix();

        // to know where to get the most opaque rendering
        int mid = (int)(n / 3.0f);

        // render first third
        for (a = mid; a < n; a++)
        {

            // put transparence to all materials of the mesh
            for (b = 0; b < matCount; ++b)
            {
                if (currentMaterial = smi[b].mat)
                {

                    col = currentMaterial->GetDiffuse();
                    col.a = (float)(n - a) / (n - mid) * smi[b].alpha;
                    currentMaterial->SetDiffuse(col);
                }
            }

            // render the mesh in a different position
            rc->SetWorldTransformationMatrix(tab[a]);
            mesh->Render(rc, ent);
        }

        // render the last third
        for (a = mid - 1; a >= 0; a--)
        {
            // put transparence to all materials of the mesh
            for (b = 0; b < matCount; ++b)
            {
                if (currentMaterial = smi[b].mat)
                {

                    col = currentMaterial->GetDiffuse();
                    col.a = (float)(a + 1) / (mid + 1) * smi[b].alpha;
                    currentMaterial->SetDiffuse(col);
                }
            }

            // render the mesh in a different position
            rc->SetWorldTransformationMatrix(tab[a]);
            mesh->Render(rc, ent);
        }

        // we restore the transfo mat
        rc->SetWorldTransformationMatrix(oldwtm);

        // we restore the material as they were before bluring
        for (a = 0; a < matCount; ++a)
        {
            if (currentMaterial = smi[a].mat)
            {
                currentMaterial->SetSourceBlend(smi[a].srcmode);
                currentMaterial->SetDestBlend(smi[a].destmode);
                currentMaterial->EnableAlphaBlend(smi[a].alphaBlendWasEnabled);
                currentMaterial->EnableZWrite(smi[a].ZWriteWasEnabled);

                col = currentMaterial->GetDiffuse();
                col.a = smi[a].alpha;
                currentMaterial->SetDiffuse(col);
            }
        }

        delete[] smi;
    }
    else
    {
        // just render normally the mesh without its trails if
        // the last one is at the same position/orientation as the object itself

        mesh->Render(rc, ent);
    }

    return 1;
}
