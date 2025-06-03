/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ObjectBallSlider
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CalcCollision.h"

#define MINVERTICES_FORCLASSIFY 64

CKObjectDeclaration *FillBehaviorObjectBallSliderDecl();
CKERROR CreateObjectBallSliderProto(CKBehaviorPrototype **);
int ObjectBallSlider(const CKBehaviorContext &behcontext);
CKERROR ObjectBallSliderCallBackObject(const CKBehaviorContext &behcontext);

// Inner struct
typedef struct
{
    VxVector Old_Pos;
} A_OBS;
//---

CKObjectDeclaration *FillBehaviorObjectBallSliderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Slider");
    od->SetDescription("Object Ball Slider ; Impede the 3d object from penetrating a 3dobject in the specified group.");
    /* rem:
      <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
      <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
      <SPAN CLASS=out>Contact: </SPAN>activate each time there's a contact.<BR>
      <SPAN CLASS=out>No Contact: </SPAN>activate each time there's no contact.<BR>
    <BR>
    <SPAN CLASS=pin>Radius: </SPAN>radius to impede object penetration (expressed in world absolute value).<BR>
    <SPAN CLASS=pin>Group: </SPAN>group in which other objects are to be concidered (the object [ball] will not penetrate those objects).<BR>
    <BR>
      <SPAN CLASS=pout>Reaction Vector: </SPAN>3d vector response (not normalized).<BR>
    <BR>
    <SPAN CLASS=setting>Accuracy: </SPAN>maximum accuracy for object collision.<BR>
    <SPAN CLASS=setting>Touched Objects Group: </SPAN>group filled with all the touched objects when a contact occurs.<BR>
    <SPAN CLASS=setting>Place Optimization: </SPAN>if TRUE, the process will be optimized for levels designed with Places.
      Instead of parsing all the objects of the group, and then performing a bounding box collision test for each of them,
      we parse instead all the places and perform a hierarchical bounding box collision test with those places.
      If the test is TRUE, then we parse the place hierarchy in a recursive way, and for each child of the place,
      we perform again a hierarchical bounding box collision test ... and so on.<BR>
      This method is very efficient if your level is splitted into places, and if each place contain - on average - more than one
      collision object defined in the input group. Because in that way, just a few objects from the group will be tested.<BR>
    <BR>
    */
    /* warning:
    - the "Touched Objects Group" (in settings), is cleared and filled again at each contact events. It isn't clear if no collision occured.
    Therefore you can use this group to retrieve all the touched object, even if the collision occured 5 or 10 frames ago.<BR>
    - you can change the "radius" and the "group" at run-time.<BR>
    - you can use the 'Reaction' output parameter to get information about the normal of the collision (don't forget to normalize if you need a normalized vector),
    or to get the point of collision on the sphere [ contact_point = -Radius * Normalize( Reaction ) ].
    - unlike the 'Layer Slider' building block, the radius isn't proportionnal to the object's bounding sphere. Radius is expressed in absolute world values.<BR>
    - all object in group must be 3d Ojects ( no Groups, no Places no Data Arrays ... etc, only 3d Objects or derived types ).<BR>
    - all object in group must be 1x1x1 scaled.<BR>
    - If you want to teleport your object from place to an other, you should deactivate the building block ( triggers OFF, or deactivate the script ).<BR>
      - Do not check "Place Optimization" setting if you have more places in your level than objects in the group (it would result in a slower process).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetCategory("Collisions/3D Entity");
    od->SetGuid(CKGUID(0x13603db0, 0x16bc321a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectBallSliderProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateObjectBallSliderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Slider");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Contact");
    proto->DeclareOutput("No Contact");

    proto->DeclareInParameter("Radius", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->DeclareOutParameter("Reaction", CKPGUID_VECTOR);

    proto->DeclareSetting("Accuracy", CKPGUID_INT, "0");
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // inner struct OBS
    proto->DeclareSetting("Touched Objects Group", CKPGUID_GROUP);
    proto->DeclareSetting("Place Optimization", CKPGUID_BOOL, FALSE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ObjectBallSlider);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(ObjectBallSliderCallBackObject);

    *pproto = proto;
    return CK_OK;
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR ObjectBallSliderCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        A_OBS *obs = new A_OBS;
        beh->SetLocalParameterValue(1, &obs, sizeof(obs));
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        A_OBS *obs = NULL;
        beh->GetLocalParameterValue(1, &obs);
        if (obs)
        {

            delete obs;
            obs = NULL;
            beh->SetLocalParameterValue(1, &obs);
        }
    }
    break;
    }

    return CKBR_OK;
}

/***********************************************/
/*             Thread Object                   */
/* convenient function to thread               */
/* the ball/object possible collision tests    */
/***********************************************/
inline void ThreadObjectOBS(CK3dEntity *const CurrentEntity, CK3dEntity *const ball,
                            const VxVector &ball_pos, CollBallFace &collballface,
                            const float radius, int &precis_tmp, CKGroup *touched_group,
                            CKBOOL &touched)
{

    //________________________________/ Rejection if = Ball
    if (!CurrentEntity || CurrentEntity == ball)
        return;

    //______________________________/ Rejection by Bounding Cube (in world)
    const VxBbox &box = CurrentEntity->GetBoundingBox();
    if (box.Min.x - radius <= ball_pos.x && box.Max.x + radius >= ball_pos.x &&
        box.Min.y - radius <= ball_pos.y && box.Max.y + radius >= ball_pos.y &&
        box.Min.z - radius <= ball_pos.z && box.Max.z + radius >= ball_pos.z)
    {
        CKMesh *CurrentMesh = CurrentEntity->GetCurrentMesh();

        //________________________________/ Rejection if no mesh
        if (!CurrentMesh)
            return;

        CKBOOL current_object_touched = FALSE;

        VxVector ball_pos_local, dif;

        collballface.UpdateValues(radius, &ball_pos_local, CurrentMesh);
        ball->GetPosition(&ball_pos_local, CurrentEntity);

        VxVector contact;
        collballface.contact = &contact;

        CKWORD *faceIndices = CurrentMesh->GetFacesIndices();

        int vcount = CurrentMesh->GetVertexCount();
        int fcount = CurrentMesh->GetFaceCount();

        if (vcount > MINVERTICES_FORCLASSIFY)
        {
            // we need to classify the vertices

            // Get the vertices
            CKDWORD vStride = 0;
            CKBYTE *vPos = (CKBYTE *)collballface.mesh->GetPositionsPtr(&vStride);

            // Ask CK for a block of preallocated memory for our flags
            unsigned long *vFlags = new unsigned long[vcount];

            // we create the box of the sphere, locally to the object
            VxBbox spherebox;
            spherebox.SetCenter(ball_pos_local, VxVector(radius, radius, radius));

            /////////////////////////
            // The classification
            ////////////////////////

            if (vcount > fcount * 2)
            {
                // Single axis classification

                // we select the maximum axis
                const VxBbox &localBox = CurrentMesh->GetLocalBox();
                VxVector dif = localBox.Max - localBox.Min;
                int axis = 0;
                float max = dif.x;
                if (max < dif.y)
                {
                    max = dif.y;
                    axis = 1;
                }
                if (max < dif.z)
                {
                    max = dif.z;
                    axis = 2;
                }

                spherebox.ClassifyVerticesOneAxis(vcount, vPos, vStride, axis, vFlags);
            }
            else
            {
                // 3 axis classification
                spherebox.ClassifyVertices(vcount, vPos, vStride, vFlags);
            }

            // now we only need to consider the faces not enterely
            // culled by one faces of the spherebox

            for (int f = 0; f < fcount; ++f, faceIndices += 3)
            {
                collballface.face = f;

                if (vFlags[faceIndices[0]] & vFlags[faceIndices[1]] & vFlags[faceIndices[2]]) // culled !
                    continue;

                if (collballface.DetectCollisionWithBallAndFace())
                {
                    dif = ball_pos_local - contact;
                    dif *= radius / Magnitude(dif) - 1.0f;
                    ball->Translate(&dif, CurrentEntity);
                    ball_pos_local += dif;
                    precis_tmp = 0;
                    current_object_touched = TRUE;
                }
            }

            delete[] vFlags;
        }
        else
        {
            // no need to bother the classification

            for (int f = 0; f < fcount; ++f, faceIndices += 3)
            {
                collballface.face = f;

                if (collballface.DetectCollisionWithBallAndFace())
                {
                    dif = ball_pos_local - contact;
                    dif *= radius / Magnitude(dif) - 1.0f;
                    ball->Translate(&dif, CurrentEntity);
                    ball_pos_local += dif;
                    precis_tmp = 0;
                    current_object_touched = TRUE;
                }
            }
        }

        if (current_object_touched)
        {
            if (touched_group)
            {
                if (!touched)
                    touched_group->Clear();
                touched_group->AddObject(CurrentEntity);
            }
            touched = TRUE;
        }
    }
}

