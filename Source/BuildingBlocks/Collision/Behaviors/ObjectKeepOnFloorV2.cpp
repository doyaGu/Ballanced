/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Object Keep On Floor V2
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "FloorTools.h"

#define FACESMAXNUMBER 64

CKERROR CreateObjectKeepOnFloorV2BehaviorProto(CKBehaviorPrototype **);
int DoObjectKeepOnFloorV2Point(const CKBehaviorContext &behcontext);
int DoObjectKeepOnFloorV2Box(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorObjectKeepOnFloorV2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Keep On Floor V2");
    od->SetDescription("Forces an object to stay on a declared floor (so it will not hover above it).");
    /* rem:
    <BR>
    <SPAN CLASS=in>On: </SPAN>starts recording.<BR>
    <SPAN CLASS=in>Off: </SPAN>stops recording.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>activated after entering by ON.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>activated after entering by OFF.<BR>
    <SPAN CLASS=out>Touched Floor Limits: </SPAN>is activated if the object touches the floors' limits.<BR>
    <BR>
    <SPAN CLASS=pin>Follow Inclination: </SPAN>if TRUE, then the object will always be upright, parallel to the face of the floor, even if there is a slope for example.<BR>
    <SPAN CLASS=pin>Replacement Altitude: </SPAN>distance of the object from the floor, 0 means touching.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <SPAN CLASS=pin>Keep In Floor Boundary: </SPAN>if TRUE, the object will not be able to leave the floor's boundaries.<BR>
    <SPAN CLASS=pin>Detection Offset: </SPAN>distance from the bottom of the object after which a character will take a floor above rather than a floor below.<BR>
    <SPAN CLASS=pin>Orientation Attenuation: </SPAN>attenuates the effect of Follow Inclination, with 100%
    being very rough, and 1% being very smooth.<BR>
    <BR>
    <SPAN CLASS=pout>Floor: </SPAN>object that has been chosen as the nearest floor.<BR>
    <SPAN CLASS=pout>Floor Type: </SPAN>type of floor chosen, for example grass, ice, tarmac etc. The type of floor is represented by a whole number defined by the user in Attributes Setup.<BR>
    <SPAN CLASS=pout>Floor Slope: </SPAN>slope of the floor in relation to the object's position.<BR>
    <SPAN CLASS=pout>Old Floor Distance: </SPAN>distance of the object off the floor before the object was forced to stay on it.<BR>
    <BR>
    <SPAN CLASS=setting>Object Extents: </SPAN>how the object should be represented for calculation purposes : a single point or a bounding box. An object with the bounding box setting needs 4 times more computation time than the single point version.<BR>
    <BR>
    This building block makes a 3D entity stay on the floors, by testing either the bounding box of an object or a single point in relation to a floor.<BR>
    Objects representing floors must be declared as floors first, either by using the building block Declare Floors or by setting Floor Attributes to them.<BR>
    */
    /* warning:
    This building block needs to be looped for an object to be constrained to a floor all the time.<BR>
    The building block "Character Keep On Floor" should be used for characters.<BR>
    */
    od->SetCategory("3D Transformations/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3de853bd, 0x133d0d2e));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectKeepOnFloorV2BehaviorProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR
ObjectKeepOnFloorV2Callback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = beh->GetCKContext();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        int oe = 1;
        beh->GetLocalParameterValue(0, &oe);

        switch (oe)
        {
        case 1:
            beh->SetFunction(DoObjectKeepOnFloorV2Point);
            break;
        case 2:
            beh->SetFunction(DoObjectKeepOnFloorV2Box);
            break;
        }
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateObjectKeepOnFloorV2BehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Keep On Floor V2");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Touched Floor Limits");

    proto->DeclareInParameter("Follow Inclination", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Replacement Altitude", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Keep In Floor Boundary", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Detection Offset", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Orientation Attenuation", CKPGUID_PERCENTAGE, "90");

    proto->DeclareOutParameter("Floor", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Floor Type", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Floor Slope", CKPGUID_ANGLE, "0");
    proto->DeclareOutParameter("Old Floor Distance", CKPGUID_FLOAT, "0");

    proto->DeclareSetting("Object Extents", CKGUID(0x193e0d51, 0x54625c8a), "Point");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(DoObjectKeepOnFloorV2Point);
    proto->SetBehaviorCallbackFct(ObjectKeepOnFloorV2Callback);

    *pproto = proto;
    return CK_OK;
}

