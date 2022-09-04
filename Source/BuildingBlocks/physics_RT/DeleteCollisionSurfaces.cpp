/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Delete Collision Surfaces
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorDeleteCollisionSurfacesDecl();
CKERROR CreateDeleteCollisionSurfacesProto(CKBehaviorPrototype **pproto);
int DeleteCollisionSurfaces(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDeleteCollisionSurfacesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("DeleteCollisionSurfaces");
    od->SetDescription("Delete Collision Behaviors");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53bf75aa, 0x770c7021));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeleteCollisionSurfacesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDeleteCollisionSurfacesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("DeleteCollisionSurfaces");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeleteCollisionSurfaces);

    *pproto = proto;
    return CK_OK;
}

int DeleteCollisionSurfaces(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_DeleteCollisionSurfaces: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}