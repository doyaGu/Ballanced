#include "N3dGraph.h"

#include "CKAll.h"

char *Network3dName = "Nodal Path";

N3DGraph::N3DGraph(CKContext *context) : m_Size(0), m_AllocatedSize(10)
{
    m_States = new N3DState[m_AllocatedSize];
    m_Display = NULL;
    m_EdgeNumber = 0;
    m_Context = context;
}

N3DGraph::~N3DGraph()
{
    for (int i = 0; i < m_Size; i++)
    {
        N3DEdge *edge = m_States[i].m_Edges;
        N3DEdge *temp;
        while (edge)
        {
            temp = edge;
            edge = edge->m_Next;
            delete temp;
        }
    }
    delete[] m_States;
    m_States = NULL;
}

void N3DGraph::TestAndReallocate(int n)
{
    if (m_Size + n > m_AllocatedSize)
    {
        m_AllocatedSize *= 2;
        N3DState *old = m_States;
        m_States = new N3DState[m_AllocatedSize];
        memcpy(m_States, old, m_Size * sizeof(N3DState));
        delete[] old;
    }
}

int N3DGraph::GetStatesNumber() const
{
    return m_Size;
}

int N3DGraph::GetEdgesNumber() const
{
    return m_EdgeNumber;
}

N3DState *
N3DGraph::InsertState(CK3dEntity *e)
{
    if (!StateSeek(CKOBJID(e)))
    {
        TestAndReallocate();
        m_States[m_Size].m_Edges = NULL;
        m_States[m_Size].m_Data = CKOBJID(e);
        m_Size++;
        return m_States + (m_Size - 1);
    }
    return NULL;
}

N3DEdge *
N3DGraph::InsertEdge(CK3dEntity *s, CK3dEntity *e, float d)
{
    if (s == e)
        return NULL;

    int spos = SeekPosition(CKOBJID(s));
    if (spos == -1)
    { // the first state isn't yet in graph
        InsertState(s);
        spos = m_Size - 1;
    }
    // we check if the destination state is in the graph
    int epos = SeekPosition(CKOBJID(e));
    if (epos == -1)
    {
        InsertState(e);
        epos = m_Size - 1;
    }

    N3DEdge *edge = m_States[spos].m_Edges;
    if (edge)
    {
        while (edge->m_Next)
        {
            // if successor is already there, we didn't do anything
            if (edge->m_Successor == epos)
                return NULL;
            edge = edge->m_Next;
        }
        if (edge->m_Successor == epos)
            return NULL;
        edge->m_Next = CreateEdge(epos, d, GetDistance(GetState(spos), GetState(epos)));
        return edge->m_Next;
    }
    else
    {
        m_States[spos].m_Edges = CreateEdge(epos, d, GetDistance(GetState(spos), GetState(epos)));
        return m_States[spos].m_Edges;
    }
}

void N3DGraph::ActivateEdge(CK3dEntity *s, CK3dEntity *e, CKBOOL a, CKBOOL dyn)
{
    N3DEdge *edge = EdgeSeek(CKOBJID(s), CKOBJID(e));
    if (edge)
    {
        if (dyn)
        {
            if (a)
                edge->m_Active |= STATEDYNAMICACTIVITY;
            else
                edge->m_Active &= ~STATEDYNAMICACTIVITY;
        }
        else
        {
            if (a)
            {
                edge->m_Active |= STATEBASICACTIVITY;
                edge->m_Active |= STATEDYNAMICACTIVITY;
            }
            else
            {
                edge->m_Active &= ~STATEBASICACTIVITY;
                edge->m_Active &= ~STATEDYNAMICACTIVITY;
            }
        }
    }
}

