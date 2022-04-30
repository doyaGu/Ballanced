/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT List ScreenModes
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorListScreenModesDecl();
CKERROR CreateListScreenModesProto(CKBehaviorPrototype **);
int ListScreenModes(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorListScreenModesDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT List ScreenModes");
	od->SetDescription("List Screenmodes");
	od->SetCategory("TT InterfaceManager/Display");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x4E7A0194, 0x40328FD));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateListScreenModesProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateListScreenModesProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT List ScreenModes");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("OK");
	proto->DeclareOutput("Error");

	proto->DeclareInParameter("Driver ID", CKPGUID_INT);
	proto->DeclareInParameter("Return: Modes", CKPGUID_DATAARRAY);

	proto->DeclareOutParameter("Install Mode", CKPGUID_INT);

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
	{
		screenModes->RemoveColumn(0);
	}

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

		int i, j, k;
		for (i = 0, j = 0; i < drDesc->DisplayModeCount; ++i)
		{
			if (dm[i].Bpp > 8)
			{
				if (i > 0)
				{
					for (k = 0;
						 dm[i].Width != dm[j].Width ||
						 dm[i].Height != dm[j].Height ||
						 dm[i].Bpp != dm[j].Bpp;
						 ++j, ++k)
					{
						if (i < k)
						{
							break;
						}
					}
				}
				screenModes->InsertRow();
				screenModes->SetElementValue(j, 0, &i, sizeof(int));
				screenModes->SetElementValue(j, 1, &dm[i].Width, sizeof(int));
				screenModes->SetElementValue(j, 2, &dm[i].Height, sizeof(int));
				screenModes->SetElementValue(j, 3, &dm[i].Bpp, sizeof(int));
				++j;
			}
		}
	}

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man)
	{
		TT_ERROR("ListScreenModes.cpp", "int ListScreenModes(...)", " im == NULL");
		beh->ActivateOutput(1);
		return CKBR_OK;
	}

	int screenModeIndex = man->GetScreenModeIndex();
	beh->SetOutputParameterValue(0, &screenModeIndex, sizeof(int));
	beh->ActivateOutput(0);
	return CKBR_OK;
}