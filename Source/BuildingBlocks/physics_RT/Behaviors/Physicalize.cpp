/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Physicalize
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

#include <string.h>

CKObjectDeclaration *FillBehaviorPhysicalizeDecl();
CKERROR CreatePhysicalizeProto(CKBehaviorPrototype **pproto);
int Physicalize(const CKBehaviorContext &behcontext);
CKERROR PhysicalizeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicalizeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physicalize");
    od->SetDescription("Physicalizes ... ");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7522370e, 0x37ec15ec));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicalizeProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicalizeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physicalize");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Physicalize");
    proto->DeclareInput("Unphysicalize");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Fixed ?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Friction", CKPGUID_FLOAT, "0.7");
    proto->DeclareInParameter("Elasticity", CKPGUID_FLOAT, "0.4");
    proto->DeclareInParameter("Mass", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Collision Group", CKPGUID_STRING);
    proto->DeclareInParameter("Start Frozen", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Enable Collision", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Automatic Calculate Mass Center", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Linear Speed Dampening", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Rot Speed Dampening", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Collision Surface", CKPGUID_STRING);
    proto->DeclareInParameter("Convex", CKPGUID_MESH);

    proto->DeclareSetting("Convex Count", CKPGUID_INT, "1");
    proto->DeclareSetting("Ball Count", CKPGUID_INT, "0");
    proto->DeclareSetting("Concave Count", CKPGUID_INT, "0");
    proto->DeclareSetting("Shift Mass Center", CKPGUID_VECTOR, "0,0,0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Physicalize);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(PhysicalizeCallBack);

    *pproto = proto;
    return CK_OK;
}

#define FIXED 0
#define FRICTION 1
#define ELASTICITY 2
#define MASS 3
#define COLLISION_GROUP 4
#define START_FROZEN 5
#define ENABLE_COLLISION 6
#define AUTOMATIC_CALCULATE_MASS_CENTER 7
#define LINEAR_SPEED_DAMPENING 8
#define ROT_SPEED_DAMPENING 9
#define COLLISION_SURFACE 10
#define CONVEX 11

int Physicalize(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    PhysicsObject *po = man->GetPhysicsObject(ent);

    if (beh->IsInputActive(0)) // Physicalize
    {
        ++man->m_PhysicalizeCalls;
        beh->ActivateInput(0, FALSE);
        if (po)
        {
            beh->ActivateOutput(0, TRUE);
            return CKBR_OK;
        }

        CKBOOL fixed = FALSE;
        beh->GetInputParameterValue(FIXED, &fixed);

        float friction = 0.4f;
        beh->GetInputParameterValue(FRICTION, &friction);

        float elasticity = 0.5f;
        beh->GetInputParameterValue(ELASTICITY, &elasticity);

        float mass = 1.0f;
        beh->GetInputParameterValue(MASS, &mass);

        CKSTRING collisionGroup = (CKSTRING)beh->GetInputParameterReadDataPtr(COLLISION_GROUP);

        CKBOOL startFrozen = FALSE;
        beh->GetInputParameterValue(START_FROZEN, &startFrozen);

        CKBOOL enableCollision = TRUE;
        beh->GetInputParameterValue(ENABLE_COLLISION, &enableCollision);

        CKBOOL autoCalcMassCenter = TRUE;
        beh->GetInputParameterValue(AUTOMATIC_CALCULATE_MASS_CENTER, &autoCalcMassCenter);

        float linearSpeedDampening = 0.1f;
        beh->GetInputParameterValue(LINEAR_SPEED_DAMPENING, &linearSpeedDampening);

        float rotSpeedDampening = 0.1f;
        beh->GetInputParameterValue(ROT_SPEED_DAMPENING, &rotSpeedDampening);

        CKSTRING collisionSurface = (CKSTRING)beh->GetInputParameterReadDataPtr(COLLISION_SURFACE);

        int convexCount = 1;
        beh->GetLocalParameterValue(0, &convexCount);

        int ballCount = 0;
        beh->GetLocalParameterValue(1, &ballCount);

        int concaveCount = 0;
        beh->GetLocalParameterValue(2, &concaveCount);

        int pos = CONVEX;
        CKMesh **convexMeshes = (convexCount > 0) ? new CKMesh *[convexCount] : NULL;
        CKMesh **concaveMeshes = (concaveCount > 0) ? new CKMesh *[concaveCount] : NULL;
        float ballRadius = 1.0f;

        for (int i = 0; i < convexCount; ++i)
            convexMeshes[i] = (CKMesh *)beh->GetInputParameterObject(pos + i);
        pos += convexCount;

        for (int j = 0; j < ballCount; ++j)
        {
            beh->GetInputParameterValue(pos + 2 * j + 1, &ballRadius);
        }
        pos += ballCount * 2;

        for (int k = 0; k < concaveCount; ++k)
            concaveMeshes[k] = (CKMesh *)beh->GetInputParameterObject(pos + k);
        pos += concaveCount;

        VxVector shiftMassCenter;
        beh->GetLocalParameterValue(3, &shiftMassCenter);
        VxVector *shiftMassCenterPtr = (!autoCalcMassCenter) ? &shiftMassCenter : NULL;

        IVP_Material *material = new IVP_Material_Simple(friction, elasticity);
        man->m_Materials.add(material);
        int ret = man->CreatePhysicsObjectOnParameters(ent, convexCount, convexMeshes, ballCount, concaveCount, concaveMeshes,
                                                       ballRadius, collisionSurface, shiftMassCenterPtr, fixed, material,
                                                       mass, collisionGroup, startFrozen, enableCollision,
                                                       autoCalcMassCenter, linearSpeedDampening, rotSpeedDampening);

        if (convexMeshes)
            delete[] convexMeshes;
        if (concaveMeshes)
            delete[] concaveMeshes;

        beh->ActivateOutput(0, TRUE);
        return ret;
    }
    else // Unphysicalize
    {
        ++man->m_DePhysicalizeCalls;
        beh->ActivateInput(1, FALSE);
        if (po)
        {
            po->m_RealObject->delete_silently();
            man->RemovePhysicsObject(ent);
        }
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }
}

CKERROR PhysicalizeCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKERR_INVALIDPARAMETER;

    CK3dEntity *eno = (CK3dEntity *)beh->GetOwner();
    if (!eno)
        return CKBR_OWNERERROR;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        if (eno->GetClassID() != CKCID_3DOBJECT)
            return CKBR_OK;

        CKParameterIn *convexParamIn = beh->GetInputParameter(CONVEX);
        if (!convexParamIn)
            return CKBR_OK;

        CKParameter *convexParam = convexParamIn->GetRealSource();
        if (!convexParam)
            return CKBR_OK;

        CKMesh *mesh = eno->GetCurrentMesh();
        if (!mesh)
            return CKBR_OK;

        CK_ID id = mesh->GetID();
        convexParam->SetValue(&id);

        CKParameterIn *collSurfaceParamIn = beh->GetInputParameter(COLLISION_SURFACE);
        if (!collSurfaceParamIn)
            return CKBR_OK;

        CKParameter *collSurfaceParam = collSurfaceParamIn->GetRealSource();
        if (!collSurfaceParam)
            return CKBR_OK;

        collSurfaceParam->SetValue(mesh->GetName(), strlen(mesh->GetName()) + 1);
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        char buffer[128];

        int inputParamCount = beh->GetInputParameterCount();
        while (inputParamCount > CONVEX)
        {
            CKParameterIn *paramIn = beh->RemoveInputParameter(--inputParamCount);
            CKDestroyObject(paramIn);
        }

        int convexCount = 1;
        beh->GetLocalParameterValue(0, &convexCount);
        for (int i = 0; i < convexCount; ++i)
        {
            snprintf(buffer, sizeof(buffer), "convex %d", i + 1);
            beh->CreateInputParameter(buffer, CKPGUID_MESH);
        }

        int ballCount = 0;
        beh->GetLocalParameterValue(1, &ballCount);
        for (int j = 0; j < ballCount; ++j)
        {
            snprintf(buffer, sizeof(buffer), "ball position %d", j + 1);
            beh->CreateInputParameter(buffer, CKPGUID_VECTOR);
            snprintf(buffer, sizeof(buffer), "ball radius %d", j + 1);
            beh->CreateInputParameter(buffer, CKPGUID_FLOAT);
        }

        int concaveCount = 0;
        beh->GetLocalParameterValue(2, &concaveCount);
        for (int k = 0; k < concaveCount; ++k)
        {
            snprintf(buffer, sizeof(buffer), "concave %d", k + 1);
            beh->CreateInputParameter(buffer, CKPGUID_MESH);
        }
    }
    break;
    default:
        break;
    }

    return CKBR_OK;
}