/***********************************************/
/*        Parse Hierarchy Child OBS            */
/* convenient function to parse the hierarchy  */
/* and call ThreadObjectOBS if necessary.      */
/* Recursive function                          */
/***********************************************/
void ParseHierarchyChildOBS(CK3dEntity *const CurrentEntity, CK3dEntity *const ball,
                            const VxVector &ball_pos, CollBallFace &collballface,
                            const float radius, int &precis_tmp, CKGroup *touched_group,
                            CKBOOL &touched, CKGroup *const group)
{

    //______________________________/ Rejection by Hierarchical Bounding Cube (in world)
    const VxBbox &box = CurrentEntity->GetHierarchicalBox();
    if (box.Min.x - radius <= ball_pos.x && box.Max.x + radius >= ball_pos.x &&
        box.Min.y - radius <= ball_pos.y && box.Max.y + radius >= ball_pos.y &&
        box.Min.z - radius <= ball_pos.z && box.Max.z + radius >= ball_pos.z)
    {
        int childCount = CurrentEntity->GetChildrenCount();

        CK3dEntity *child;
        while (childCount)
        {

            child = CurrentEntity->GetChild(--childCount);
            if (!child)
                continue;

            if (child->IsInGroup(group))
            {
                ThreadObjectOBS(child, ball, ball_pos, collballface,
                                radius, precis_tmp, touched_group, touched);
            }
            if (child->GetChildrenCount())
            {
                ParseHierarchyChildOBS(child, ball, ball_pos, collballface,
                                       radius, precis_tmp, touched_group, touched, group);
            }
        }
    }
}

