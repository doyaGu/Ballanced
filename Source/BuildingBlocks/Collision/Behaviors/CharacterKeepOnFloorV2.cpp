/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CharacterKeepOnFloorV2
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "FloorTools.h"

#define CKPGUID_FRONTAL_DIRECTION CKDEFINEGUID(0x286652d, 0x5ea709c2)

CKERROR CreateCharacterKeepOnFloorV2BehaviorProto(CKBehaviorPrototype **);
int DoCharacterKeepOnFloorV2(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCharacterKeepOnFloorV2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Enhanced Character Keep On Floor");
    od->SetDescription("Forces a character to stay on a declared floor (so it will not hover above it).");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <SPAN CLASS=out>Touched Floor Limits: </SPAN>is activated if the charcter touches the floors' limits.<BR>
    <BR>
    <SPAN CLASS=pin>Follow Inclination: </SPAN>TRUE means the character will follow any curvatures in the floor.
    This parameter works together with "Orientation Attenuation" parameter, which sets how smooth a character's change
    in orientation would be.<BR>
    <SPAN CLASS=pin>Replacement Altitude: </SPAN>distance of the charcter from the floor, 0 (zero) means touching.<BR>
    <SPAN CLASS=pin>Keep In Floor Boundary: </SPAN>if TRUE, ther character will not be able to venture outside the floor's boundaries.<BR>
    <SPAN CLASS=pin>Detection Offset: </SPAN>distance from the bottom of the object after which a character
    will take a floor above rather than a floor below. It is also the maximum height, in the world's Y axis,
    that a character can climb.<BR>
    <SPAN CLASS=pin>Character Extents: </SPAN>extent of the character to take into account (showed by a little white rectangle).<BR>
    <SPAN CLASS=pin>Orientation Attenuation: </SPAN>attenuates the effect of Follow Inclination, with 100%
    being very rough, and 1% being very smooth.<BR>
    <SPAN CLASS=pin>Weight: </SPAN>the weight of the character, so as to simulate the effect of gravity.<BR>
    <SPAN CLASS=pin>Character Direction: </SPAN>defines the character's frontal direction.<BR>
    <BR>
    <SPAN CLASS=pout>Floor: </SPAN>object that has been chosen as the nearest floor.<BR>
    <SPAN CLASS=pout>Floor Type: </SPAN>type of the floor chosen, for example grass, ice, tarmac etc. The type of floor is represented by a whole number defined by the user in the attributes setup.<BR>
    <SPAN CLASS=pout>Floor Normal: </SPAN>normal of the floor in relation to the character's position.<BR>
    <SPAN CLASS=pout>Old Floor Distance: </SPAN>character's distance from the floor before being forced to stay on it.<BR>
    <BR>
    <SPAN CLASS=setting>Show Extents: </SPAN>if checked, the character's extent, shown by a white rectangle will be seen.<BR>
    <BR>
    To have a floor reference object (for example feet), the character must have an object containing
    ("FloorRef","Foot","Pas") in its name, and be a child of the root of the character in order to be
    recognized.<BR>
    If this object is not present the building block uses a character's bounding box to make contact.<BR>
    Objects representing floors must be declared as floors first, either by using the building block
    DeclareFloors or giving them the Floor attribute.<BR>
    See also: Character Keep On Floor<BR>
    */
    od->SetCategory("Characters/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xdbde2424, 0x6addef12));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCharacterKeepOnFloorV2BehaviorProto);
    od->SetCompatibleClassId(CKCID_CHARACTER);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

struct CKOF2
{
    VxVector pts[4];
    CKFloorPoint fps[4];
    CK_FLOORNEAREST floors[4];
};

CKERROR CharacterKeepOnFloorV2Callback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = beh->GetCKContext();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        CKMesh *tempMesh = (CKMesh *)ctx->CreateObject(CKCID_MESH, "EKOF Mesh", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        tempMesh->SetVertexCount(4);
        tempMesh->SetLineCount(4);
        tempMesh->SetLine(0, 0, 1);
        tempMesh->SetLine(1, 1, 2);
        tempMesh->SetLine(2, 2, 3);
        tempMesh->SetLine(3, 3, 0);
        tempMesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        CK3dEntity *tempEntity = (CK3dEntity *)ctx->CreateObject(CKCID_3DENTITY, "EKOF Entity", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        tempEntity->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        tempEntity->SetCurrentMesh(tempMesh);
        beh->SetLocalParameterObject(0, tempEntity);
        CKOF2 olddata;
        beh->SetLocalParameterValue(2, &olddata, sizeof(CKOF2));
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CK3dEntity *tempEntity;
        tempEntity = (CK3dEntity *)beh->GetLocalParameterObject(0);
        CKDWORD displayExtents = CKHIDE;
        beh->GetLocalParameterValue(1, &displayExtents);
        tempEntity->Show((CK_OBJECT_SHOWOPTION)displayExtents);
    }
    break;
    case CKM_BEHAVIORDELETE:
    {
        CK3dEntity *tempEntity;
        tempEntity = (CK3dEntity *)beh->GetLocalParameterObject(0);
        if (tempEntity)
        {
            CKMesh *tempMesh = tempEntity->GetCurrentMesh();
            CKDestroyObject(tempMesh);
            CKDestroyObject(tempEntity);
        }
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    {
        CK3dEntity *tempEntity;
        tempEntity = (CK3dEntity *)beh->GetLocalParameterObject(0);

        // we remove the emitter from all the scene it was in
        for (int i = 0; i < tempEntity->GetSceneInCount(); i++)
        {
            CKScene *scene = tempEntity->GetSceneIn(i);
            if (scene)
                scene->RemoveObjectFromScene(tempEntity);
        }

        CKScene *scene = ctx->GetCurrentScene();
        if (beh->IsParentScriptActiveInScene(scene))
            scene->AddObjectToScene(tempEntity);
    }
    }
    return CKBR_OK;
}

CKERROR CreateCharacterKeepOnFloorV2BehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Enhanced Character Keep On Floor");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Touched Floor Limits");

    proto->DeclareInParameter("Follow Inclination", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Replacement Altitude", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Keep In Floor Boundary", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Detection Offset", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Character Extents", CKPGUID_2DVECTOR, "0.1,0.1");
    proto->DeclareInParameter("Orientation Attenuation", CKPGUID_PERCENTAGE, "90");
    proto->DeclareInParameter("Weight", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Character Direction", CKPGUID_FRONTAL_DIRECTION, "Z");

    proto->DeclareOutParameter("Floor", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Floor Type", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Floor Normal", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareOutParameter("Old Floor Distance", CKPGUID_FLOAT, "0");
    proto->DeclareOutParameter("Down Velocity", CKPGUID_VECTOR, "0,0,0");

    // Entity
    proto->DeclareLocalParameter(NULL, CKPGUID_3DENTITY);
    proto->DeclareSetting("Show Extents", CKPGUID_BOOL, "FALSE");
    // Old Values
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // MAc Safe

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorCallbackFct(CharacterKeepOnFloorV2Callback);
    proto->SetFunction(DoCharacterKeepOnFloorV2);
    *pproto = proto;
    return CK_OK;
}

#define FACESMAXNUMBER 64

int Test4Points3(CKFloorManager *floorman, VxVector *pts, CKFloorPoint *fps, CK_FLOORNEAREST *floors, float detoffset)
{
    int oldfloor = 0;

    for (int i = 0; i < 4; i++)
    {
        fps[i].Clear();
        floors[i] = floorman->GetNearestFloors(pts[i], &fps[i]);

        switch (floors[i])
        {
        case CKFLOOR_UP:
            // we check if the up has not been preferred to a down floor
            // whereas it was outer the detoffset limit
            if (fps[i].m_DownFloor && (fps[i].m_UpDistance > detoffset))
                floors[i] = CKFLOOR_DOWN;
            break;
        case CKFLOOR_DOWN:
            // we check if an up floor can be preferred to a down, even
            // if the down is nearer because of the detoffset preference
            if (fps[i].m_UpFloor && (fps[i].m_UpDistance < detoffset))
                floors[i] = CKFLOOR_UP;
            break;
        case CKFLOOR_NOFLOOR:
            oldfloor |= (1 << i);
            break;
        }
    }

    return oldfloor;
}

void ReturnCharacterDirection(CK3dEntity *carac, int CharacterDirection, VxVector *dir, VxVector *up, VxVector *right)
{
    switch (CharacterDirection)
    {
    case 1:
        carac->GetOrientation(right, dir, up);
        break;
    case 2:
    {
        carac->GetOrientation(right, dir, up);
        if (dir)
            *dir = -*dir;
        if (right)
            *right = -*right;
    }
    break;
    case 3:
        carac->GetOrientation(up, right, dir);
        break;
    case 4:
    {
        carac->GetOrientation(up, right, dir);
        if (dir)
            *dir = -*dir;
        if (right)
            *right = -*right;
    }
    break;
    case 5:
        carac->GetOrientation(dir, up, right);
        break;
    case 6:
    {
        carac->GetOrientation(dir, up, right);
        if (dir)
            *dir = -*dir;
        if (right)
            *right = -*right;
    }
    break;
    }
}

void SetCharacterDirection(CK3dEntity *carac, int CharacterDirection, VxVector *dir, VxVector *up, VxVector *right)
{
    VxVector nright = CrossProduct(*up, *dir);
    switch (CharacterDirection)
    {
    case 1:
        carac->SetOrientation(&nright, dir, up);
        break;
    case 2:
    {
        if (dir)
            *dir = -*dir;
        nright = -nright;
        carac->SetOrientation(&nright, dir, up);
    }
    break;
    case 3:
        carac->SetOrientation(up, &nright, dir);
        break;
    case 4:
    {
        if (dir)
            *dir = -*dir;
        nright = -nright;
        carac->SetOrientation(up, &nright, dir);
    }
    break;
    case 5:
        carac->SetOrientation(dir, up, right);
        break;
    case 6:
    {
        if (dir)
            *dir = -*dir;
        if (right)
            *right = -*right;
        carac->SetOrientation(dir, up, right);
    }
    break;
    }
}

int DoCharacterKeepOnFloorV2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKCharacter *character = (CKCharacter *)beh->GetOwner();

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    // replacement altitude
    float offset = 0.0;
    beh->GetInputParameterValue(1, &offset);

    // we get the down velocity
    VxVector *downvelocity;
    CKParameterOut *dvpout = beh->GetOutputParameter(4);
    if (!dvpout)
    {
        ctx->OutputToConsole("Character Keep On Floor V2 : Obsolete Version : please drag and drop the new one");
        return CKBR_PARAMETERERROR;
    }
    downvelocity = (VxVector *)dvpout->GetWriteDataPtr(); // we get the output param

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
            // we reset the down velocity
            downvelocity->Set(0, 0, 0);
        }
        else
        { // we've been reactivated automatically
            // we translate down the character
            if (offset > 0.0f)
                character->Translate3f(0, -offset, 0);
        }
    }

    // we get the weight
    float weight = 0;
    beh->GetInputParameterValue(6, &weight);

    // we apply the gravity to the character
    character->Translate(downvelocity);

    downvelocity->y -= weight * behcontext.DeltaTime * 0.001f; // we apply the gravity

    // Input Parameters Variables
    CKBOOL follow = FALSE;
    beh->GetInputParameterValue(0, &follow);

    CKBOOL keepinfloor = FALSE;
    beh->GetInputParameterValue(2, &keepinfloor);

    float detoffset = 0.0;
    beh->GetInputParameterValue(3, &detoffset);

    Vx2DVector extents(1.0f, 1.0f);
    beh->GetInputParameterValue(4, &extents);

    float orientationatt = 0.9f;
    beh->GetInputParameterValue(5, &orientationatt);

    // we get the old data structure
    CKOF2 *olddata = (CKOF2 *)beh->GetLocalParameterWriteDataPtr(2);

    // Output Parameters Variables
    CK3dEntity *outFloor = NULL;
    int outFloorType = 0;
    VxVector outFloorNormal(0, 0, 0);
    float outFloorOldDistance = 0;

    // we search the low part of the character
    float verticalBasement = 0.0f;

    CK3dEntity *floorReferenceObject = character->GetFloorReferenceObject();
    if (floorReferenceObject)
    { // the character possess a reference object : we use it as low base
        VxVector pos;
        floorReferenceObject->GetPosition(&pos);
        verticalBasement = pos.y;
    }
    else
    { // no reference object : we take the bottom of the bounding box
        const VxBbox &box = character->GetHierarchicalBox();
        verticalBasement = box.Min.y;
    }

    // we'll get the center of the root body part as the character center
    CK3dEntity *rootObject = character->GetRootBodyPart();
    if (!rootObject)
        return CKBR_GENERICERROR;

    VxVector characterCenter;
    rootObject->GetBaryCenter(&characterCenter);

    /////////////////////////////////////////////////////////////
    // now we must define the 4 basement points for the character
    //           0      <X>      1
    //            ---------------          Z
    //          ^|               |         ^
    //          Y|               |         |
    //          v|               |          -->X
    //            ---------------
    //           3               2

    // We get the character Orientation
    int characterOrientation = 1;
    beh->GetInputParameterValue(7, &characterOrientation);
    VxVector dir;
    ReturnCharacterDirection(rootObject, characterOrientation, &dir, NULL, NULL);

    // we first need the character orientation
    dir.y = 0;
    dir.Normalize();
    VxVector right = Normalize(CrossProduct(VxVector(0, 1, 0), dir));

    // apply extents length to vectors
    right *= extents.x;
    dir *= extents.y;

    // now we find the 4 points
    VxVector pts[4];
    pts[0] = characterCenter + dir - right;
    pts[1] = characterCenter + dir + right;
    pts[2] = characterCenter - dir + right;
    pts[3] = characterCenter - dir - right;
    pts[0].y = verticalBasement;
    pts[1].y = verticalBasement;
    pts[2].y = verticalBasement;
    pts[3].y = verticalBasement;

    // we now check the 4 points for floor validity
    CKFloorPoint fps[4];
    CK_FLOORNEAREST floors[4];
    int floor = 0;
    floor = Test4Points3(floorman, pts, fps, floors, detoffset);

    // we now look for the higher Y point which should be taken as a base for the character
    // not considering detoffset and offset

    CKBOOL up = FALSE;
    float higherYReplacement = -1000000000.0;
    for (int i = 0; i < 4; ++i)
    {
        switch (floors[i])
        {
        case CKFLOOR_DOWN:
            if (fps[i].m_DownDistance > higherYReplacement)
            {
                higherYReplacement = fps[i].m_DownDistance;
            }
            pts[i].y += fps[i].m_DownDistance;
            break;
        case CKFLOOR_UP:
            if (fps[i].m_UpDistance > higherYReplacement)
            {
                higherYReplacement = fps[i].m_UpDistance;
            }
            pts[i].y += fps[i].m_UpDistance;
            up = TRUE;
            break;
        }
    }

    if (up)
        downvelocity->Set(0.0f, 0.0f, 0.0f);

    // We now test if there are points above no floors
    if (!floor)
    {
        // every point are up the floor

        // we calculate the medium direction vector
        VxVector dir = Normalize(pts[0] - pts[3] + pts[1] - pts[2]);
        // we calculate the medium right vector
        VxVector right = Normalize(pts[1] - pts[0] + pts[2] - pts[3]);

        VxVector vup = Normalize(CrossProduct(dir, right));

        if (follow && up)
        {
            VxVector up;
            character->GetOrientation(&dir, &up);
            vup = up * orientationatt + vup * (1 - orientationatt);
            // we set a limit angle
            if (DotProduct(vup, VxVector(0, 1.0f, 0)) > 0.8)
            {
                right = Normalize(CrossProduct(vup, dir));
                dir = Normalize(CrossProduct(right, vup));
                character->SetOrientation(&dir, &vup, &right);
            }
        }
        if (weight <= 0.0f || up)
        {
            character->Translate3f(0, higherYReplacement, 0);
        }

        outFloorNormal = vup;
        switch (floors[0])
        {
        case CKFLOOR_DOWN:
            outFloor = (CK3dEntity *)ctx->GetObject(fps[0].m_DownFloor);
            break;
        case CKFLOOR_UP:
            outFloor = (CK3dEntity *)ctx->GetObject(fps[0].m_UpFloor);
            break;
        }
        outFloorOldDistance = higherYReplacement + offset;
        beh->ActivateOutput(0);
    }
    else
    {
        // some or all points are off the floor

        // we check if the user wants limit constraint and if it saved its last matrix
        if (keepinfloor && character->GetFlags() | CK_3DENTITY_UPDATELASTFRAME)
        {
            // only activated when the user wants to be kept in floors
            beh->ActivateOutput(2);

            // old floor positions
            CK_FLOORNEAREST oldfloors[4];
            // now we find the 4 points
            VxVector oldpts[4];
            // Old floor points
            CKFloorPoint oldfps[4];

            // we get these values
            memcpy(oldpts, olddata->pts, 4 * sizeof(VxVector));
            memcpy(oldfps, olddata->fps, 4 * sizeof(CKFloorPoint));
            memcpy(oldfloors, olddata->floors, 4 * sizeof(CK_FLOORNEAREST));

            // TEMP for a woking version : no sliding
            character->SetWorldMatrix(character->GetLastFrameMatrix());
            memcpy(pts, oldpts, 4 * sizeof(VxVector));
            memcpy(fps, oldfps, 4 * sizeof(CKFloorPoint));
            memcpy(floors, oldfloors, 4 * sizeof(CK_FLOORNEAREST));
            // end of TEMP

            /*
            // we now look where they were...
            BOOL oldpointsinside = TRUE;
            for(int i=0;i<4;i++) {
                oldpts[i].y = pts[i].y;
                if(!oldfloors[i]) oldpointsinside = FALSE;
            }

            // we can only constrain if all the old points were on a floor
            if(oldpointsinside) {
                // The replacement loop : while there are points outside, we continue
                BOOL pointsOutside = TRUE;
                int maximumTry = 0;
                VxFace* faces[FACESMAXNUMBER];

                int max_i;
                VxVector newpos;
                do {
                    VxVector max_edge1,max_edge2,max_intersection;
                    // this is the greater over distance
                    float max_distance = 0.0f;
                    max_i = 4;

                    for(i=0;i<4;i++) {
                        // we have to test the point only if it is outside now
                        if(floors[i] != CKFLOOR_NOFLOOR) continue;

                        CK3dEntity* floorobject = NULL;
                        switch(oldfloors[i]) {
                        case CKFLOOR_UP:
                            floorobject = (CK3dEntity*)CKGetObject(oldfps[i].m_UpFloor);
                            break;
                        case CKFLOOR_DOWN:
                            floorobject = (CK3dEntity*)CKGetObject(oldfps[i].m_DownFloor);
                            break;
                        }

                        // no floor : buggy
                        if(!floorobject) continue;

                        // we now want to select the face under the segment oldpos -> newpos
                        int nbcf = CrossedFaces(floorobject,oldpts[i],pts[i],faces,FACESMAXNUMBER);

                        // no faces crossed : must be an error...
                        if(!nbcf) {
#ifdef _DEBUG
                            CKOutputToConsole("No Faces crossed");
#endif
                            break;
                        }

                        float maxsm=100000.0f,sm;
                        VxVector e1,e2,edge1,edge2,inter,intersection;
                        BOOL edgecross = FALSE;

                        for(int j=0;j<nbcf ;j++) {
                            sm = RayExtrudedFacesIntersection(floorobject,oldpts[i],pts[i],faces[j],inter,e1,e2);
                            if(sm<maxsm) {
                                maxsm = sm;
                                intersection = inter;
                                edge1 = e1;
                                edge2 = e2;
                                edgecross = TRUE;
                            }
                        }

                        if(edgecross) {
                            if(maxsm > max_distance) {
                                max_distance = maxsm;
                                max_edge1 = e1;
                                max_edge2 = e2;
                                max_intersection = intersection;
                                max_i = i;
                            }
                        } else {
#ifdef _DEBUG
                            char buffer[256];
                            sprintf(buffer,"No Edge Cross : nbcf = %d ; i = %d old = %f,%f pts = %f,%f",nbcf,i,oldpts[i].x,oldpts[i].z,pts[i].x,pts[i].z);
                            sm = RayExtrudedFacesIntersection(floorobject,oldpts[i],pts[i],faces[0],inter,e1,e2);
                            CKOutputToConsole(buffer);
#endif
                        }

                    }

                    // we now replace the object according to the point that has exceed the much the limits
                    if(max_i < 4) { // we test if it found a good point (if not : hum, not good)
                        VxVector edge = Normalize(max_edge2-max_edge1);
                        float ps = DotProduct(pts[max_i]-max_intersection,edge);
                        newpos = max_intersection + edge*ps+(oldpts[max_i]-pts[max_i])*0.1f - pts[max_i];
                        // we translate every points by this vector
                        for(int j=0;j<4;j++) pts[j]+=newpos;
                        // and we translate effectively the object
                        character->Translate3f(&newpos);

                        floor = Test4Points3(pts,fps,floors,detoffset);

#ifdef _DEBUG
                        if(maximumTry>0) {
                            char buffer[256];
                            sprintf(buffer,"Maximum Try = %d, maxi = %d, inter = %f,%f",maximumTry,max_i,max_intersection.x,max_intersection.z);
                            CKOutputToConsole(buffer);
                        }
#endif

                        if(!floor) { // all points are inside the floor
                            pointsOutside = FALSE;
                        } else {
                            pointsOutside = TRUE;

                        }
                    } else {
                        // we didn't found good replacement
                        // Normally we should exit but for now, all we can do is set the
                        // old matrix
#ifdef _DEBUG
                        CKOutputToConsole("No Replacement found : replacing at the old one");
#endif
                        VxMatrix oldmat;
                        character->GetLastFrameMatrix(oldmat);
                        character->SetWorldMatrix(oldmat);
                        memcpy(pts,oldpts,4*sizeof(VxVector));
                        memcpy(fps,oldfps,4*sizeof(CKFloorPoint));
                        memcpy(floors,oldfloors,4*sizeof(CK_FLOORNEAREST));
                        pointsOutside = FALSE;
                    }

                    maximumTry++;
                } while(pointsOutside && maximumTry<8);

                // we now have to DO the floor constraint in Y
                if(maximumTry<8) { // only if we could replace it...
                    for(i=0;i<4;i++) {
                        switch(floors[i]) {
                        case CKFLOOR_DOWN:
                            pts[i].y += fps[i].m_DownDistance;
                            if(!up) {
                                if(fps[i].m_DownDistance > higherYReplacement) {
                                    higherYReplacement = fps[i].m_DownDistance;
                                }
                            }
                            break;
                        case CKFLOOR_UP:
                            pts[i].y += fps[i].m_UpDistance;
                            if(fps[i].m_UpDistance > higherYReplacement) {
                                higherYReplacement = fps[i].m_UpDistance;
                                up=TRUE;
                            }
                            break;
                        }
                    }

                    // we calculate the medium direction vector
                    VxVector dir = Normalize(pts[0] - pts[3] + pts[1] - pts[2]);
                    // we calculate the medium right vector
                    VxVector right = Normalize(pts[1] - pts[0] + pts[2] - pts[3]);

                    VxVector vup = Normalize(CrossProduct(dir,right));

                    if(follow) {
                        VxVector up;
                        character->GetOrientation(&dir,&up);
                        vup = up*orientationatt+vup*(1-orientationatt);
                        character->SetOrientation(&dir, &vup);
                    }

                    character->Translate3f(0,higherYReplacement+offset,0);

                    outFloorNormal = vup;
                    switch(floors[0]) {
                    case CKFLOOR_DOWN:
                        outFloor = (CK3dEntity*)CKGetObject(fps[0].m_DownFloor);
                        break;
                    case CKFLOOR_UP:
                        outFloor = (CK3dEntity*)CKGetObject(fps[0].m_UpFloor);
                        break;
                    }
                    outFloorOldDistance = higherYReplacement+offset;
                }
            } else { // end if oldpointsinside
#ifdef _DEBUG
                CKOutputToConsole("Object wasn't on floor before");
#endif
            }
            */
        }
        else
        { // end if(keepinfloor)
            beh->ActivateOutput(1);
        }
    }

    // Extents Displaying
    CKBOOL displayExtents = FALSE;
    beh->GetLocalParameterValue(1, &displayExtents);
    if (displayExtents)
    {
        CK3dEntity *tempEntity;
        tempEntity = (CK3dEntity *)beh->GetLocalParameterObject(0);
        if (tempEntity)
        {
            CKMesh *tempMesh = tempEntity->GetCurrentMesh();
            for (int i = 0; i < 4; i++)
            {
                tempMesh->SetVertexPosition(i, &pts[i]);
            }
        }
    }

    // we now fill the old data structure
    memcpy(olddata->pts, pts, 4 * sizeof(VxVector));
    memcpy(olddata->fps, fps, 4 * sizeof(CKFloorPoint));
    memcpy(olddata->floors, floors, 4 * sizeof(CK_FLOORNEAREST));

    // we now fill the Output parameters
    beh->SetOutputParameterObject(0, outFloor);
    floorman->ReadAttributeValues(outFloor, NULL, NULL, &outFloorType);
    beh->SetOutputParameterValue(1, &outFloorType);
    beh->SetOutputParameterValue(2, &outFloorNormal);
    beh->SetOutputParameterValue(3, &outFloorOldDistance);

    // we do this on the end because we want to be sure that it will be there when replacing it
    if (keepinfloor)
    {
        character->SetFlags(character->GetFlags() | CK_3DENTITY_UPDATELASTFRAME);
    }

    // we translate back in space the character
    if (offset > 0.0f)
        character->Translate3f(0, offset, 0);

    dvpout->DataChanged(); // we update the down velocity vector in output
    return CKBR_ACTIVATENEXTFRAME;
}