int DoObjectKeepOnFloorV2Point(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK3dEntity *entity = (CK3dEntity *)beh->GetTarget();
    if (!entity)
        return CKBR_OWNERERROR;

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    if (beh->IsInputActive(1))
    { // WE GET BY THE OFF INPUT
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }
    else
    {
        if (beh->IsInputActive(0))
        { // WE GET BY THE ON INPUT
            beh->ActivateInput(0, FALSE);
            beh->ActivateOutput(0);
        }
    }

    CKBOOL follow = FALSE;
    beh->GetInputParameterValue(0, &follow);

    float offset = 0.0;
    beh->GetInputParameterValue(1, &offset);

    CKBOOL hiera = TRUE;
    beh->GetInputParameterValue(2, &hiera);
    hiera = !hiera;

    CKBOOL keepinfloor = FALSE;
    beh->GetInputParameterValue(3, &keepinfloor);

    float detoffset = 0.0;
    beh->GetInputParameterValue(4, &detoffset);

    float orientationatt = 0.9f;
    beh->GetInputParameterValue(5, &orientationatt);

    CKFloorPoint fp;
    CK_FLOORNEAREST floorn;

    const VxBbox &box = entity->GetBoundingBox();

    VxVector pos = (box.Max + box.Min) * 0.5f;
    pos.y = box.Min.y + detoffset;

    if (floorn = floorman->GetNearestFloors(pos, &fp))
    {

        VxVector up;
        entity->GetPosition(&pos);

        switch (floorn)
        {
        case CKFLOOR_DOWN:
            pos.y += fp.m_DownDistance + offset + detoffset;
            up = fp.m_DownNormal;
            break;
        case CKFLOOR_UP:
            pos.y += fp.m_UpDistance + offset + detoffset;
            up = fp.m_UpNormal;
            break;
        }

        entity->SetPosition(&pos, NULL, hiera);
        if (follow)
        {
            VxVector dir, right, vup;
            entity->GetOrientation(&dir, &vup, &right);
            vup = up * orientationatt + vup * (1 - orientationatt);
            right = Normalize(CrossProduct(up, dir));
            dir = Normalize(CrossProduct(right, up));
            entity->SetOrientation(&dir, &vup, &right, NULL, hiera);
        }
    }
    else
    {
        beh->ActivateOutput(2);
        if (keepinfloor)
        {
            if (entity->GetFlags() | CK_3DENTITY_UPDATELASTFRAME)
            {
                // we now have to test if it were on a floor before
                VxVector newpos = (box.Max + box.Min) * 0.5f;
                newpos.y = box.Min.y;
                VxMatrix wmat = entity->GetWorldMatrix();
                const VxMatrix &mat = entity->GetLastFrameMatrix();
                entity->SetWorldMatrix(mat);
                const VxBbox &lbox = entity->GetBoundingBox();
                VxVector oldpos = (lbox.Max + lbox.Min) * 0.5f;
                oldpos.y = lbox.Min.y + detoffset;
                entity->SetWorldMatrix(wmat);

                fp.Clear();
                if (floorn = floorman->GetNearestFloors(oldpos, &fp))
                {

                    int faces[FACESMAXNUMBER];

                    CK3dEntity *floor;
                    switch (floorn)
                    {
                    case CKFLOOR_DOWN:
                        floor = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                        break;
                    case CKFLOOR_UP:
                        floor = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                        break;
                    }

                    VxVector e1, e2, edge1, edge2, inter, intersection;
                    CKBOOL edgecross;

                    // we loop until we are safely replaced
                    int failure = 0;
                    do
                    {
                        // we now want to select the face under the segment oldpos -> newpos
                        int nbcf = CrossedFaces(floor, oldpos, newpos, faces, FACESMAXNUMBER);

                        // no faces crossed : must be an error...
                        if (!nbcf)
                            break;

                        float maxsm = 100000.0f, sm;

                        edgecross = FALSE;
                        for (int i = 0; i < nbcf; i++)
                        {
                            sm = RayExtrudedFacesIntersection(floor, oldpos, newpos, faces[i], inter, e1, e2);
                            if (sm < maxsm)
                            {
                                maxsm = sm;
                                intersection = inter;
                                edge1 = e1;
                                edge2 = e2;
                                edgecross = TRUE;
                            }
                        }

                        if (edgecross)
                        {
                            VxVector edge = Normalize(edge2 - edge1);
                            float ps = DotProduct(newpos - intersection, edge);
                            newpos = intersection + edge * ps + (oldpos - pos) * 0.01f;
                        }
                        else
                        {   // no edge crossed : probably a bug
                            //							CKOutputToConsole("No Edge Crossed");
                            newpos = oldpos;
                        }

                        newpos.y = box.Min.y;
                        fp.Clear();
                        floorn = floorman->GetNearestFloors(newpos, &fp);
                        failure++;
                    } while (!floorn && failure < 8);

                    VxVector up;

                    switch (floorn)
                    {
                    case CKFLOOR_DOWN:
                        newpos.y += fp.m_DownDistance + offset + detoffset;
                        up = fp.m_DownNormal;
                        break;
                    case CKFLOOR_UP:
                        newpos.y += fp.m_UpDistance + offset + detoffset;
                        up = fp.m_UpNormal;
                        break;
                    }

                    newpos.y += (box.Max.y - box.Min.y) * 0.5f;

                    entity->SetPosition(&newpos, NULL, hiera);
                    if (follow)
                    {
                        VxVector dir, right;
                        entity->GetOrientation(&dir, NULL, &right);
                        right = Normalize(CrossProduct(up, dir));
                        dir = Normalize(CrossProduct(right, up));
                        entity->SetOrientation(&dir, &up, &right, NULL, hiera);
                    }
                }
                else
                { // can't find any intersection : a shitty case
                    // all we can do is replace it where it was before
                    entity->SetWorldMatrix(mat);
                }
            }
        }
    }

    // we now fill the Output parameters
    int type = 0;
    float slope = 0.0f;
    float distance = 0.0f;
    VxVector wup(0, 1.0f, 0);
    switch (floorn)
    {
    case CKFLOOR_UP:
        beh->SetOutputParameterValue(0, &(fp.m_UpFloor));
        floorman->ReadAttributeValues((CK3dEntity *)ctx->GetObject(fp.m_UpFloor), NULL, NULL, &type);
        distance = fp.m_UpDistance;
        slope = acosf(DotProduct(wup, Normalize(fp.m_UpNormal)));
        break;
    case CKFLOOR_DOWN:
        beh->SetOutputParameterValue(0, &(fp.m_DownFloor));
        floorman->ReadAttributeValues((CK3dEntity *)ctx->GetObject(fp.m_DownFloor), NULL, NULL, &type);
        distance = fp.m_DownDistance;
        slope = acosf(DotProduct(wup, Normalize(fp.m_DownNormal)));
        break;
    }
    beh->SetOutputParameterValue(1, &type);
    beh->SetOutputParameterValue(2, &slope);
    beh->SetOutputParameterValue(3, &distance);

    // we do this on the end because we want to be sure that it will be there when replacing it
    if (keepinfloor)
    {
        entity->SetFlags(entity->GetFlags() | CK_3DENTITY_UPDATELASTFRAME);
    }
    // REMOVED BECAUSE IN NETWORKMANAGER IN NEED LASTFRAME ... SO ...
    // WE NEED TO FIND ANOTHER SOLUTION
    /*else {
        entity->SetFlags(entity->GetFlags() & ~CK_3DENTITY_UPDATELASTFRAME);
    }*/

    return CKBR_ACTIVATENEXTFRAME;
}

