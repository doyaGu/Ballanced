/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Collision Detection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CollisionManager.h"

#define IMPACTFLAGS CKGUID(0x507f4ca4, 0x548e153c)

CKObjectDeclaration *FillBehaviorMultiDetectCollisionDecl();
CKERROR CreateMultiDetectCollisionProto(CKBehaviorPrototype **);
int MultiDetectCollision(const CKBehaviorContext &behcontext);
int MultiDetectCollision2(const CKBehaviorContext &behcontext);
CKERROR MultiDetectCollisionCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMultiDetectCollisionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Multi Collision Detection");
    od->SetDescription("Detects the collision between any 3D objects declared as obstacles.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if an obstacle touched the object.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if no obstacle touched the object.<BR>
    <BR>
    <SPAN CLASS=pin>Geometry Precision: </SPAN>you can force here a geometry precision for all the obstacles or use the attributes' specified precision with the 'Automatic' value.<BR>
    <SPAN CLASS=pin>Detection Tests: </SPAN>the maximum number of tests to be performed in addition to the initial one, to determine if a collision occured. The building block stops testing at the first collision it encounters, then tests for a safe position from that point.<BR>
    <SPAN CLASS=pin>Safe Position Tests: </SPAN>the maximum number of tests to be performed in addition to the initial one, to determine the nearest safe position from the collision point.<BR>
    <BR>
    <SPAN CLASS=pout>Object Part: </SPAN>if the object which building block is applied to, is composed of many others, 'Object Part' retrieves the collided sub object. If there's no sub object, 'Object Part' retrieves simply this object.<BR>
    <SPAN CLASS=pout>Touched Obstacle: </SPAN>3D Entity touched by the object (retrieves the parent of the obstacle if you specified the collision to be hierarchical).<BR>
    <SPAN CLASS=pout>Touched Sub Obstacle: </SPAN>3D Entity touched by the object (retrieves the exact object on a hierarchical collision).<BR>
    <SPAN CLASS=pout>Touched Vertex: </SPAN>nearest Vertex of the obstacle when the collision occurs.<BR>
    <SPAN CLASS=pout>Touched Face: </SPAN>nearest Face of the obstacle when the collision occurs.<BR>
    <SPAN CLASS=pout>Touching Vertex: </SPAN>nearest Vertex of the tested object when the collision occurs.<BR>
    <SPAN CLASS=pout>Touching Face: </SPAN>nearest Face of the tested object when the collision occurs.<BR>
    <SPAN CLASS=pout>Impact Point: </SPAN>the approximate impact point (based on the two nearest vertices of the two objects).<BR>
    <SPAN CLASS=pout>Impact Normal: </SPAN>the approximate impact normal based on the two nearest vertices of the two objects, not on the face normal of the obstacle).<BR>
    <SPAN CLASS=pout>Impact Orientation: </SPAN>the world matrix of the tested object before the collision occurs (the matrix of the previous frame if it can't find anything better.)<BR>
    <BR>
    <SPAN CLASS=setting>Impact Data: </SPAN>With these flags, you can choose which information is relevant to you.<BR>
    */
    /* warning:
    - This building block tells at a given time if the object is touching or not an obstacle.<BR>
    If you need to execute this test every 5 behavioral process, you should then loop this building block with a 5 frame link delay.<BR>
    If you need to execute this test at each rendering process, you should then loop this building block with a 1 frame link delay.<BR>
    - Each obstacles to be assigned the "Fixed Obstacle" or "Moving Obstacle" attribute, knowing that only the objects that doesn't move from one frame to another can be safely assigned the "Fixed Obstacle" attribute.<BR>
    */
    od->SetCategory("Collisions/3D Entity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56204f09, 0x611101e8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateMultiDetectCollisionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateMultiDetectCollisionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Multi Collision Detection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("All Collisions parsed");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Geometry Precision", CKPGUID_OBSTACLEPRECISIONBEH, "Automatic");

    proto->DeclareOutParameter("Index", CKPGUID_INT, "0"); // Index
    proto->DeclareOutParameter("Object 1", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Object 2", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Object 1 Part", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Object 2 Part", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Object 1 Face", CKPGUID_INT);
    proto->DeclareOutParameter("Object 2 Face", CKPGUID_INT);
    proto->DeclareOutParameter("Object 1 Vertex", CKPGUID_INT);
    proto->DeclareOutParameter("Object 2 Vertex", CKPGUID_INT);
    proto->DeclareOutParameter("Impact Point", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Impact Normal", CKPGUID_VECTOR);

    proto->DeclareSetting("Impact Data", IMPACTFLAGS);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MultiDetectCollision2);
    proto->SetBehaviorCallbackFct(MultiDetectCollisionCallback);

    *pproto = proto;
    return CK_OK;
}

