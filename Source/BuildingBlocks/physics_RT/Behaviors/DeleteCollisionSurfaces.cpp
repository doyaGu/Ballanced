/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Delete Collision Surfaces
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

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
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeleteCollisionSurfaces);

    *pproto = proto;
    return CK_OK;
}

class DeleteCollisionSurfacesCallback : public PhysicsCallback
{
public:
    DeleteCollisionSurfacesCallback(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        if (m_IpionManager->GetPhysicsObjectCount() != 0)
        {
            m_IpionManager->m_Context->OutputToConsole("Please dephysicalize all objects before calling DeleteCollisionSurfaces!!!");
            return CKBR_OK;
        }

        m_IpionManager->ClearCollisionSurfaces();

        return CKBR_ACTIVATENEXTFRAME;
    }
};

int DeleteCollisionSurfaces(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    DeleteCollisionSurfacesCallback *cb = new DeleteCollisionSurfacesCallback(man, beh);
    man->m_PreSimulateCallbacks->Process(cb);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}