#define POINT0 1
#define POINT1 2
#define POINT4 4
#define POINT5 8

int Test4Points(CKFloorManager *floorman, VxVector *pts, CKFloorPoint *fps, CK_FLOORNEAREST *floors, float detoffset)
{
    int i, oldfloor = 0;
    for (i = 0; i < 6; i++)
    {
        if (i == 2)
            continue;
        if (i == 3)
            continue;
        fps[i].Clear();
        pts[i] += VxVector(detoffset);
        floors[i] = floorman->GetNearestFloors(pts[i], &fps[i]);
        switch (floors[i])
        {
        case CKFLOOR_UP:
            fps[i].m_UpDistance -= detoffset;
            break;
        case CKFLOOR_DOWN:
            fps[i].m_DownDistance -= detoffset;
            break;
        }
        if (!floors[i])
        {
            switch (i)
            {
            case 0:
                oldfloor |= POINT0;
                break;
            case 1:
                oldfloor |= POINT1;
                break;
            case 4:
                oldfloor |= POINT4;
                break;
            case 5:
                oldfloor |= POINT5;
                break;
            }
        }
        pts[i] -= VxVector(detoffset);
    }

    return oldfloor;
}

int DoObjectKeepOnFloorV2Box(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK3dEntity *entity = (CK3dEntity *)beh->GetTarget();
    if (!entity)
        return CKBR_OWNERERROR;

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    if (beh->IsInputActive(1))
    { // WE GET BY THE OFF INPUT
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }
    else
    {
        if (beh->IsInputActive(0))
        { // WE GET BY THE ON INPUT
            beh->ActivateInput(0, FALSE);
            beh->ActivateOutput(0);
        }
    }

    CKBOOL follow = FALSE;
    beh->GetInputParameterValue(0, &follow);

    float offset = 0.0;
    beh->GetInputParameterValue(1, &offset);

    CKBOOL hiera = TRUE;
    beh->GetInputParameterValue(2, &hiera);
    hiera = !hiera;

    CKBOOL keepinfloor = FALSE;
    beh->GetInputParameterValue(3, &keepinfloor);

    float detoffset = 0.0;
    beh->GetInputParameterValue(4, &detoffset);

    CKFloorPoint fp;
    CK_FLOORNEAREST floorn;

    const VxBbox &box = entity->GetBoundingBox(TRUE);

    int floor = 0;
    CKBOOL up = FALSE;
    float distmin = -1000000000.0;
    VxVector pts[8];
    const VxMatrix &mat = entity->GetWorldMatrix();
    box.TransformTo(pts, mat);
    int i;
    for (i = 0; i < 6; i++)
    {
        if (i == 2)
            continue;
        if (i == 3)
            continue;
        fp.Clear();
        pts[i].y += detoffset;
        if (floorn = floorman->GetNearestFloors(pts[i], &fp))
        {
            switch (floorn)
            {
            case CKFLOOR_DOWN:
                pts[i].y += fp.m_DownDistance + detoffset;
                if (!up)
                {
                    if (fp.m_DownDistance > distmin)
                    {
                        distmin = fp.m_DownDistance;
                    }
                }
                break;
            case CKFLOOR_UP:
                pts[i].y += fp.m_UpDistance + detoffset;
                if (fp.m_UpDistance > distmin)
                {
                    distmin = fp.m_UpDistance;
                    up = TRUE;
                }
                break;
            }
        }
        else
        {
            switch (i)
            {
            case 0:
                floor |= POINT0;
                break;
            case 1:
                floor |= POINT1;
                break;
            case 4:
                floor |= POINT4;
                break;
            case 5:
                floor |= POINT5;
                break;
            }
        }
        pts[i].y -= detoffset;
    }

    // We now test if there are points above no floors
    if (!floor)
    { // every point are up the floor
        // TODO : move all this after the replacement loop...
        VxVector dir = Normalize(pts[1] - pts[0] + pts[5] - pts[4]);
        VxVector right = Normalize(pts[4] - pts[0] + pts[5] - pts[1]);

        VxVector vup = Normalize(CrossProduct(dir, right));

        if (follow)
        {
            entity->SetOrientation(&dir, &vup, &right, NULL, hiera);
        }
        entity->Translate3f(0, distmin + offset + detoffset, 0, NULL, hiera);

        switch (floorn)
        {
        case CKFLOOR_UP:
            fp.m_UpNormal = vup;
            break;
        case CKFLOOR_DOWN:
            fp.m_DownNormal = vup;
            break;
        }
    }
    else
    { // some or all points are off the floor

        beh->ActivateOutput(2);
        // we check if the user wants limit constraint and if it saved its last matrix
        if (keepinfloor && entity->GetFlags() | CK_3DENTITY_UPDATELASTFRAME)
        {
            const VxMatrix &oldmat = entity->GetLastFrameMatrix();

            VxVector oldpts[8];
            CKFloorPoint oldfps[6];
            CKFloorPoint fps[6];
            CK_FLOORNEAREST oldfloorsn[6];
            CK_FLOORNEAREST floorsn[6];

            box.TransformTo(oldpts, oldmat);

            int i, oldfloor = 0;
            for (i = 0; i < 6; i++)
            {
                if (i == 2)
                    continue;
                if (i == 3)
                    continue;
                oldfps[i].Clear();
                floorn = floorman->GetNearestFloors(oldpts[i], &oldfps[i]);
                oldfloorsn[i] = floorn;
                if (!floorn)
                    break;
            }

            // we can only constrain if all the old points were on a floor
            if (floorn)
            {
                // The replacement loop : while there are points outside, we continue
                CKBOOL pointsOutside = TRUE;
                int maximumTry = 0;
                int faces[FACESMAXNUMBER];

                int max_i;
                VxVector newpos;
                do
                {
                    VxVector max_edge1, max_edge2, max_intersection;
                    // this is the greater over distance
                    float max_distance = 0.0f;
                    max_i = 6;

                    for (i = 0; i < 6; i++)
                    {
                        if (i == 2)
                            continue;
                        if (i == 3)
                            continue;

                        // we have to test the point only if it is outside now
                        switch (i)
                        {
                        case 0:
                            if (!(floor & POINT0))
                                continue;
                            break;
                        case 1:
                            if (!(floor & POINT1))
                                continue;
                            break;
                        case 4:
                            if (!(floor & POINT4))
                                continue;
                            break;
                        case 5:
                            if (!(floor & POINT5))
                                continue;
                            break;
                        }

                        CK3dEntity *floorobject = NULL;
                        switch (oldfloorsn[i])
                        {
                        case CKFLOOR_UP:
                            floorobject = (CK3dEntity *)ctx->GetObject(oldfps[i].m_UpFloor);
                            break;
                        case CKFLOOR_DOWN:
                            floorobject = (CK3dEntity *)ctx->GetObject(oldfps[i].m_DownFloor);
                            break;
                        }

                        // no floor : buggy
                        if (!floorobject)
                            continue;

                        // we now want to select the face under the segment oldpos -> newpos
                        int nbcf = CrossedFaces(floorobject, oldpts[i], pts[i], faces, FACESMAXNUMBER);

                        // no faces crossed : must be an error...
                        if (!nbcf)
                        {
                            ctx->OutputToConsole("No Faces crossed");
                            break;
                        }

                        float maxsm = 100000.0f, sm;
                        VxVector e1, e2, edge1, edge2, inter, intersection;
                        CKBOOL edgecross = FALSE;

                        for (int j = 0; j < nbcf; j++)
                        {
                            sm = RayExtrudedFacesIntersection(floorobject, oldpts[i], pts[i], faces[j], inter, e1, e2);
                            if (sm < maxsm)
                            {
                                maxsm = sm;
                                intersection = inter;
                                edge1 = e1;
                                edge2 = e2;
                                edgecross = TRUE;
                            }
                        }

                        if (edgecross)
                        {
                            if (maxsm > max_distance)
                            {
                                max_distance = maxsm;
                                max_edge1 = e1;
                                max_edge2 = e2;
                                max_intersection = intersection;
                                max_i = i;
                            }
                        }
                        else
                        {
                            ctx->OutputToConsoleEx("No Edge Cross : nbcf = %d ; i = %d old = %f,%f pts = %f,%f", nbcf, i, oldpts[i].x, oldpts[i].z, pts[i].x, pts[i].z);
                        }
                    }

                    // we now replace the object according to the point that has exceed the much the limits
                    if (max_i < 6)
                    { // we test if it found a good point (if not : hum, not good)
                        VxVector edge = Normalize(max_edge2 - max_edge1);
                        float ps = DotProduct(pts[max_i] - max_intersection, edge);
                        newpos = max_intersection + edge * ps + (oldpts[max_i] - pts[max_i]) * 0.1f - pts[max_i];
                        // we translate every points by this vector
                        for (int j = 0; j < 6; j++)
                        {
                            if (j == 2)
                                continue;
                            if (j == 3)
                                continue;
                            pts[j] += newpos;
                        }
                        // and we translate effectively the object
                        entity->Translate(&newpos, NULL, hiera);

                        floor = Test4Points(floorman, pts, fps, floorsn, detoffset);

                        if (maximumTry > 0)
                        {
                            /*
                            char buffer[256];
                            sprintf(buffer,"Maximum Try = %d, maxi = %d, inter = %f,%f",maximumTry,max_i,max_intersection.x,max_intersection.z);
                            CKOutputToConsole(buffer);
                            */
                        }

                        if (!floor)
                        { // all points are inside the floor
                            pointsOutside = FALSE;
                        }
                        else
                        {
                            pointsOutside = TRUE;
                        }
                    }
                    else
                    {
                        // we didn't found good replacement
                        // Normally we should exit but for now, all we can do is set the
                        // old matrix
                        ctx->OutputToConsole("No Replacement found : replacing at the old one");
                        entity->SetWorldMatrix(oldmat);
                        memcpy(pts, oldpts, 8 * sizeof(VxVector));
                        memcpy(fps, oldfps, 6 * sizeof(CKFloorPoint));
                        memcpy(floorsn, oldfloorsn, 6 * sizeof(CK_FLOORNEAREST));
                        pointsOutside = FALSE;
                    }

                    maximumTry++;
                } while (pointsOutside && maximumTry < 8);

                // we now have to DO the floor constraint in Y
                if (maximumTry < 8)
                { // only if we could replace it...
                    for (i = 0; i < 6; i++)
                    {
                        if (i == 2)
                            continue;
                        if (i == 3)
                            continue;

                        switch (floorsn[i])
                        {
                        case CKFLOOR_DOWN:
                            pts[i].y += fps[i].m_DownDistance;
                            if (!up)
                            {
                                if (fps[i].m_DownDistance > distmin)
                                {
                                    distmin = fps[i].m_DownDistance;
                                }
                            }
                            break;
                        case CKFLOOR_UP:
                            pts[i].y += fps[i].m_UpDistance;
                            if (fps[i].m_UpDistance > distmin)
                            {
                                distmin = fps[i].m_UpDistance;
                                up = TRUE;
                            }
                            break;
                        }
                    }

                    VxVector dir = Normalize(pts[1] - pts[0] + pts[5] - pts[4]);
                    VxVector right = Normalize(pts[4] - pts[0] + pts[5] - pts[1]);

                    VxVector vup = Normalize(CrossProduct(dir, right));

                    if (follow)
                    {
                        entity->SetOrientation(&dir, &vup, &right, NULL, hiera);
                    }
                    entity->Translate3f(0, distmin + offset, 0, NULL, hiera);

                    floorn = floorsn[0];
                    switch (floorn)
                    {
                    case CKFLOOR_UP:
                        fp.m_UpFloor = fps[i].m_UpFloor;
                        fp.m_UpNormal = vup;
                        break;
                    case CKFLOOR_DOWN:
                        fp.m_DownFloor = fps[i].m_DownFloor;
                        fp.m_DownNormal = vup;
                        break;
                    }
                }
            } // end if(floorn)
        } // end if(keepinfloor)
    }

    // we now fill the Output parameters
    int type = 0;
    float slope = 0.0f;
    float distance = distmin;
    VxVector wup(0, 1.0f, 0);
    switch (floorn)
    {
    case CKFLOOR_UP:
        beh->SetOutputParameterValue(0, &(fp.m_UpFloor));
        floorman->ReadAttributeValues((CK3dEntity *)ctx->GetObject(fp.m_UpFloor), NULL, NULL, &type);
        slope = acosf(DotProduct(wup, Normalize(fp.m_UpNormal)));
        break;
    case CKFLOOR_DOWN:
        beh->SetOutputParameterValue(0, &(fp.m_DownFloor));
        floorman->ReadAttributeValues((CK3dEntity *)ctx->GetObject(fp.m_DownFloor), NULL, NULL, &type);
        slope = acosf(DotProduct(wup, Normalize(fp.m_DownNormal)));
        break;
    }
    beh->SetOutputParameterValue(1, &type);
    beh->SetOutputParameterValue(2, &slope);
    beh->SetOutputParameterValue(3, &distance);

    // we do this on the end because we want to be sure that it will be there when replacing it
    if (keepinfloor)
    {
        entity->SetFlags(entity->GetFlags() | CK_3DENTITY_UPDATELASTFRAME);
    }

    return CKBR_ACTIVATENEXTFRAME;
}