void N3DGraph::ActivateState(CK3dEntity *s, CKBOOL a)
{
    int sp = SeekPosition(CKOBJID(s));
    if (sp < 0)
        return;

    N3DState *state = GetState(sp);
    if (state)
    {
        // first we (de)activate all the leaving edges
        N3DEdge *edge;
        edge = state->m_Edges;
        while (edge)
        {
            if (a)
                edge->m_Active |= STATEDYNAMICACTIVITY;
            else
                edge->m_Active &= ~STATEDYNAMICACTIVITY;
            edge = edge->m_Next;
        }

        // next we iterate all the edges to (de)activate the entering ones
        for (int i = 0; i < m_Size; i++)
        {
            // we have done it already...
            if (i == sp)
                continue;

            edge = m_States[i].m_Edges;
            while (edge)
            {
                if (edge->m_Successor == sp)
                {
                    if (a)
                        edge->m_Active |= STATEDYNAMICACTIVITY;
                    else
                        edge->m_Active &= ~STATEDYNAMICACTIVITY;
                }
                edge = edge->m_Next;
            }
        }
    }
}

CKBOOL
N3DGraph::SetOccupier(CK3dEntity *s, CK3dEntity *o)
{
    N3DState *state = StateSeek(CKOBJID(s));
    if (state)
    {
        CK_ID id = CKOBJID(o);
        if (id)
        {                                                     // we are trying to set an occupier
            if (state->m_Occupier && state->m_Occupier != id) // there is already an occupier : we can't set it
                return FALSE;
            state->m_Occupier = id;
        }
        else
        { // we are trying to remove an occupation
            state->m_Occupier = 0;
        }
        return TRUE;
    }
    return FALSE;
}

CK3dEntity *
N3DGraph::GetOccupier(CK3dEntity *s)
{
    N3DState *state = StateSeek(CKOBJID(s));
    if (state)
    {
        return (CK3dEntity *)m_Context->GetObject(state->m_Occupier);
    }

    return NULL;
}

CKBOOL
N3DGraph::IsEdgeActive(CK3dEntity *s, CK3dEntity *e)
{
    N3DEdge *edge = EdgeSeek(CKOBJID(s), CKOBJID(e));
    if (edge)
    {
        return (edge->m_Active & STATEDYNAMICACTIVITY);
    }
    else
        return false;
}

void N3DGraph::ResetActivity()
{
    N3DEdge *edge;
    for (int i = 0; i < m_Size; i++)
    {
        // we delete the occupation
        m_States[i].m_Occupier = 0;
        // we reset links activity
        edge = m_States[i].m_Edges;
        while (edge)
        {
            if (edge->m_Active & STATEBASICACTIVITY)
                edge->m_Active |= STATEDYNAMICACTIVITY;
            else
                edge->m_Active &= ~STATEDYNAMICACTIVITY;
            edge = edge->m_Next;
        }
    }
}

void N3DGraph::DeleteState(CK_ID e)
{
    int ni = SeekPosition(e);
    if (ni < 0)
        return;
    // we now destroy all the edges leaving the destroyed node
    N3DEdge *edge;
    N3DEdge *backedge;
    edge = m_States[ni].m_Edges;
    while (edge)
    {
        backedge = edge;
        edge = edge->m_Next;
        delete backedge;
        m_EdgeNumber--;
    }

    memmove(m_States + ni, m_States + ni + 1, sizeof(N3DState) * (m_Size - ni - 1));
    int *trans = new int[m_Size];
    int i;
    for (i = 0; i < m_Size; i++)
    {
        if (i < ni)
            trans[i] = i;
        else if (i == ni)
            trans[i] = -1;
        else
            trans[i] = i - 1;
    }
    m_Size--;

    // now we iterate all the edges to decal the indices
    for (i = 0; i < m_Size; i++)
    {
        edge = m_States[i].m_Edges;
        backedge = NULL;
        while (edge)
        {
            edge->m_Successor = trans[edge->m_Successor];
            if (edge->m_Successor == -1)
            {
                if (backedge)
                {
                    N3DEdge *temp = backedge->m_Next;
                    backedge->m_Next = edge->m_Next;
                    edge = edge->m_Next;
                    delete temp;
                    m_EdgeNumber--;
                }
                else
                {
                    N3DEdge *temp = edge;
                    m_States[i].m_Edges = edge->m_Next;
                    edge = edge->m_Next;
                    delete temp;
                    m_EdgeNumber--;
                }
            }
            else
            {
                backedge = edge;
                edge = edge->m_Next;
            }
        }
    }
    delete[] trans;
}

