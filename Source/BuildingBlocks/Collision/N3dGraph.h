#ifndef N3DGRAPH_H
#define N3DGRAPH_H

#include <CK3dEntity.h>
#include <CKGroup.h>

#pragma warning(disable : 4786)

#include <XList.h>

#define N3DGRAPHSTART_IDENTIFIER    2000666
#define N3DGRAPH_VERSION1           1
#define N3DGRAPH_VERSION2           2

#define STATEBASICACTIVITY          1
#define STATEDYNAMICACTIVITY        2

#define LISTNONE                    0
#define LISTOPEN                    1
#define LISTCLOSE                   2

struct N3DEdge;
struct N3DState;

struct N3DAStar
{
    N3DAStar() : g(0.0f), h(0.0f), f(0.0f), parent(NULL) { ; }
    // cheapest cost of arriving to node n
    float g;
    // heuristic estimate of the cost to the goal
    float h;
    // cost of node n
    float f;
    // index of the parent state
    N3DState *parent;
};

struct N3DState
{
    N3DState() : m_Edges(NULL), m_Data(0), m_Occupier(0), m_Flag(0) {};
    // list of transitions
    N3DEdge *m_Edges;
    // Pointer on Associated Entity
    CK_ID m_Data;
    // Pointer on Associated Entity
    CK_ID m_Occupier;
    // AStar structure
    N3DAStar m_AStar;
    // List flag : LISTOPEN, LISTCLOSE or LISTNONE
    CKDWORD m_Flag;
    // Node Position
    VxVector m_Position;
};

struct N3DExtent
{
    // Node ID
    CK_ID m_Node;
    // Node Extent
    CKRECT m_Rect;
};

struct N3DEdge
{
    N3DEdge() : m_Successor(-1), m_Active(TRUE), m_Difficulty(0), m_Next(NULL) {};
    // state successor
    int m_Successor;
    // activity
    int m_Active;
    // terrain difficulty
    float m_Distance;
    // terrain difficulty
    float m_Difficulty;
    // Next Edge
    N3DEdge *m_Next;
};

struct N3DDisplayStructure
{
    N3DDisplayStructure(CKContext *context) : m_Node(0), m_EndNode(0)
    {

        m_Group = (CKGroup *)context->CreateObject(CKCID_GROUP, "Graph Path");
    }
    ~N3DDisplayStructure()
    {
        for (void **ptr = m_NodeExtents.Begin(); ptr != m_NodeExtents.End(); ptr++)
            delete (N3DExtent *)*ptr;

        CKDestroyObject(m_Group);
    }
    // Nodes extents
    XVoidArray m_NodeExtents;
    // group of nodes to go
    CKGroup *m_Group;
    // node selected
    CK_ID m_Node;
    // end node for the link selected
    CK_ID m_EndNode;
};

class N3DGraph
{
public:
    // Constructor
    N3DGraph(CKContext *context);
    ~N3DGraph();

    // Cleanification method
    void Clean();

    // Test Method
    int GraphEmpty() const;

    // Data Access Methods
    int GetStatesNumber() const;
    N3DState *GetState(int i);
    int GetEdgesNumber() const;
    CK3dEntity *GetSafeEntity(N3DState *s);
    //	int GetWeight(const CK3dEntity& v1, const CK3dEntity& v2);

    // Find the State Index by the associated data
    int SeekPosition(CK_ID e) const;
    int SeekPosition(N3DState *e) const;
    // Get the successors of State i
    N3DEdge *GetSuccessorsList(int i);

    // Graph Modification
    // insertion
    N3DState *InsertState(CK3dEntity *e);
    N3DEdge *InsertEdge(CK3dEntity *s, CK3dEntity *e, float d);

    // Activity
    void ActivateEdge(CK3dEntity *s, CK3dEntity *e, CKBOOL a, CKBOOL dyn = TRUE);
    void ActivateState(CK3dEntity *s, CKBOOL a);
    void ResetActivity();
    CKBOOL IsEdgeActive(CK3dEntity *s, CK3dEntity *e);

    // Occupation
    CKBOOL SetOccupier(CK3dEntity *s, CK3dEntity *o);
    CK3dEntity *GetOccupier(CK3dEntity *s);

    // deletion
    void DeleteState(CK_ID e);
    void DeleteEdge(CK3dEntity *s, CK3dEntity *e);

    void CreateFromChunk(CKStateChunk *);
    void SaveToChunk(CKStateChunk *);

    // Path finding
    CK3dEntity *FindPath(CK3dEntity *start, CK3dEntity *goal, CKGroup *path, CKBOOL basic = FALSE, CKBOOL occupation = FALSE);

    // Distance update
    void UpdateDistance();

    // Author highlight display
    void StartDisplay()
    {
        if (m_Display)
            delete m_Display;
        m_Display = new N3DDisplayStructure(m_Context);
    }
    void StopDisplay()
    {
        delete m_Display;
        m_Display = NULL;
    }
    N3DDisplayStructure *GetDisplay() { return m_Display; }
    CKGroup *GetPath()
    {
        if (m_Display)
            return m_Display->m_Group;
        else
            return NULL;
    }
    XVoidArray *GetExtentsList()
    {
        if (m_Display)
            return &m_Display->m_NodeExtents;
        else
            return NULL;
    }

private:
    // methods
    void TestAndReallocate(int i = 1);
    // insertion in a queue
    void PushOnOpen(XList<N3DState *> &open, N3DState *n);
    // return en edge pointer, NULL if not found
    N3DEdge *EdgeSeek(CK_ID s, CK_ID e);
    // same thing with the indice of the start node
    //	N3DEdge* EdgeSeek(int s,CK_ID e);
    // Create a State
    N3DEdge *CreateEdge(int e, float d, float dist);
    // Find the node Containing the Entity e
    N3DState *StateSeek(CK_ID e);
    // retreive the distance between 2 nodes
    float GetDistance(N3DState *n1, N3DState *n2);

    // members

    // list of vertices
    N3DState *m_States;
    // graph size
    int m_Size;
    // edge number;
    int m_EdgeNumber;
    // Graph allocated size
    int m_AllocatedSize;
    // Display structure (for selected items highlighting)
    N3DDisplayStructure *m_Display;
    CKContext *m_Context;
};

extern char *Network3dName;

void GraphRender(CKRenderContext *ctx, void *arg);

#endif