/***********************************************/
/*               Main Function                 */
/***********************************************/
int ObjectBallSlider(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    {
        // enter by OFF
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CK3dEntity *ball = (CK3dEntity *)beh->GetTarget();
    if (!ball)
    {
        if (beh->IsInputActive(0))
            beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OWNERERROR;
    }

    A_OBS *obs = NULL; // get local param
    beh->GetLocalParameterValue(1, &obs);

    VxVector init_pos;
    ball->GetPosition(&init_pos);

    if (beh->IsInputActive(0))
    {
        // enter by ON
        beh->ActivateInput(0, FALSE);

        obs->Old_Pos = init_pos;
    }

    float radius = 1.0f; // get radius
    beh->GetInputParameterValue(0, &radius);

    //---
    CKBOOL placeOptim = FALSE; // use portal optimization ?
    beh->GetLocalParameterValue(3, &placeOptim);

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(1);
    if (!group)
        return CKBR_OK;

    CKGroup *touched_group = (CKGroup *)beh->GetLocalParameterObject(2);

    VxVector contact_world;
    VxVector ball_pos, pos;
    CK3dEntity *CurrentEntity;

    CollBallFace collballface;

    int precis = 0; // get accuracy level
    beh->GetLocalParameterValue(0, &precis);
    if (precis < 0)
        precis = 0;

    int precis_tmp = precis;

    VxVector ball_step;
    if (precis)
    {
        precis_tmp = (int)(Magnitude(init_pos - obs->Old_Pos) / (radius - 0.1f));
        if (precis_tmp > precis)
            precis_tmp = precis;
        ball_step = (init_pos - obs->Old_Pos) / (precis_tmp + 1.0f);
        ball_pos = obs->Old_Pos;
    }
    else
    {
        ball_pos = init_pos;
    }

    CKBOOL touched = FALSE;

    //________________________________________
    //--- if we parse a group of object
    if (!placeOptim)
    {
        do
        {
            if (precis)
            {
                ball_pos += ball_step;
                ball->SetPosition(&ball_pos);
            }

            int objCount = group->GetObjectCount();
            for (int i = 0; i < objCount; i++) // For all objects
            {
                CurrentEntity = (CK3dEntity *)group->GetObject(i);

                ThreadObjectOBS(CurrentEntity, ball, ball_pos, collballface,
                                radius, precis_tmp, touched_group, touched);
            }
        } while (precis_tmp--);

        //________________________________________
        //--- if we parse places & hierarchy
    }
    else
    {
        CKContext *ctx = behcontext.Context;

        // get places count (if placeOptim)
        CK_ID *placeID;
        CKPlace *currentPlace;
        int placeCount = ctx->GetObjectsCountByClassID(CKCID_PLACE);
        placeID = ctx->GetObjectsListByClassID(CKCID_PLACE);

        do
        {
            if (precis)
            {
                ball_pos += ball_step;
                ball->SetPosition(&ball_pos);
            }

            for (int a = 0; a < placeCount; ++a)
            {
                currentPlace = (CKPlace *)ctx->GetObject(placeID[a]);

                ParseHierarchyChildOBS(currentPlace, ball, ball_pos, collballface,
                                       radius, precis_tmp, touched_group, touched, group);
            }
        } while (precis_tmp--);
    }

    //--- responce
    VxVector responce;
    ball->GetPosition(&responce);
    responce -= ball_pos;
    beh->SetOutputParameterValue(0, &responce);

    if (precis)
    {
        ball->GetPosition(&obs->Old_Pos);
    }

    if (touched)
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    return CKBR_ACTIVATENEXTFRAME;
}
