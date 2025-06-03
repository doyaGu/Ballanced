/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Object Between ?
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_INTERSECTIONPRECISIONTYPE CKDEFINEGUID(0x6cf55733, 0x5af72dae)

CKObjectDeclaration *FillBehaviorObjectBetweenDecl();
CKERROR CreateObjectBetweenProto(CKBehaviorPrototype **);
int ObjectBetween(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorObjectBetweenDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Between");
    od->SetDescription("Tests if there is an object between the two given entities");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Yes: </SPAN>is activated if an obstacle is between the two given objects.<BR>
    <SPAN CLASS=out>No: </SPAN>is activated if there is no obstacle between the two objects.<BR>
    <BR>
    <SPAN CLASS=pin>Object 1: </SPAN>the first object of the test.<BR>
    <SPAN CLASS=pin>Object 2: </SPAN>the second object of the test.<BR>
    <SPAN CLASS=pin>Precision: </SPAN>level of precision of the intersection detection. whether 'Bounding Box' (tests only the intersection between bounding box) or 'Face' (test intersection with all the faces of obstacles)<BR>
    <SPAN CLASS=pin>Test Only Obstacles: </SPAN>if this boolean is set to TRUE, only obstacles registred in the Collision Manager will be taken into account during the test.<BR>
    <BR>
    <SPAN CLASS=pout>Obstacle: </SPAN> the nearest object from object #1 that is between object #1 and #2.<BR>
    */
    od->SetCategory("Logics/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45484b53, 0x8eb1fab));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectBetweenProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObjectBetweenProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Between");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Object 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Object 2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Intersection Precision Level", CKPGUID_INTERSECTIONPRECISIONTYPE, "Face");
    proto->DeclareInParameter("Test Only Obstacles", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Obstacle", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ObjectBetween);

    *pproto = proto;
    return CK_OK;
}

CKBOOL RayBoxIntersection(VxBbox &box, VxVector &p0, VxVector &p1)
{
    CKBOOL inside = TRUE;
    int quadrant[3];
    int i;
    int whichPlane;
    VxVector res, norm(0.0f), maxT;
    VxVector candidatePlane(0.0f);
    VxVector dir = p1 - p0;
    // Find candidate planes; this loop can be avoided if
    // rays cast all from the eye(assume perpsective view)
    for (i = 0; i < 3; i++)
        if (p0.v[i] < box.Min.v[i])
        {
            quadrant[i] = 1;
            candidatePlane.v[i] = box.Min.v[i];
            norm.v[i] = -1.0f;
            inside = FALSE;
        }
        else if (p0.v[i] > box.Max.v[i])
        {
            quadrant[i] = 0;
            candidatePlane.v[i] = box.Max.v[i];
            norm.v[i] = 1.0f;
            inside = FALSE;
        }
        else
            quadrant[i] = 2;

    // Ray p0.v inside bounding box
    if (inside)
    {
        res = p0;
        return (TRUE);
    }

    // Calculate T distances to candidate planes
    for (i = 0; i < 3; i++)
        if (quadrant[i] != 2 && dir[i] != 0.)
            maxT.v[i] = (candidatePlane.v[i] - p0.v[i]) / dir[i];
        else
            maxT.v[i] = -1.;

    // Get largest of the maxT.v's for final choice of intersection
    whichPlane = 0;
    for (i = 1; i < 3; i++)
        if (maxT.v[whichPlane] < maxT.v[i])
            whichPlane = i;

    // Check final candidate actually inside box
    if (maxT.v[whichPlane] < 0.)
        return (FALSE);
    if (maxT.v[whichPlane] > 1.)
        return (FALSE);
    for (i = 0; i < 3; i++)
        if (whichPlane != i)
        {
            norm.v[i] = 0.0f;
            res.v[i] = p0.v[i] + maxT.v[whichPlane] * dir[i];
            if (res.v[i] < box.Min.v[i] || res.v[i] > box.Max.v[i])
                return (FALSE);
        }
        else
        {
            res.v[i] = candidatePlane.v[i];
        }
    return (TRUE);
}

CKBOOL RayFaceIntersection(VxVector &ori, VxVector &dest, VxVector &vect1, VxVector &vect2, VxVector &vect3)
{
    VxVector Normal = CrossProduct(vect2 - vect1, vect3 - vect1);

    CKBOOL inter = FALSE;
    VxVector V = dest - ori;
    int i1, i2;
    VxVector AP = ori - vect1;
    float denom = DotProduct(V, Normal);
    if (denom == 0.0f)
        return FALSE;
    float t = -DotProduct(Normal, AP) / denom;

    if (t <= 0)
        return FALSE;
    if (t > 1)
        return FALSE;
    VxVector res = ori + t * V;

    // i1 et i2 indices correspondant aux deux coordon�es min de la normale
    float cmax = (float)fabs(Normal.x);
    i1 = 1;
    i2 = 2;
    if (cmax < fabs(Normal.y))
    {
        i1 = 0;
        i2 = 2;
        cmax = (float)fabs(Normal.y);
    }
    if (cmax < fabs(Normal.z))
    {
        i1 = 0;
        i2 = 1;
    }

    float u0, v0, u1, v1, u2, v2, beta, alpha;
    u0 = res[i1] - vect1[i1];
    v0 = res[i2] - vect1[i2];
    u1 = vect2[i1] - vect1[i1];
    v1 = vect2[i2] - vect1[i2];
    u2 = vect3[i1] - vect1[i1];
    v2 = vect3[i2] - vect1[i2];

    if (u1 == 0)
    {
        beta = u0 / u2;
        if ((beta >= 0.) && (beta <= 1.))
        {
            alpha = (v0 - beta * v2) / v1;
            inter = ((alpha >= 0.) && (alpha + beta) <= 1.);
        }
    }
    else
    {
        beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
        if ((beta >= 0.) && (beta <= 1.))
        {
            alpha = (u0 - beta * u2) / u1;
            inter = ((alpha >= 0) && ((alpha + beta) <= 1.));
        }
    }

    return inter;
}

int ObjectBetween(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    // Set Input State
    beh->ActivateInput(0, FALSE);

    // Get the first object
    CK3dEntity *Object1 = (CK3dEntity *)beh->GetInputParameterObject(0);

    // Get the second object
    CK3dEntity *Object2 = (CK3dEntity *)beh->GetInputParameterObject(1);

    // Get the precision
    int Precision = 2;
    beh->GetInputParameterValue(2, &Precision);

    // Test only obstacles ?
    CKBOOL OnlyObstacles = TRUE;
    beh->GetInputParameterValue(3, &OnlyObstacles);

    // Compute Ray Data
    VxVector Origin(0.0f);
    if (Object1)
        Object1->GetPosition(&Origin);

    VxVector Destination(0.0f);
    if (Object2)
        Object2->GetPosition(&Destination);

    // Local variables
    VxVector LocalOrigin, LocalDestination;

    CKMesh *mesh;
    VxBbox box;

    CKDWORD res = CKBR_OK;
    if (OnlyObstacles)
    {
        CKAttributeManager *attman = behcontext.AttributeManager;
        int Attribute = attman->GetAttributeTypeByName("Fixed Obstacle");
        const XObjectPointerArray &group = attman->GetAttributeListPtr(Attribute);

        int NbFaces;

        for (CKObject **o = group.Begin(); o != group.End(); ++o)
        {
            CK3dEntity *Obstacle = (CK3dEntity *)*o;
            box = Obstacle->GetBoundingBox();

            VxVector dir;
            dir = Destination - Origin;
            if (Obstacle != Object1 && Obstacle != Object2 && RayBoxIntersection(box, Origin, Destination))
            {
                if (Precision == 2)
                {
                    Obstacle->InverseTransform(&LocalOrigin, &Origin);
                    Obstacle->InverseTransform(&LocalDestination, &Destination);

                    mesh = Obstacle->GetCurrentMesh();
                    if (!mesh)
                        continue;
                    NbFaces = mesh->GetFaceCount();

                    int i1, i2, i3;
                    VxVector v1, v2, v3;

                    for (int j = 0; j < NbFaces; j++)
                    {
                        mesh->GetFaceVertexIndex(j, i1, i2, i3);

                        mesh->GetVertexPosition(i1, &v1);
                        mesh->GetVertexPosition(i2, &v2);
                        mesh->GetVertexPosition(i3, &v3);

                        if (RayFaceIntersection(LocalOrigin, LocalDestination, v1, v2, v3))
                        {
                            beh->SetOutputParameterObject(0, Obstacle);

                            beh->ActivateOutput(0, TRUE);
                            beh->ActivateOutput(1, FALSE);

                            return CKBR_OK;
                        }
                    }
                }
                else
                {
                    beh->SetOutputParameterObject(0, Obstacle);

                    beh->ActivateOutput(0, TRUE);
                    beh->ActivateOutput(1, FALSE);

                    return CKBR_OK;
                }
            }
        }
    }
    else
    {
        const XObjectPointerArray &ObstacleList = ctx->GetObjectListByType(CKCID_3DOBJECT, TRUE);

        for (CKObject **it = ObstacleList.Begin(); it != ObstacleList.End(); ++it)
        {
            CK3dEntity *Obstacle = (CK3dEntity *)*it;

            box = Obstacle->GetBoundingBox();

            if (Obstacle != Object1 && Obstacle != Object2 && RayBoxIntersection(box, Origin, Destination))
            {
                if (Precision == 2)
                {
                    Obstacle->InverseTransform(&LocalOrigin, &Origin);
                    Obstacle->InverseTransform(&LocalDestination, &Destination);

                    mesh = Obstacle->GetCurrentMesh();
                    if (!mesh)
                        continue;
                    int NbFaces = mesh->GetFaceCount();

                    int i1, i2, i3;
                    VxVector v1, v2, v3;

                    for (int j = 0; j < NbFaces; j++)
                    {
                        mesh->GetFaceVertexIndex(j, i1, i2, i3);

                        mesh->GetVertexPosition(i1, &v1);
                        mesh->GetVertexPosition(i2, &v2);
                        mesh->GetVertexPosition(i3, &v3);

                        if (RayFaceIntersection(LocalOrigin, LocalDestination, v1, v2, v3))
                        {
                            beh->SetOutputParameterObject(0, Obstacle);

                            beh->ActivateOutput(0, TRUE);
                            beh->ActivateOutput(1, FALSE);

                            return CKBR_OK;
                        }
                    }
                }
                else
                {
                    beh->SetOutputParameterObject(0, Obstacle);

                    beh->ActivateOutput(0, TRUE);
                    beh->ActivateOutput(1, FALSE);

                    return CKBR_OK;
                }
            }
        }
    }

    beh->ActivateOutput(0, FALSE);
    beh->ActivateOutput(1, TRUE);

    return CKBR_OK;
}
