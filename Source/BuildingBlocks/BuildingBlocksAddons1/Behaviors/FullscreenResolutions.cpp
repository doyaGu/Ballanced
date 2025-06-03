/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Fullscreen Resolutions
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define IN_IN               0

#define OUT_OUT             0
#define OUT_TRUE            0
#define OUT_FALSE           1

#define IN_P_ARRAY          0
#define IN_P_RESOLUTION     0
#define IN_P_BPP            1

#define LOCAL_P_ENUMERATE   0

CKObjectDeclaration *FillBehaviorFullscreenResolutionsDecl();
CKERROR CreateFullscreenResolutionsProto(CKBehaviorPrototype **);
int FullscreenResolutions(const CKBehaviorContext &behcontext);
CKERROR FullscreenResolutionsCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFullscreenResolutionsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Fullscreen Resolutions");
    od->SetDescription("Retrieves resolutions that current driver can display in fullscreen mode");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3f6e1e2d, 0x76f5338e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFullscreenResolutionsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Optimizations/System");
    return od;
}

CKERROR CreateFullscreenResolutionsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Fullscreen Resolutions");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Resolution Array", CKPGUID_DATAARRAY);

    proto->DeclareSetting("Enumerate ?", CKPGUID_BOOL, "TRUE");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CK_BEHAVIORPROTOTYPE_NORMAL));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FullscreenResolutions);
    proto->SetBehaviorCallbackFct((CKBEHAVIORCALLBACKFCT)FullscreenResolutionsCallBack);

    *pproto = proto;
    return CK_OK;
}

CKBOOL CheckResolution(CKBehavior *beh, VxDriverDesc *desc)
{
    Vx2DVector res;
    beh->GetInputParameterValue(IN_P_RESOLUTION, &res);
    int bpp;
    beh->GetInputParameterValue(IN_P_BPP, &bpp);

#if CKVERSION == 0x13022002
    const int displayModeCount = desc->DisplayModeCount;
#else
    const int displayModeCount = desc->DisplayModes.Size();
#endif
    for (int i = 0; i < displayModeCount; ++i)
    {
        VxDisplayMode dm = desc->DisplayModes[i];
        if (dm.Width == (int)res.x && dm.Height == (int)res.y && dm.Bpp == bpp)
        {
            return TRUE;
        }
    }
    return FALSE;
}

CKBOOL CheckArrayFormat(CKDataArray *array)
{
    if (!array || array->GetColumnCount() < 2)
    {
        return FALSE;
    }

    if (array->GetColumnParameterGuid(0) != CKPGUID_2DVECTOR)
    {
        return FALSE;
    }

    if (array->GetColumnType(1) != CKARRAYTYPE_INT && array->GetColumnParameterGuid(1) != CKPGUID_INT)
    {
        return FALSE;
    }

    return TRUE;
}

void EnumerateResolutions(CKDataArray *array, VxDriverDesc *desc)
{
    Vx2DVector res;
    CKDWORD key = (CKDWORD)(&res);
    int bpp = 0;
    int row = 0;

#if CKVERSION == 0x13022002
    const int displayModeCount = desc->DisplayModeCount;
#else
    const int displayModeCount = desc->DisplayModes.Size();
#endif
    for (int i = 0; i < displayModeCount; ++i)
    {
        VxDisplayMode dm = desc->DisplayModes[i];
        if (dm.Width != (int)res.x || dm.Height != (int)res.y || dm.Bpp != bpp)
        {

            res.x = (float)dm.Width;
            res.y = (float)dm.Height;
            bpp = dm.Bpp;

            // search in the array if this row already exists
            int index = 0;
            int rowbpp = 0;
            while (index != -1)
            {
                index = array->FindRowIndex(0, CKEQUAL, key, sizeof(Vx2DVector), index);
                if (index != -1)
                {
                    array->GetElementValue(index, 1, &rowbpp);
                    if (rowbpp == bpp)
                    {
                        break;
                    }
                    index++;
                }
            }

            // if we have not found the row, add it
            if (index == -1)
            {
                array->AddRow();
                array->SetElementValue(row, 0, &res, sizeof(Vx2DVector));
                array->SetElementValue(row, 1, &bpp, sizeof(int));
                row++;
            }
        }
    }
}

int FullscreenResolutions(const CKBehaviorContext &behcontext)
{
    CKContext *ctx = behcontext.Context;
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(IN_IN, FALSE);

    CKRenderContext *rcx = ctx->GetPlayerRenderContext();
    if (!rcx)
    {
        return (CKBR_BEHAVIORERROR);
    }

    int currentdriver = rcx->GetDriverIndex();

    CKRenderManager *rman = ctx->GetRenderManager();
    if (!rman)
    {
        return (CKBR_BEHAVIORERROR);
    }

    VxDriverDesc *desc = rman->GetRenderDriverDescription(currentdriver);

    CKBOOL enumerate = TRUE;
    beh->GetLocalParameterValue(LOCAL_P_ENUMERATE, &enumerate);

    if (enumerate)
    {
        beh->ActivateOutput(OUT_OUT);

        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(IN_P_ARRAY);
        if (!CheckArrayFormat(array))
        {
            ctx->OutputToConsole("Input Array has an incorrect format");
        }
        else
        {
            array->Clear();
            EnumerateResolutions(array, desc);
        }
    }
    else
    {
        if (CheckResolution(beh, desc))
        {
            beh->ActivateOutput(OUT_TRUE);
        }
        else
        {
            beh->ActivateOutput(OUT_FALSE);
        }
    }

    return CKBR_OK;
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR FullscreenResolutionsCallBack(const CKBehaviorContext &behcontext)
{
    CKContext *ctx = behcontext.Context;
    CKBehavior *beh = behcontext.Behavior;

    CKParameterManager *pman = ctx->GetParameterManager();

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL enumerate = TRUE;
        beh->GetLocalParameterValue(LOCAL_P_ENUMERATE, &enumerate);

        if (enumerate)
        {
            if (beh->GetInputParameterCount() != 1)
            {
                beh->DeleteOutput(OUT_FALSE);
                CKBehaviorIO *out = beh->GetOutput(OUT_TRUE);
                out->SetName("Out");

                CKParameterIn *pin = beh->RemoveInputParameter(IN_P_BPP);
                ctx->DestroyObject(pin);

                pin = beh->GetInputParameter(IN_P_RESOLUTION);
                pin->SetGUID(CKPGUID_DATAARRAY, TRUE, "Resolution Array");
            }
        }
        else
        {
            if (beh->GetInputParameterCount() != 2)
            {
                CKBehaviorIO *out = beh->GetOutput(OUT_OUT);
                out->SetName("True");
                beh->AddOutput("False");

                CKParameterIn *pin = beh->GetInputParameter(IN_P_ARRAY);
                pin->SetGUID(CKPGUID_2DVECTOR, TRUE, "Resolution");
                beh->InsertInputParameter(IN_P_BPP, "Bpp", pman->ParameterGuidToType(CKPGUID_INT));
            }
        }
    }
    break;
    }

    return CKBR_OK;
}