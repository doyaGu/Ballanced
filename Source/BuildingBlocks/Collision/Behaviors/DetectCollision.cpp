/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Collision Detection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define IMPACTFLAGS CKGUID(0x507f4ca4, 0x548e153c)

CKObjectDeclaration *FillBehaviorDetectCollisionDecl();
CKERROR CreateDetectCollisionProto(CKBehaviorPrototype **);
int DetectCollision(const CKBehaviorContext &behcontext);
CKERROR DetectCollisionCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDetectCollisionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Collision Detection");
    od->SetDescription("Detects the collision between the 3D object and other 3D object declared as obstacles.");
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
    od->SetGuid(CKGUID(0x56204f19, 0x682801e8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDetectCollisionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateDetectCollisionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Collision Detection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Geometry Precision", CKPGUID_OBSTACLEPRECISIONBEH, "Automatic");
    proto->DeclareInParameter("Detection Tests", CKPGUID_INT, "0");
    proto->DeclareInParameter("Safe Position Tests", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Object Part", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Touched Obstacle", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Touched Sub Obstacle", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Touched Face", CKPGUID_INT);
    proto->DeclareOutParameter("Touching Face", CKPGUID_INT);
    proto->DeclareOutParameter("Touched Vertex", CKPGUID_INT);
    proto->DeclareOutParameter("Touching Vertex", CKPGUID_INT);
    proto->DeclareOutParameter("Impact Point", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Impact Normal", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Impact Orientation", CKPGUID_MATRIX);

    proto->DeclareSetting("Impact Data", IMPACTFLAGS, "Object Part,Touched Obstacle,Touched Sub Obstacle,Touched Face,Touching Face,Touched Vertex,Touching Vertex,Impact World Matrix,Impact Point,Impact Normal");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DetectCollision);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(DetectCollisionCallback);

    *pproto = proto;
    return CK_OK;
}

int DetectCollision(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set Input state
    beh->ActivateInput(0, FALSE);

    // we get the owner
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        beh->ActivateOutput(1);
        return CKBR_OWNERERROR;
    }

    // Get the precision of detection
    CK_GEOMETRICPRECISION precis_level = CKCOLLISION_NONE;
    beh->GetInputParameterValue(0, &precis_level);

    // Get the precision of detection
    int detprecision = 0;
    beh->GetInputParameterValue(1, &detprecision);

    // Get the precision of detection
    int repprecision = 0;
    beh->GetInputParameterValue(2, &repprecision);

    // we get the current collision manager
    CKContext *ctx = behcontext.Context;
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);

    CK_IMPACTINFO flags;
    beh->GetLocalParameterValue(0, &flags);

    ImpactDesc imp;
    CKBOOL Collision = cm->DetectCollision(ent, precis_level, repprecision, detprecision, flags, &imp);

    if (Collision)
    {
        if (flags & OWNERENTITY)
            beh->SetOutputParameterValue(0, &(imp.m_OwnerEntity));
        if (flags & OBSTACLETOUCHED)
            beh->SetOutputParameterValue(1, &(imp.m_ObstacleTouched));
        if (flags & SUBOBSTACLETOUCHED)
            beh->SetOutputParameterValue(2, &(imp.m_SubObstacleTouched));
        if (flags & TOUCHEDFACE)
            beh->SetOutputParameterValue(3, &(imp.m_TouchedFace));
        if (flags & TOUCHINGFACE)
            beh->SetOutputParameterValue(4, &(imp.m_TouchingFace));
        if (flags & TOUCHEDVERTEX)
            beh->SetOutputParameterValue(5, &(imp.m_TouchedVertex));
        if (flags & TOUCHINGVERTEX)
            beh->SetOutputParameterValue(6, &(imp.m_TouchingVertex));
        if (flags & IMPACTPOINT)
            beh->SetOutputParameterValue(7, &(imp.m_ImpactPoint));
        if (flags & IMPACTNORMAL)
            beh->SetOutputParameterValue(8, &(imp.m_ImpactNormal));
        if (flags & IMPACTWORLDMATRIX)
            beh->SetOutputParameterValue(9, &(imp.m_ImpactWorldMatrix));

        // For visual debug output of the intersection point...
        //		behcontext.Context->GetRenderManager()->RegisterPoint(imp.m_ImpactPoint,0xFFFF0000);
        //		behcontext.Context->GetRenderManager()->RegisterNormal(VxRay(imp.m_ImpactPoint,imp.m_ImpactNormal,NULL),0xFF0000FF);

        beh->ActivateOutput(0);
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

CKERROR DetectCollisionCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKDWORD flags;
        beh->GetLocalParameterValue(0, &flags);

        beh->EnableOutputParameter(0, flags & OWNERENTITY);
        beh->EnableOutputParameter(1, flags & OBSTACLETOUCHED);
        beh->EnableOutputParameter(2, flags & SUBOBSTACLETOUCHED);
        beh->EnableOutputParameter(3, flags & TOUCHEDFACE);
        beh->EnableOutputParameter(4, flags & TOUCHINGFACE);
        beh->EnableOutputParameter(5, flags & TOUCHEDVERTEX);
        beh->EnableOutputParameter(6, flags & TOUCHINGVERTEX);
        beh->EnableOutputParameter(7, flags & IMPACTPOINT);
        beh->EnableOutputParameter(8, flags & IMPACTNORMAL);
        beh->EnableOutputParameter(9, flags & IMPACTWORLDMATRIX);
    }
    break;
    }
    return CKBR_OK;
}