void N3DGraph::DeleteEdge(CK3dEntity *s, CK3dEntity *e)
{
    int si = SeekPosition(CKOBJID(s));
    int ei = SeekPosition(CKOBJID(e));

    N3DEdge *edge;
    N3DEdge *backedge;
    edge = m_States[si].m_Edges;
    backedge = NULL;
    while (edge)
    {
        if (edge->m_Successor == ei)
        {
            if (backedge)
            {
                N3DEdge *temp = backedge->m_Next;
                backedge->m_Next = edge->m_Next;
                edge = edge->m_Next;
                delete temp;
                m_EdgeNumber--;
            }
            else
            {
                N3DEdge *temp = edge;
                m_States[si].m_Edges = edge->m_Next;
                edge = edge->m_Next;
                delete temp;
                m_EdgeNumber--;
            }
        }
        else
        {
            backedge = edge;
            edge = edge->m_Next;
        }
    }
}

N3DEdge *
N3DGraph::EdgeSeek(CK_ID s, CK_ID e)
{
    int si = SeekPosition(s);
    int ei = SeekPosition(e);

    if (si < 0 || ei < 0)
        return NULL;

    N3DEdge *edge;
    edge = m_States[si].m_Edges;
    while (edge)
    {
        if (edge->m_Successor == ei)
        {
            return edge;
        }
        else
        {
            edge = edge->m_Next;
        }
    }
    return NULL;
}

N3DState *
N3DGraph::StateSeek(CK_ID s)
{
    for (int i = 0; i < m_Size; i++)
    {
        if (m_States[i].m_Data == s)
            return m_States + i;
    }
    return NULL;
}

N3DEdge *
N3DGraph::CreateEdge(int e, float d, float dist)
{
    N3DEdge *edge = new N3DEdge;
    m_EdgeNumber++;
    edge->m_Active = STATEBASICACTIVITY | STATEDYNAMICACTIVITY;
    edge->m_Next = NULL;
    edge->m_Successor = e;
    edge->m_Difficulty = d;
    return edge;
}

N3DEdge *
N3DGraph::GetSuccessorsList(int i)
{
    return m_States[i].m_Edges;
}

int N3DGraph::SeekPosition(CK_ID e) const
{
    for (int i = 0; i < m_Size; i++)
    {
        if (m_States[i].m_Data == e)
            return i;
    }
    return -1;
}

N3DState *
N3DGraph::GetState(int i)
{
    return m_States + i;
}

typedef XList<N3DState *> nodelist;
typedef XListIt<N3DState *> nodelistit;

