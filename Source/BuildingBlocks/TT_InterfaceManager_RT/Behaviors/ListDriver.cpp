/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT List Drivers
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorListDriverDecl();
CKERROR CreateListDriverProto(CKBehaviorPrototype **pproto);
int ListDriver(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListDriverDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT List Driver");
    od->SetDescription("List Drivers");
    od->SetCategory("TT InterfaceManager/Display");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x62d00456, 0x30eb4245));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateListDriverProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateListDriverProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT List Driver");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Driver List", CKPGUID_DATAARRAY);

    proto->DeclareOutParameter("Installed Driver", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ListDriver);

    *pproto = proto;
    return CK_OK;
}

int ListDriver(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKDataArray *drivers = (CKDataArray *)beh->GetInputParameterObject(0);
    if (!drivers)
    {
        context->OutputToConsoleExBeep("ListDriver: No DataArray Object is found.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    drivers->GetColumnCount();
    drivers->Clear();
    while (drivers->GetColumnCount() > 0)
        drivers->RemoveColumn(0);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("ListDriver: im == NULL");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    drivers->InsertColumn(-1, CKARRAYTYPE_STRING, "DriverDesc");
    drivers->InsertColumn(0, CKARRAYTYPE_STRING, "DriverName");
    drivers->InsertColumn(1, CKARRAYTYPE_INT, "DriverID");

    const int driverCount = context->GetRenderManager()->GetRenderDriverCount();
    for (int i = 0; i < driverCount; ++i)
    {
        VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(i);
        drivers->InsertRow();
#if CKVERSION == 0x13022002
        drivers->SetElementStringValue(i, 0, drDesc->DriverName);
        drivers->SetElementValue(i, 1, &i, sizeof(int));
        drivers->SetElementStringValue(i, 2, drDesc->DriverDesc);
#else
        drivers->SetElementStringValue(i, 0, drDesc->DriverName.Str());
        drivers->SetElementValue(i, 1, &i, sizeof(int));
        drivers->SetElementStringValue(i, 2, drDesc->DriverDesc.Str());
#endif
    }

    int driver = man->GetDriver();
    beh->SetOutputParameterValue(0, &driver, sizeof(int));
    beh->ActivateOutput(0);
    return CKBR_OK;
}