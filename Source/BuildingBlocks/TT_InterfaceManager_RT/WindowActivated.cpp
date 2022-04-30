/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Window Activate?
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorWindowActivatedDecl();
CKERROR CreateWindowActivatedProto(CKBehaviorPrototype **);
int WindowActivated(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWindowActivatedDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Window Activate?");
	od->SetDescription("TT Window Activate?");
	od->SetCategory("TT InterfaceManager/Display");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x70936783, 0x7FEE4A3B));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateWindowActivatedProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateWindowActivatedProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Window Activate?");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Activate");
	proto->DeclareOutput("else");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(WindowActivated);

	*pproto = proto;
	return CK_OK;
}

int WindowActivated(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (man && man->IsWindowActivated())
	{
		man->SetWindowActivated(false);
		beh->ActivateOutput(0);
	}
	else
	{
		beh->ActivateOutput(1);
	}

	return CKBR_OK;
}