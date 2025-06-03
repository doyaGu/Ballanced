/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CharacterGoToNode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "N3dGraph.h"

#define CKPGUID_FRONTAL_DIRECTION CKDEFINEGUID(0x286652d, 0x5ea709c2)

CKERROR CreateCharacterGoToNodeBehaviorProto(CKBehaviorPrototype **);
int DoCharacterGoToNode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGoToNodeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Character Go To Node");
    od->SetDescription("Makes the target character walk to the selected node, by taking the shortest path possible.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <SPAN CLASS=in>Stop: </SPAN>deactivates the building block and free the links the character was currently occupying.<BR>
    <BR>
    <SPAN CLASS=out>Arrived:  </SPAN>is activated when the distance between the character and the "Target Object" is lower than the "Distance" input parameter.<BR>
    <SPAN CLASS=out>Blocked: </SPAN>is activated if the character is unable to get to the selected node.<BR>
    <SPAN CLASS=out>Loop Out:  </SPAN>is activated when the process needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>nodal path to go through.<BR>
    <SPAN CLASS=pin>Target Object: </SPAN>destination to reach.<BR>
    <SPAN CLASS=pin>Guard Distance: </SPAN>distance just before destination node where character should stop.<BR>
    <SPAN CLASS=pin>Character Direction: </SPAN>defines the character's frontal direction.<BR>
    <SPAN CLASS=pin>Limit Angle: </SPAN>limits the angle between character's direction and destination node. Value should not be less than 20 degrees.<BR>
    <SPAN CLASS=pin>Mark Occupation: </SPAN>if TRUE, node or link character currently occupied is marked as
    occupied, and no other character can use it.<BR>
    <BR>
    <SPAN CLASS=setting>Turn With Messages: </SPAN>character will respond to messages from a character controller (e.g. Joystick) if this checkbox is crossed.<BR>
    <SPAN CLASS=setting>Rotation Speed: </SPAN>speed at which character turns, if this is not handled by messages. Range is between 1 = slow, and 100 = instantaneous.<BR>
    */
    od->SetCategory("3D Transformations/Nodal Path");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c710f65, 0x32ed4ea6));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCharacterGoToNodeBehaviorProto);
    od->SetCompatibleClassId(CKCID_CHARACTER);
    return od;
}

CKERROR DaCharacterGoToNodeCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = beh->GetCKContext();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORREADSTATE:
    {
        CKMessageManager *mm = ctx->GetMessageManager();

        // TODO : mettre les messages en settings....
        int tab[4];
        tab[0] = mm->AddMessageType("Joy_Up");
        tab[1] = mm->AddMessageType("Joy_Down");
        tab[2] = mm->AddMessageType("Joy_Left");
        tab[3] = mm->AddMessageType("Joy_Right");

        beh->SetLocalParameterValue(0, tab, 4 * sizeof(int));

        return CKBR_OK;
    }

    break;

    case CKM_BEHAVIORDELETE:
    {
        CKDWORD d = 0;
        beh->SetLocalParameterValue(0, &d, sizeof(CKDWORD));

        return CKBR_OK;
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateCharacterGoToNodeBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Character Go To Node");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Arrived");
    proto->DeclareOutput("Can't Go");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);
    proto->DeclareInParameter("Goal Node", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Guard Distance", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Character Direction", CKPGUID_FRONTAL_DIRECTION, "X");
    proto->DeclareInParameter("Limit Angle", CKPGUID_ANGLE, "0:70");
    proto->DeclareInParameter("Mark Occupation", CKPGUID_BOOL, "FALSE");

    // Message array
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);
    // node to go
    proto->DeclareLocalParameter(NULL, CKPGUID_3DENTITY);
    // last node
    proto->DeclareLocalParameter(NULL, CKPGUID_3DENTITY);

    proto->DeclareSetting("Turn With Messages", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Rotation Speed", CKPGUID_PERCENTAGE, "100");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetFunction(DoCharacterGoToNode);
    proto->SetBehaviorCallbackFct(DaCharacterGoToNodeCB);
    *pproto = proto;
    return CK_OK;
}