int MultiDetectCollision(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the current collision manager
    CKContext *ctx = behcontext.Context;
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);

    int index = 0;
    beh->GetOutputParameterValue(0, &index);

    CK_IMPACTINFO flags;
    beh->GetLocalParameterValue(0, &flags);

    if (beh->IsInputActive(0))
    { // enter by in
        beh->ActivateInput(0, FALSE);

        index = 0;

        // Get the precision of detection
        CK_GEOMETRICPRECISION precis_level = CKCOLLISION_NONE;
        beh->GetInputParameterValue(0, &precis_level);

        // The collision call
        ((CollisionManager *)cm)->DetectCollision(precis_level, flags);
    }
    else
    { // loop In
        beh->ActivateInput(1, FALSE);

        ++index;
    }

    const XArray<ImpactDesc> &array = ((CollisionManager *)cm)->GetImpactArray();

    if (index >= array.Size())
    { // no more impacts in group
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    // Filling the values
    const ImpactDesc &imp = array[index];

    beh->SetOutputParameterValue(1, &(imp.m_Entity));
    beh->SetOutputParameterValue(2, &(imp.m_ObstacleTouched));
    if (flags & OWNERENTITY)
        beh->SetOutputParameterValue(3, &(imp.m_OwnerEntity));
    if (flags & SUBOBSTACLETOUCHED)
        beh->SetOutputParameterValue(4, &(imp.m_SubObstacleTouched));
    // Fix by roro => Object1 is "Touching" object while object 2 is "Touched" object in collision manager
    // syntax
    if (flags & TOUCHEDFACE)
        beh->SetOutputParameterValue(5, &(imp.m_TouchingFace));
    if (flags & TOUCHINGFACE)
        beh->SetOutputParameterValue(6, &(imp.m_TouchedFace));
    if (flags & TOUCHEDVERTEX)
        beh->SetOutputParameterValue(7, &(imp.m_TouchingVertex));
    if (flags & TOUCHINGVERTEX)
        beh->SetOutputParameterValue(8, &(imp.m_TouchedVertex));
    //  End fix
    if (flags & IMPACTPOINT)
        beh->SetOutputParameterValue(9, &(imp.m_ImpactPoint));
    if (flags & IMPACTNORMAL)
        beh->SetOutputParameterValue(10, &(imp.m_ImpactNormal));

    // For visual debug output of the intersection point...
    //	behcontext.Context->GetRenderManager()->RegisterPoint(imp.m_ImpactPoint,0xFFFF0000);
    //	behcontext.Context->GetRenderManager()->RegisterNormal(VxRay(imp.m_ImpactPoint,imp.m_ImpactNormal,NULL),0xFF0000FF);

    // update the index
    beh->SetOutputParameterValue(0, &index);

    beh->ActivateOutput(1);
    return CKBR_OK;
}

