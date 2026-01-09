//////////////////////////////////////
//////////////////////////////////////
//
//        TT Switch on Layer
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSwitchonLayerDecl();
CKERROR CreateSwitchonLayerProto(CKBehaviorPrototype **pproto);
int SwitchonLayer(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSwitchonLayerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Switch on Layer");
    od->SetDescription("Gets the value of a square");
    od->SetCategory("TT Toolbox/Grids");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1a151160, 0x1be26257));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSwitchonLayerProto);
    od->NeedManager(GRID_MANAGER_GUID);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSwitchonLayerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Switch on Layer");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("In");
    proto->DeclareOutput("Out");
    proto->DeclareOutput("Enter");
    proto->DeclareOutput("Exit");

    proto->DeclareInParameter("Ref", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Layer", CKPGUID_LAYERTYPE, "- default -");
    proto->DeclareInParameter("Value", CKPGUID_INT, "255");

    proto->DeclareOutParameter("Exitvector", CKPGUID_VECTOR);

    proto->DeclareLocalParameter("Last Position", CKPGUID_VECTOR);
    proto->DeclareLocalParameter("Was inside", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SwitchonLayer);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SwitchonLayer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    // Get input parameters
    int checkValue = 0;
    beh->GetInputParameterValue(2, &checkValue);

    CK3dEntity *refEntity = (CK3dEntity *)beh->GetInputParameterObject(0);
    int layer = 1;
    beh->GetInputParameterValue(1, &layer);

    // Position in the referential defined by 'Ref' (NULL => world).
    VxVector positionInRef;
    target->GetPosition(&positionInRef, refEntity);

    // Also compute world position for CKGrid APIs that expect world coordinates.
    VxVector positionWorld = positionInRef;
    if (refEntity)
        refEntity->Transform(&positionWorld, &positionInRef, NULL);

    // Handle Off input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    // Handle In input
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->SetLocalParameterValue(0, &positionWorld);
        CKBOOL wasInside = FALSE;
        beh->SetLocalParameterValue(1, &wasInside);
    }

    CKGridManager *gridManager = (CKGridManager *)ctx->GetManagerByGuid(GRID_MANAGER_GUID);
    if (!gridManager)
        return CKBR_ACTIVATENEXTFRAME;

    CKGrid *grid = gridManager->GetPreferredGrid(&positionInRef, refEntity);
    if (!grid)
    {
        beh->SetLocalParameterValue(0, &positionWorld);
        return CKBR_ACTIVATENEXTFRAME;
    }

    CKLayer *layerObj = grid->GetLayer(layer);
    if (!layerObj)
    {
        beh->SetLocalParameterValue(0, &positionWorld);
        return CKBR_ACTIVATENEXTFRAME;
    }

    int gridX = 0;
    int gridZ = 0;
    grid->Get2dCoordsFrom3dPos(&positionWorld, &gridX, &gridZ);

    // Validate coordinates against grid dimensions to avoid out-of-bounds access.
    if (gridX < 0 || gridZ < 0 || gridX >= grid->GetWidth() || gridZ >= grid->GetLength())
    {
        beh->SetLocalParameterValue(0, &positionWorld);
        return CKBR_ACTIVATENEXTFRAME;
    }

    int faceValue = 0;
    layerObj->GetValue(gridX, gridZ, &faceValue);

    // Get previous state
    CKBOOL wasInside = FALSE;
    beh->GetLocalParameterValue(1, &wasInside);

    // Check if at the target value
    if (faceValue == checkValue)
    {
        if (wasInside)
        {
            // Still inside
            beh->ActivateOutput(0, TRUE);
        }
        else
        {
            // Just entered
            wasInside = TRUE;
            beh->SetLocalParameterValue(1, &wasInside);
            beh->ActivateOutput(2, TRUE);
        }
    }
    else
    {
        if (wasInside)
        {
            // Just exited
            wasInside = FALSE;
            beh->SetLocalParameterValue(1, &wasInside);

            // Calculate exit vector
            VxVector lastPos;
            beh->GetLocalParameterValue(0, &lastPos);

            VxVector exitVector;
            exitVector.x = positionWorld.x - lastPos.x;
            exitVector.y = positionWorld.y - lastPos.y;
            exitVector.z = positionWorld.z - lastPos.z;

            beh->SetOutputParameterValue(0, &exitVector);
            beh->ActivateOutput(3, TRUE);
        }
        else
        {
            // Still outside
            beh->ActivateOutput(1, TRUE);
        }
    }

    // Store current position
    beh->SetLocalParameterValue(0, &positionWorld);

    return CKBR_ACTIVATENEXTFRAME;
}