CK3dEntity *
N3DGraph::FindPath(CK3dEntity *start, CK3dEntity *goal, CKGroup *path, CKBOOL basic, CKBOOL occupation)
{
    CKDWORD activity;
    if (basic)
        activity = STATEBASICACTIVITY;
    else
        activity = STATEDYNAMICACTIVITY;

    nodelist open;
    VxVector a, b, c;

    N3DState *startnode = StateSeek(CKOBJID(start));
    if (!startnode)
        return NULL;
    N3DState *goalnode = StateSeek(CKOBJID(goal));
    if (!goalnode)
        return NULL;

    startnode->m_AStar.g = 0;
    startnode->m_AStar.h = GetDistance(startnode, goalnode);
    startnode->m_AStar.f = startnode->m_AStar.g + startnode->m_AStar.h;
    startnode->m_AStar.parent = NULL;

    PushOnOpen(open, startnode);
    while (open.Size())
    {
        nodelistit first = open.Begin();
        N3DState *n = *first;
        open.Remove(first);
        n->m_Flag &= ~LISTOPEN;

        if (n == goalnode)
        {
            CK3dEntity *nodetogo = NULL;
            if (path)
            { // we construct an entire path
                // parcourir n en remontant les parent et ajouter les m_Data au path
                while (n->m_AStar.parent && (n != n->m_AStar.parent))
                {
                    path->AddObjectFront((CKBeObject *)m_Context->GetObject(n->m_Data));
                    n = n->m_AStar.parent;
                }
                nodetogo = (CK3dEntity *)path->GetObject(0);
                path->AddObjectFront((CKBeObject *)m_Context->GetObject(n->m_Data));
            }
            else
            {
                // parcourir n en remontant les parent et ajouter les m_Data au path
                CK_ID idtogo = 0;
                while (n->m_AStar.parent && (n != n->m_AStar.parent))
                {
                    idtogo = n->m_Data;
                    n = n->m_AStar.parent;
                }
                nodetogo = (CK3dEntity *)m_Context->GetObject(idtogo);
            }

            // now we must clear the flags
            for (int i = 0; i < m_Size; i++)
            {
                m_States[i].m_Flag = 0;
            }

            // and then delete the list
            // maybe todo : clear

            // in any case, we return the first node to go
            return nodetogo;
        }
        N3DEdge *edge = n->m_Edges;
        N3DState *n_;
        float newg;
        while (edge)
        {
            if (edge->m_Active & activity)
            {
                n_ = m_States + edge->m_Successor;
                if (!occupation || !n_->m_Occupier)
                {
                    // newg = n->m_AStar.g + GetDistance(n,n_);
                    newg = n->m_AStar.g + edge->m_Difficulty * edge->m_Distance;
                    if (n_->m_Flag)
                    {
                        if (n_->m_AStar.g <= newg)
                        {
                            edge = edge->m_Next;
                            continue;
                        }
                    }
                    n_->m_AStar.parent = n;
                    n_->m_AStar.g = newg;
                    n_->m_AStar.h = GetDistance(n_, goalnode);
                    n_->m_AStar.f = n_->m_AStar.g + n_->m_AStar.h;
                    // remove n' if present in closed
                    n_->m_Flag &= ~LISTCLOSE;
                    // add n_ in open if it is not yet in it
                    if (!(n_->m_Flag & LISTOPEN))
                    {
                        PushOnOpen(open, n_);
                    }
                }
            }
            // get the next successor
            edge = edge->m_Next;
        }
        // closed->InsertRear(n);
        n->m_Flag |= LISTCLOSE;
    }

    // now we must clear the flags
    for (int i = 0; i < m_Size; i++)
    {
        m_States[i].m_Flag = 0;
    }

    // we then delete the array

    return NULL;
}

void N3DGraph::PushOnOpen(nodelist &open, N3DState *n)
{
    n->m_Flag |= LISTOPEN;
    nodelistit first = open.Begin();
    nodelistit last = open.End();

    while (first != last)
    {
        if (n->m_AStar.f < (*first)->m_AStar.f)
        {
            open.Insert(first, n);
            return;
        }
        first++;
    }

    open.Insert(first, n);
}

float N3DGraph::GetDistance(N3DState *n1, N3DState *n2)
{
    return Magnitude(n1->m_Position - n2->m_Position);
}

CK3dEntity *
N3DGraph::GetSafeEntity(N3DState *s)
{
    CK3dEntity *ent = (CK3dEntity *)m_Context->GetObject(s->m_Data);
    if (!ent)
    {
        DeleteState(s->m_Data);
    }
    return ent;
}

