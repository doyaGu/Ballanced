//////////////////////////////////////
//////////////////////////////////////
//
//        TT Activate Object
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorActivateObjectDecl();
CKERROR CreateActivateObjectProto(CKBehaviorPrototype **pproto);
int ActivateObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorActivateObjectDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Activate Object");
    od->SetDescription("Activates all the scripts of an object, either in the state they were before deactivation or once reset.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x59245fae, 0x24a123f3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateActivateObjectProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateActivateObjectProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Activate Object");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareInParameter("Reset Scripts?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Activate All Scripts", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ActivateObject);

    *pproto = proto;
    return CK_OK;
}

int ActivateObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}
