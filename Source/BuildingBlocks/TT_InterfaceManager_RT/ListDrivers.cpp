/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT List Drivers
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorListDriversDecl();
CKERROR CreateListDriversProto(CKBehaviorPrototype **);
int ListDrivers(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListDriversDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT List Driver");
    od->SetDescription("List Drivers");
    od->SetCategory("TT InterfaceManager/Display");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x62D00456, 0x30EB4245));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateListDriversProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateListDriversProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT List Driver");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Driver List", CKPGUID_DATAARRAY);

    proto->DeclareOutParameter("Installed Driver", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ListDrivers);

    *pproto = proto;
    return CK_OK;
}

int ListDrivers(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKDataArray *drivers = (CKDataArray *)beh->GetInputParameterObject(0);
    if (!drivers)
    {
        context->OutputToConsoleExBeep("ListDrivers: No DataArray Object is found.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    drivers->GetColumnCount();
    drivers->Clear();
    while (drivers->GetColumnCount() > 0)
        drivers->RemoveColumn(0);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("ListDrivers.cpp", "int ListDrivers(...)", " im == NULL");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    drivers->InsertColumn(-1, CKARRAYTYPE_STRING, "DriverDesc");
    drivers->InsertColumn(0, CKARRAYTYPE_STRING, "DriverName");
    drivers->InsertColumn(1, CKARRAYTYPE_INT, "DriverID");

    int count = context->GetRenderManager()->GetRenderContextCount();
    if (count > 0)
    {
            for (int i = 0; i < count; ++i)
            {
                VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(i);
                if (drDesc->IsHardware)
                {
                    drivers->InsertRow();
                    drivers->SetElementStringValue(i, 0, drDesc->DriverName);
                    drivers->SetElementValue(i, 1, &i, sizeof(int));
                    drivers->SetElementStringValue(i, 2, drDesc->DriverDesc);
                }
            }
    }

    int driver = man->GetDriver();
    beh->SetOutputParameterValue(0, &driver, sizeof(int));
    beh->ActivateOutput(0);
    return CKBR_OK;
}