int DoCharacterGoToNode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKCharacter *caracter = (CKCharacter *)beh->GetTarget();
    if (!caracter)
    {
        beh->ActivateOutput(1);
        return CKBR_OWNERERROR;
    }
    CKContext *ctx = behcontext.Context;
    CKAttributeManager *attman = ctx->GetAttributeManager();

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        throw "No group given";
    CKParameterOut *param = group->GetAttributeParameter(attman->GetAttributeTypeByName(Network3dName));
    if (!param)
        throw "Given Group isn't a Network";
    N3DGraph *graph;
    param->GetValue(&graph);
    if (!graph)
        throw "There is no Graph attached";

    CKBOOL occupation = FALSE;
    beh->GetInputParameterValue(5, &occupation);

    // we get the character position
    VxVector carpos;
    CK3dEntity *carac = caracter->GetFloorReferenceObject();
    if (!carac)
    { // the character doesn't possess a reference object : we use the root
        carac = caracter->GetRootBodyPart();
        if (!carac)
            return CKBR_OWNERERROR;
    }
    carac->GetPosition(&carpos);
    carpos.y = 0;

    // we get the goal node
    CK3dEntity *goalnode = (CK3dEntity *)beh->GetInputParameterObject(1);

    if (beh->IsInputActive(2))
    { // We entered by stop
        beh->ActivateInput(1, FALSE);
        beh->ActivateInput(2, FALSE);

        CK3dEntity *nodetogo = (CK3dEntity *)beh->GetLocalParameterObject(1);
        CK3dEntity *oldnode = (CK3dEntity *)beh->GetLocalParameterObject(2);

        // we reactivates links currently occupied
        if (oldnode && nodetogo)
        {
            graph->ActivateEdge(oldnode, nodetogo, TRUE);
            graph->ActivateEdge(nodetogo, oldnode, TRUE);
        }

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        CK3dEntity *oldnode = (CK3dEntity *)beh->GetLocalParameterObject(2);
        if (oldnode && occupation)
        { // we have to check if we are far enough away from the old node
            graph->SetOccupier(oldnode, NULL);
        }

        beh->ActivateInput(0, FALSE);

        // we find the first node where we must go
        // for now the node the nearest
        // TODO : find a best method (cone of vision...)
        VxVector pos;

        CK3dEntity *node;
        CK3dEntity *nearestnode;
        float dist, distmin = 100000000000000000.0f;

        for (int i = 0; i < graph->GetStatesNumber(); i++)
        {
            node = (CK3dEntity *)graph->GetSafeEntity(graph->GetState(i));
            node->GetPosition(&pos);
            pos.y = 0;

            // TODO :optimize this line may be
            dist = SquareMagnitude(carpos - pos);
            if (dist < distmin)
            {
                // we test if it is a valid node
                if ((goalnode == node) || graph->FindPath(node, goalnode, NULL, TRUE))
                {
                    distmin = dist;
                    nearestnode = node;
                }
            }
        }

        if (!nearestnode)
        {
            beh->ActivateOutput(1);
            return CKBR_OK;
        }

        beh->SetLocalParameterObject(1, nearestnode);
        beh->SetLocalParameterObject(2, NULL);
        beh->ActivateOutput(2);
        return CKBR_OK;
    }

    // so we're in by the loop in input
    beh->ActivateInput(1, FALSE);

    float prox = 1.0f;
    beh->GetInputParameterValue(2, &prox);

    CK3dEntity *oldnode = (CK3dEntity *)beh->GetLocalParameterObject(2);
    if (oldnode)
    { // we have to check if we are far enough away from the old node
        VxVector nodepos;
        oldnode->GetPosition(&nodepos);
        VxVector dist = nodepos - carpos;
        dist.y = 0.0f;

        float distance = Magnitude(dist);
        if (occupation && distance > prox)
        { // we can deoccupy it
            graph->SetOccupier(oldnode, NULL);
        }
    }

    CK3dEntity *nodetogo = (CK3dEntity *)beh->GetLocalParameterObject(1);
    if (!nodetogo)
        return CKBR_PARAMETERERROR;

    // if the nodetogo is currently occupied
    if (oldnode && occupation)
    {
        CK3dEntity *occ;
        if (occ = graph->GetOccupier(nodetogo))
        {
            if (occ != caracter)
            { // it's not the character itself...
                if (nodetogo != goalnode)
                {                       // it's not the final node so we can go another way
                    nodetogo = oldnode; // graph->FindPath(oldnode,goalnode,NULL,TRUE,nodetogo);
                }
            }
        }
    }

    VxVector nodepos;
    nodetogo->GetPosition(&nodepos);
    VxVector dist = nodepos - carpos;
    dist.y = 0.0f;

    float distance = Magnitude(dist);

    if (distance < prox)
    {
        // we leave a link : we must reactivate it
        if (oldnode && occupation)
        {
            graph->ActivateEdge(oldnode, nodetogo, TRUE);
            graph->ActivateEdge(nodetogo, oldnode, TRUE);
        }

        if (nodetogo == goalnode)
        { // we are arrived
            beh->ActivateOutput(0);
            return CKBR_OK;
        }

        // we reached a node : we try to occupy it
        if (occupation && !graph->SetOccupier(nodetogo, caracter))
        {
            // the node is already occupied...
            // we do nothing but wait
            beh->ActivateOutput(2);
            return CKBR_OK;
        }

        // the old node to go is now the old node
        oldnode = nodetogo;
        beh->SetLocalParameterObject(2, oldnode);

        // we must find the next node to go
        nodetogo = graph->FindPath(oldnode, goalnode, NULL);
        if (!nodetogo)
        { // no possibility to go to the end node
            nodetogo = graph->FindPath(oldnode, goalnode, NULL, TRUE);
            if (!nodetogo)
            { // there is really no possibility to go there
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
            else
            { // I must go to nodetogo, but can I ?
                if (!graph->IsEdgeActive(oldnode, nodetogo))
                    nodetogo = oldnode;
            }
        }

        // if the nodetogo is currently occupied
        if (occupation)
        {
            CK3dEntity *occ;
            if (occ = graph->GetOccupier(nodetogo))
            {
                if (occ != caracter)
                { // it's not the character itself...
                    if (nodetogo != goalnode)
                    { // it's not the final node so we can go another way
                        nodetogo = graph->FindPath(oldnode, goalnode, NULL, TRUE, TRUE);
                        if (!nodetogo)
                            nodetogo = oldnode;
                        if (!graph->IsEdgeActive(oldnode, nodetogo))
                            nodetogo = oldnode;
                    }
                }
            }
        }

        // we disable the link that we're going to walk on
        if (occupation)
        {
            graph->ActivateEdge(oldnode, nodetogo, FALSE);
            graph->ActivateEdge(nodetogo, oldnode, FALSE);
        }

        nodetogo->GetPosition(&nodepos);

        dist = nodepos - carpos;
        dist.y = 0.0f;
        distance = Magnitude(dist);

        beh->SetLocalParameterObject(1, nodetogo);
    }

    // we now walk to the node to go
    int *tab = (int *)beh->GetLocalParameterReadDataPtr(0);

    VxVector dirx, dir;
    float angle, angle1;

    int CharacterDirection = 1;
    beh->GetInputParameterValue(3, &CharacterDirection);

    switch (CharacterDirection)
    {
    case 1:
        caracter->GetOrientation(&dirx, NULL, &dir);
        dirx = -dirx;
        break;
    case 2:
    {
        caracter->GetOrientation(&dirx, NULL, &dir);
        dir = -dir;
    }
    break;
    case 3:
        caracter->GetOrientation(NULL, &dir, &dirx);
        break;
    case 4:
    {
        caracter->GetOrientation(NULL, &dir, &dirx);
        dir = -dir;
        dirx = -dirx;
    }
    break;
    case 5:
        caracter->GetOrientation(&dir, NULL, &dirx);
        break;
    case 6:
    {
        caracter->GetOrientation(&dir, NULL, &dirx);
        dir = -dir;
        dirx = -dirx;
    }
    break;
    }

    VxVector up;
    up = CrossProduct(dir, dirx);

    dist /= distance;

    angle1 = DotProduct(dir, dist);
    angle = DotProduct(dirx, dist);

    float anglelimit = 1.22f;
    beh->GetInputParameterValue(4, &anglelimit);
    float anglelimitcos = cosf(anglelimit);

    CKMessageManager *mm = behcontext.Context->GetMessageManager();

    if (distance > prox)
    {
        if (angle1 > anglelimitcos)
        {
            mm->SendMessageSingle(tab[0], carac, carac);
        }
    }

    //	if (angle<0.02f) CKSendMessageSingle(tab[3],carac,carac);
    //	else if (angle>0.02f) CKSendMessageSingle(tab[2],carac,carac);
    CKBOOL twm = TRUE;
    beh->GetLocalParameterValue(3, &twm);
    float speed = 1.0f;
    beh->GetLocalParameterValue(4, &speed);

    if (twm)
    {
        if (angle1 < 0.999f)
        {
            if (angle < 0.0f)
            {
                mm->SendMessageSingle(tab[2], carac, carac);
            }
            else
            {
                mm->SendMessageSingle(tab[3], carac, carac);
            }
        }
    }
    else
    {
        if (angle1 < 0.999f)
        {
            caracter->Rotate(&up, speed * -angle);
            /*			if (angle<0.0f) {
                            carac->Rotate(&up,angle);
                        } else	{

                        }
            */
        }
    }

    beh->ActivateOutput(2);
    return CKBR_OK;
}
