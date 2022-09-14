/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT List ScreenModes
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorListScreenModesDecl();
CKERROR CreateListScreenModesProto(CKBehaviorPrototype **pproto);
int ListScreenModes(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListScreenModesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT List ScreenModes");
    od->SetDescription("List Screenmodes");
    od->SetCategory("TT InterfaceManager/Display");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4e7a0194, 0x40328fd));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateListScreenModesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateListScreenModesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT List ScreenModes");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Driver ID", CKPGUID_INT);
    proto->DeclareInParameter("Return: Modes", CKPGUID_DATAARRAY);

    proto->DeclareOutParameter("Installed Mode", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ListScreenModes);

    *pproto = proto;
    return CK_OK;
}

int ListScreenModes(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int driverId = 0;
    beh->GetInputParameterValue(0, &driverId);

    CKDataArray *screenModes = (CKDataArray *)beh->GetInputParameterObject(1);
    if (!screenModes)
    {
        context->OutputToConsoleExBeep("ListScreenModes: No DataArray Object is found.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    screenModes->GetColumnCount();
    screenModes->Clear();
    while (screenModes->GetColumnCount() > 0)
        screenModes->RemoveColumn(0);

    screenModes->InsertColumn(-1, CKARRAYTYPE_INT, "Bpp");
    screenModes->InsertColumn(0, CKARRAYTYPE_INT, "Mode");
    screenModes->InsertColumn(1, CKARRAYTYPE_INT, "Width");
    screenModes->InsertColumn(2, CKARRAYTYPE_INT, "Height");

    VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(driverId);
    if (!drDesc)
    {
        context->OutputToConsoleExBeep("ListScreenModes: No Driver Description for Driver-ID '%d' is found", driverId);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (drDesc->DisplayModeCount > 0)
    {
    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;

        int i;
        int maxRefreshRate = 0;
        for (i = 0; i < dmCount; ++i)
        {
            if (dm[i].RefreshRate > maxRefreshRate)
                maxRefreshRate = dm[i].RefreshRate;
            else
                break;
        }

        int j = 0;
        int width = 0, height = 0;
        for (i = 0; i < dmCount; ++i)
        {
            if ((dm[i].Width != width || dm[i].Height != height) && dm[i].Bpp > 8 && dm[i].RefreshRate == maxRefreshRate)
            {
                screenModes->InsertRow();
                screenModes->SetElementValue(j, 0, &i, sizeof(int));
                screenModes->SetElementValue(j, 1, &dm[i].Width, sizeof(int));
                screenModes->SetElementValue(j, 2, &dm[i].Height, sizeof(int));
                screenModes->SetElementValue(j, 3, &dm[i].Bpp, sizeof(int));
                ++j;

                width = dm[i].Width;
                height = dm[i].Height;
            }
        }
    }

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("ListScreenModes: im == NULL");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    int screenMode = man->GetScreenMode();
    beh->SetOutputParameterValue(0, &screenMode, sizeof(int));
    beh->ActivateOutput(0);
    return CKBR_OK;
}