/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Delete Collision Surfaces
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "PhysicsManager.h"

CKObjectDeclaration *FillBehaviorDeleteCollisionSurfacesDecl();
CKERROR CreateDeleteCollisionSurfacesProto(CKBehaviorPrototype **);
int DeleteCollisionSurfaces(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDeleteCollisionSurfacesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Delete Collision Surfaces");
    od->SetDescription("Delete Collision Behaviors");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53BF75AA, 0x770C7021));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeleteCollisionSurfacesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDeleteCollisionSurfacesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Delete Collision Surfaces");
    if (!proto)
        return CKERR_OUTOFMEMORY;

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

    CKPhysicsManager *man = CKPhysicsManager::GetManager(context);
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