void N3DGraph::CreateFromChunk(CKStateChunk *chunk)
{
    chunk->StartRead();
    if (!chunk->SeekIdentifier(N3DGRAPHSTART_IDENTIFIER))
        return;

    int version = chunk->ReadInt();
    if ((version != N3DGRAPH_VERSION1) && (version != N3DGRAPH_VERSION2))
        return;

    m_Size = chunk->ReadInt();
    delete[] m_States;
    if (m_Size)
    {
        m_States = new N3DState[m_Size];
        m_AllocatedSize = m_Size;
    }
    else
    { // size = 0
        m_States = new N3DState[1];
        m_AllocatedSize = 1;
    }

    for (int i = 0; i < m_Size; i++)
    {
        // state data
        m_States[i].m_Data = chunk->ReadObjectID();

        if (version == N3DGRAPH_VERSION2)
        {
            m_States[i].m_Occupier = chunk->ReadObjectID();
        }
        else
            m_States[i].m_Occupier = 0;

        // edges number
        int size = chunk->ReadInt();

        N3DEdge *edge;
        N3DEdge *oldedge = NULL;
        N3DEdge *firstedge = NULL;
        for (int j = 0; j < size; j++)
        {
            edge = CreateEdge(0, 0, 0.0f);
            if (!oldedge)
            {
                firstedge = edge;
                oldedge = edge;
            }
            else
            {
                oldedge->m_Next = edge;
                oldedge = edge;
            }
            edge->m_Active = chunk->ReadInt();
            edge->m_Difficulty = chunk->ReadFloat();
            edge->m_Successor = chunk->ReadInt();
        }
        m_States[i].m_Edges = firstedge;
    }

    // We clean everything
    Clean();
    // we calculate the distance
    UpdateDistance();
}

void N3DGraph::Clean()
{
    // we check if the nodes have themselves as successors = not permitted
    int i;
    for (i = 0; i < m_Size; i++)
    {
        N3DEdge *edge = m_States[i].m_Edges;
        N3DEdge *backedge = NULL;
        while (edge)
        {
            if (edge->m_Successor == i)
            { // Self edge : achtung
                if (backedge)
                {
                    N3DEdge *temp = backedge->m_Next;
                    backedge->m_Next = edge->m_Next;
                    edge = edge->m_Next;
                    delete temp;
                    m_EdgeNumber--;
                }
                else
                {
                    N3DEdge *temp = edge;
                    m_States[i].m_Edges = edge->m_Next;
                    edge = edge->m_Next;
                    delete temp;
                    m_EdgeNumber--;
                }
            }
            else
            {
                backedge = edge;
                edge = edge->m_Next;
            }
        }
    }
}

void N3DGraph::SaveToChunk(CKStateChunk *chunk)
{
    Clean();

    chunk->StartWrite();
    // start identifier
    chunk->WriteIdentifier(N3DGRAPHSTART_IDENTIFIER);
    // version
    chunk->WriteInt(N3DGRAPH_VERSION2);
    // size
    chunk->WriteInt(m_Size);
    // state size
    int i;
    for (i = 0; i < m_Size; i++)
    {
        chunk->WriteObject(m_Context->GetObject(m_States[i].m_Data));
        chunk->WriteObject(m_Context->GetObject(m_States[i].m_Occupier));
        int count = 0;
        N3DEdge *edge = m_States[i].m_Edges;
        while (edge)
        {
            edge = edge->m_Next;
            count++;
        }

        chunk->WriteInt(count);
        edge = m_States[i].m_Edges;
        while (edge)
        {
            chunk->WriteInt(edge->m_Active);
            chunk->WriteFloat(edge->m_Difficulty);
            chunk->WriteInt(edge->m_Successor);
            edge = edge->m_Next;
        }
    }
    // end identifier
    chunk->CloseChunk();
}

void N3DGraph::UpdateDistance()
{
    int i;
    for (i = 0; i < m_Size; i++)
    {
        VxVector a;
        CK3dEntity *ent;
        ent = GetSafeEntity(m_States + i);
        if (ent)
            ent->GetPosition(&a);
        m_States[i].m_Position = a;
    }
    for (i = 0; i < m_Size; i++)
    {
        N3DEdge *edge = m_States[i].m_Edges;
        while (edge)
        {
            edge->m_Distance = GetDistance(GetState(i), GetState(edge->m_Successor));
            edge = edge->m_Next;
        }
    }
}

