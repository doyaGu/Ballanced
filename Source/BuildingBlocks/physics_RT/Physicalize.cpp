/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Physicalize
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "CKIpionManager.h"

#include <string.h>

CKObjectDeclaration *FillBehaviorPhysicalizeDecl();
CKERROR CreatePhysicalizeProto(CKBehaviorPrototype **);
int Physicalize(const CKBehaviorContext &behcontext);
CKERROR PhysicalizeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicalizeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physicalize");
    od->SetDescription("Physicalizes ... ");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7522370E, 0x37EC15EC));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
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

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_TARGETABLE));
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
    {
        return CKBR_OWNERERROR;
    }

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_Physicalize: pm==NULL.");
        return CKBR_OK;
    }

    PhysicsStruct *phyStruct = man->HasPhysics(ent);
    if (beh->IsInputActive(0)) // Physicalize
    {
        ++man->m_PhysicalizeCalls;
        beh->ActivateInput(0, FALSE);
        if (phyStruct)
        {
            beh->ActivateInput(0, TRUE);
            return CKBR_OK;
        }

        BOOL fixed;
        beh->GetInputParameterValue(FIXED, &fixed);
        float friction;
        beh->GetInputParameterValue(FRICTION, &friction);
        float elasticity;
        beh->GetInputParameterValue(ELASTICITY, &elasticity);
        float mass;
        beh->GetInputParameterValue(MASS, &mass);
        CKSTRING collisionGroup = (CKSTRING)beh->GetInputParameterReadDataPtr(COLLISION_GROUP);
        BOOL startFrozen;
        beh->GetInputParameterValue(START_FROZEN, &startFrozen);
        BOOL enableCollision;
        beh->GetInputParameterValue(ENABLE_COLLISION, &enableCollision);
        BOOL autoCalcMassCenter;
        beh->GetInputParameterValue(AUTOMATIC_CALCULATE_MASS_CENTER, &autoCalcMassCenter);
        float linearSpeedDampening;
        beh->GetInputParameterValue(LINEAR_SPEED_DAMPENING, &linearSpeedDampening);
        float rotSpeedDampening;
        beh->GetInputParameterValue(ROT_SPEED_DAMPENING, &rotSpeedDampening);
        CKSTRING collisionSurface = (CKSTRING)beh->GetInputParameterReadDataPtr(COLLISION_SURFACE);

        int convexCount;
        beh->GetLocalParameterValue(0, &convexCount);
        int ballCount;
        beh->GetLocalParameterValue(1, &ballCount);
        int concaveCount;
        beh->GetLocalParameterValue(2, &concaveCount);

        CKMesh **convexes = (convexCount > 0) ? new CKMesh *[convexCount] : NULL;
        float ballRadius = 1.0f;
        CKMesh **concaves = (concaveCount > 0) ? new CKMesh *[concaveCount] : NULL;

        int pos = CONVEX;
        for (int i = 0; i < convexCount; ++i)
        {
            convexes[i] = (CKMesh *)beh->GetInputParameterReadDataPtr(pos + i);
        }
        pos += convexCount;

        for (int j = 0; j < ballCount; ++j)
        {
            beh->GetInputParameterValue(pos + 2 * j + 1, &ballRadius);
        }
        pos += ballCount * 2;

        for (int k = 0; k < concaveCount; ++k)
        {
            concaves[k] = (CKMesh *)beh->GetInputParameterReadDataPtr(pos + k);
        }
        pos += concaveCount;

        VxVector shiftMassCenter;
        beh->GetLocalParameterValue(0, &shiftMassCenter);
        IVP_Material *material = new IVP_Material_Simple(friction, elasticity);
        man->m_Materials.add(material);
        int ret = man->CreatePhysicObjectOnParameters(ent,
                                   convexCount,
                                   convexes,
                                   ballCount,
                                   concaveCount,
                                   concaves,
                                   ballRadius,
                                   collisionSurface,
                                   &shiftMassCenter,
                                   fixed,
                                   material,
                                   mass,
                                   collisionGroup,
                                   startFrozen,
                                   enableCollision,
                                   autoCalcMassCenter,
                                   linearSpeedDampening,
                                   rotSpeedDampening);

        if (convexes)
            delete[] convexes;
        if (concaves)
            delete[] concaves;

        beh->ActivateInput(0);
        return ret;
    }
    else // Unphysicalize
    {
        ++man->m_DePhysicalizeCalls;
        beh->ActivateInput(1, FALSE);
        if (phyStruct)
        {
            delete phyStruct->m_PhysicsObject;
            man->m_PhysicStructTable.Remove(ent->GetID());
        }
        beh->ActivateInput(1);
        return CKBR_OK;
    }
}

CKERROR PhysicalizeCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *eno = (CK3dEntity *)beh->GetOwner();
    if (!eno)
        return CKBR_OWNERERROR;

    char buffer[128];
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        if (eno->GetClassID() != CKCID_3DOBJECT)
            return CKBR_OK;

        sprintf(buffer, "Convex Mesh");
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
        strcpy(buffer, mesh->GetName());
        collSurfaceParam->SetValue(buffer, strlen(buffer) + 1);
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        if (!beh)
            return CKERR_INVALIDPARAMETER;
        int inputParamCount = beh->GetInputParameterCount();
        while (inputParamCount > CONVEX)
        {
            CKParameterIn *paramIn =  beh->RemoveInputParameter(--inputParamCount);
            CKDestroyObject(paramIn);
        }

        int convexCount = 1;
        beh->GetLocalParameterValue(0, &convexCount);
        if (convexCount > 0)
            for (int i = 0; i < convexCount; ++i)
            {
                sprintf(buffer, "convex %d", i + 1);
                beh->CreateInputParameter(buffer, CKPGUID_MESH);
            }

        int ballCount = 0;
        beh->GetLocalParameterValue(1, &ballCount);
        if (ballCount > 0)
            for (int j = 0; j < ballCount; ++j)
            {
                sprintf(buffer, "ball position %d", j + 1);
                beh->CreateInputParameter(buffer, CKPGUID_VECTOR);
                sprintf(buffer, "ball radius %d", j + 1);
                beh->CreateInputParameter(buffer, CKPGUID_FLOAT);
            }

        int concaveCount = 0;
        beh->GetLocalParameterValue(2, &concaveCount);
        if (concaveCount > 0)
            for (int k = 0; k < concaveCount; ++k)
            {
                sprintf(buffer, "concave %d", k + 1);
                beh->CreateInputParameter(buffer, CKPGUID_MESH);
            }
    }
    break;
    default:
        break;
    }

    return CKBR_OK;
}