CKBOOL FillOutputs(CKBehavior *beh, CK_IMPACTINFO flags, CollisionManager *cm, int index)
{

    const XArray<ImpactDesc> &array = ((CollisionManager *)cm)->GetImpactArray();
    int size = array.Size();
    if (size && index < size)
    {
        // Filling the values
        const ImpactDesc &imp = array[index];

        beh->SetOutputParameterValue(1, &(imp.m_Entity));
        beh->SetOutputParameterValue(2, &(imp.m_ObstacleTouched));
        if (flags & OWNERENTITY)
            beh->SetOutputParameterValue(3, &(imp.m_OwnerEntity));
        if (flags & SUBOBSTACLETOUCHED)
            beh->SetOutputParameterValue(4, &(imp.m_SubObstacleTouched));
        // Fix by roro => Object1 is "Touching" object while object 2 is "Touched" object in collision manager
        // syntax
        if (flags & TOUCHEDFACE)
            beh->SetOutputParameterValue(5, &(imp.m_TouchingFace));
        if (flags & TOUCHINGFACE)
            beh->SetOutputParameterValue(6, &(imp.m_TouchedFace));
        if (flags & TOUCHEDVERTEX)
            beh->SetOutputParameterValue(7, &(imp.m_TouchingVertex));
        if (flags & TOUCHINGVERTEX)
            beh->SetOutputParameterValue(8, &(imp.m_TouchedVertex));
        if (flags & IMPACTPOINT)
            beh->SetOutputParameterValue(9, &(imp.m_ImpactPoint));
        if (flags & IMPACTNORMAL)
            beh->SetOutputParameterValue(10, &(imp.m_ImpactNormal));

        // For visual debug output of the intersection point...
        //		beh->GetCKContext()->GetRenderManager()->RegisterPoint(imp.m_ImpactPoint,0xFFFFFFFF);
        //		beh->GetCKContext()->GetRenderManager()->RegisterNormal(VxRay(imp.m_ImpactPoint,imp.m_ImpactNormal,NULL),0xFF00FF00);

        // update the index
        beh->SetOutputParameterValue(0, &index);
        return CK_OK;
    }
    return CKERR_INVALIDPARAMETER;
}

int MultiDetectCollision2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the current collision manager
    CKContext *ctx = behcontext.Context;
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);

    int index = 0;
    beh->GetOutputParameterValue(0, &index);

    CK_IMPACTINFO flags;
    beh->GetLocalParameterValue(0, &flags);

    if (beh->IsInputActive(0))
    { // enter by On
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    if (beh->IsInputActive(1))
    {
        // Off
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(2))
    {
        // Loopin fill the outputs with next collision data
        beh->ActivateInput(2, FALSE);
        index++;
        if (FillOutputs(beh, flags, (CollisionManager *)cm, index))
        {
            // Output set failed: invalid index or no  collision
            beh->ActivateOutput(2, TRUE);
            return CKBR_ACTIVATENEXTFRAME;
        }
        else
            beh->ActivateOutput(3, TRUE);
    }
    else
    {
        index = 0;
    }

    if (!index)
    {
        // Get the precision of detection
        CK_GEOMETRICPRECISION precis_level = CKCOLLISION_NONE;
        beh->GetInputParameterValue(0, &precis_level);
        // The collision call if we are not already processing previous collisions
        ((CollisionManager *)cm)->DetectCollision(precis_level, flags);
        const XArray<ImpactDesc> &array = ((CollisionManager *)cm)->GetImpactArray();
        if (array.Size())
        {
            FillOutputs(beh, flags, (CollisionManager *)cm, index);
            beh->ActivateOutput(3, TRUE);
        }
        else
            return CKBR_ACTIVATENEXTFRAME;
    }

    return CKBR_OK;
}

CKERROR MultiDetectCollisionCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
        if (beh->GetVersion() == 0x00010000)
            beh->SetFunction(MultiDetectCollision);
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKDWORD flags;
        beh->GetLocalParameterValue(0, &flags);

        beh->EnableOutputParameter(3, flags & OWNERENTITY);
        beh->EnableOutputParameter(4, flags & SUBOBSTACLETOUCHED);
        beh->EnableOutputParameter(5, flags & TOUCHEDFACE);
        beh->EnableOutputParameter(6, flags & TOUCHINGFACE);
        beh->EnableOutputParameter(7, flags & TOUCHEDVERTEX);
        beh->EnableOutputParameter(8, flags & TOUCHINGVERTEX);
        beh->EnableOutputParameter(9, flags & IMPACTPOINT);
        beh->EnableOutputParameter(10, flags & IMPACTNORMAL);
    }
    break;
    }
    return CKBR_OK;
}