void GraphRender(CKRenderContext *rc, void *arg)
{
    VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, 3);
    CKWORD *indices = rc->GetDrawPrimitiveIndices(3);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector4 *positions = (VxVector4 *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxVector4 *positions = (VxVector4 *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    VxVector pos;
    VxVector nodepos, nodepos2;
    VxVector4 hompos;

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    VxVector v0(0, 3, 0);
    VxVector v1(3, -3, 0);
    VxVector v2(-3, -3, 0);

    int width = rc->GetWidth();
    int height = rc->GetHeight();

    colors[0] = RGBAFTOCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    VxRect ScreenSize;
    rc->GetViewRect(ScreenSize);
    float WidthOn2 = ScreenSize.GetWidth() * 0.5f;
    float HeightOn2 = ScreenSize.GetHeight() * 0.5f;
    float DevX = ScreenSize.left + WidthOn2;
    float DevY = ScreenSize.top + HeightOn2;

    v0.x *= 0.5f / WidthOn2;
    v1.x *= 0.5f / WidthOn2;
    v2.x *= 0.5f / WidthOn2;

    v0.y *= 0.5f / HeightOn2;
    v1.y *= 0.5f / HeightOn2;
    v2.y *= 0.5f / HeightOn2;

    VxMatrix View = rc->GetViewTransformationMatrix();
    VxMatrix Projection = rc->GetProjectionTransformationMatrix();
    // A custom projection matrix to render homogenous coordinates
    rc->SetWorldTransformationMatrix(VxMatrix::Identity());
    rc->SetViewTransformationMatrix(VxMatrix::Identity());

    N3DGraph *graph = (N3DGraph *)arg;
    if (!graph)
        return;
    N3DDisplayStructure *display = graph->GetDisplay();

    XVoidArray *extents = graph->GetExtentsList();
    if (extents)
    {
        for (void **ptr = extents->Begin(); ptr != extents->End(); ++ptr)
            delete (N3DExtent *)*ptr;
        extents->Clear();
    }

    N3DEdge *edge;
    N3DEdge *edgeend;
    N3DState *start;
    N3DState *end;
    CK3dEntity *startent;
    CK3dEntity *endent;
    int i;
    rc->SetTexture(NULL);
    rc->SetState(VXRENDERSTATE_ALPHATESTENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_ZENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
    rc->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);

    for (i = 0; i < graph->GetStatesNumber(); i++)
    {
        start = graph->GetState(i);
        edge = start->m_Edges;
        startent = graph->GetSafeEntity(start);
        if (!startent)
        {
            continue;
        }

        startent->GetPosition(&pos);

        // Transform Pos to Node_Pos & Hom_pos (ClipFlags is 0 if the vertex is inside the view frustrum)
        Vx3DMultiplyMatrixVector(&nodepos, View, &pos);
        Vx3DMultiplyMatrixVector4(&hompos, Projection, &nodepos);
        CKBOOL Clipped = FALSE;
        if ((hompos.x < -hompos.w) || (hompos.x > hompos.w) ||
            (hompos.y < -hompos.w) || (hompos.y > hompos.w) ||
            (hompos.z < 0) || (hompos.z > hompos.w))
        {
            Clipped = TRUE;
        }

        // First we draw the node
        data->VertexCount = 3;
        if (!Clipped)
        { // is the node in viewport
            // we can then add it to the Node Extents list
            if (extents)
            {
                N3DExtent *ext = new N3DExtent;
                VxVector4 screenpos = hompos;
                screenpos.w = 1.0f / screenpos.w;
                screenpos.x = (screenpos.x * screenpos.w * WidthOn2) + DevX;
                screenpos.y = -(screenpos.y * screenpos.w * HeightOn2) + DevY;

                ext->m_Node = start->m_Data;
                ext->m_Rect.left = (long)screenpos.x - 6;
                ext->m_Rect.right = (long)screenpos.x + 6;
                ext->m_Rect.top = (long)screenpos.y - 6;
                ext->m_Rect.bottom = (long)screenpos.y + 6;
                extents->PushBack((void *)ext);
            }

            if (display && ((start->m_Data == display->m_Node) || (start->m_Data == display->m_EndNode)))
            { // this node is the selected one or the ending one
                positions[0] = nodepos + 2 * v0 * nodepos.z;
                positions[1] = nodepos + 2 * v1 * nodepos.z;
                positions[2] = nodepos + 2 * v2 * nodepos.z;
                colors[0] = colors[1] = colors[2] = RGBAFTOCOLOR(1.0f, 0, 1.0f, 0.6f);
            }
            else
            {
                positions[0] = nodepos + v0 * nodepos.z;
                positions[1] = nodepos + v1 * nodepos.z;
                positions[2] = nodepos + v2 * nodepos.z;
                if (start->m_Occupier)
                {
                    colors[0] = colors[1] = colors[2] = RGBAFTOCOLOR(1.0f, 0.0f, 0.0f, 0.6f);
                }
                else
                {
                    colors[0] = colors[1] = colors[2] = RGBAFTOCOLOR(0.3f, 0.3f, 1.0f, 0.6f);
                }
            }
            // we actually draw it
            rc->DrawPrimitive(VX_TRIANGLELIST, indices, 3, data);
        }

        // Next we draw the edges
        data->VertexCount = 2;
        while (edge)
        {
            CKBOOL MustDraw = TRUE;
            end = graph->GetState(edge->m_Successor);
            edgeend = end->m_Edges;

            if (edge->m_Active & STATEDYNAMICACTIVITY)
            {
                colors[0] = RGBAFTOCOLOR(0, 1.0f, 0, 0.6f);
            }
            else
            {
                colors[0] = RGBAFTOCOLOR(1.0f, 0, 0, 0.6f);
            }
            colors[1] = RGBAFTOCOLOR(0, 0, 1.0f, 0.6f);
            while (edgeend)
            {
                if (edgeend->m_Successor == i)
                {
                    if (i > edge->m_Successor)
                        MustDraw = FALSE;
                    else
                    {
                        if (edge->m_Active & STATEDYNAMICACTIVITY)
                        {
                            colors[0] = RGBAFTOCOLOR(0, 1.0f, 0.0f, 0.6f);
                        }
                        else
                        {
                            colors[0] = RGBAFTOCOLOR(1.0f, 0, 0, 0.6f);
                        }
                        if (edgeend->m_Active & STATEDYNAMICACTIVITY)
                        {
                            colors[1] = RGBAFTOCOLOR(0, 1.0f, 0, 0.6f);
                        }
                        else
                        {
                            colors[1] = RGBAFTOCOLOR(1.0f, 0, 0, 0.6f);
                        }
                    }
                    break;
                }
                edgeend = edgeend->m_Next;
            }
            if (MustDraw)
            {
                endent = graph->GetSafeEntity(end);
                if (!endent)
                    return;
                endent->GetPosition(&pos);
                Vx3DMultiplyMatrixVector(&nodepos2, View, &pos);
                positions[0] = nodepos;
                positions[1] = nodepos2;
                rc->DrawPrimitive(VX_LINELIST, indices, 2, data);
            }

            edge = edge->m_Next;
        }
    }

    // We now draw the path selected
    data->VertexCount = 3;
    CKGroup *group = graph->GetPath();
    if (group)
    {
        int count = group->GetObjectCount();
        for (i = 1; i < count; i++)
        {
            // we draw the line from the past one to the current one
            startent = (CK3dEntity *)group->GetObject(i - 1);
            endent = (CK3dEntity *)group->GetObject(i);

            startent->GetPosition(&pos);
            Vx3DMultiplyMatrixVector(&nodepos, View, &pos);
            endent->GetPosition(&pos);
            Vx3DMultiplyMatrixVector(&nodepos2, View, &pos);
            positions[2] = nodepos2;

            VxVector d = Normalize(nodepos2 - nodepos);
            d.x *= nodepos.z * 0.5f / WidthOn2;
            d.y *= nodepos.z * 0.5f / HeightOn2;
            VxVector n(-d.y * 3.0f, d.x * 3.0f, 0);
            positions[0] = nodepos + n;
            positions[1] = nodepos - n;

            colors[0] = colors[1] = colors[2] = RGBAFTOCOLOR(1.0f, 0.0f, 1.0f, 0.6f);
            // we actually draw it
            rc->DrawPrimitive(VX_TRIANGLELIST, indices, 3, data);
        }
    }

    rc->SetViewTransformationMatrix(View);
}
