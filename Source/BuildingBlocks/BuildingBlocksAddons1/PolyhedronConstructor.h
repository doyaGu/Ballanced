// PolyhedronConstructor.h: interface for the PolyhedronConstructor class.
//
//////////////////////////////////////////////////////////////////////

#ifndef POLYHEDRONCONSTRUCTOR
#define POLYHEDRONCONSTRUCTOR

#define ADJACENCYMETHOD

class PolyhedronConstructor
{
public:
    PolyhedronConstructor(CKMesh *mesh);
    ~PolyhedronConstructor();

#ifdef __MWERKS__
    void Simplify(float threshold);
#else
    void Simplify(float threshold) throw();
#endif

    int GetPolyhedronCount()
    {
        return m_Polyhedrons.Size();
    }

    void GetFaceVertexIndex(CKWORD face, int &a, int &b, int &c)
    {
#ifdef ADJACENCYMETHOD
        a = m_FaceIndices[face * 3 + 0];
        b = m_FaceIndices[face * 3 + 1];
        c = m_FaceIndices[face * 3 + 2];
#else
        m_Mesh->GetFaceVertexIndex(face, a, b, c);
#endif
    }
    const VxVector &GetPolyhedronNormal(int i) { return m_Polyhedrons[i]->m_Normal; }
    const VxBbox &GetPolyhedronBox(int i) { return m_Polyhedrons[i]->m_Box; }
    const XArray<int> &GetPolyhedronFaceIndices(int i) { return m_Polyhedrons[i]->m_OriginalFaces; }
    const XArray<int> &GetPolyhedronVertexIndices(int i) { return m_Polyhedrons[i]->m_OriginalVertices; }
    CKBOOL GetPolyhedronSmooth(int i) { return m_Polyhedrons[i]->m_Smooth; }
    CKBOOL IsPolyhedronValid(int i)
    {
        return ((m_Polyhedrons[i]->m_Magic == 0) || (m_Polyhedrons[i]->m_Magic == 1));
    }

protected:
    // Classes
    struct Edge;

    struct Polyhedron
    {
        Polyhedron() : m_Magic(0), m_Smooth(0), m_ReferenceFace(0xffff), m_Edges(0){};
        int m_Magic;
        CKBOOL m_Smooth;
        CKWORD m_ReferenceFace;
        VxBbox m_Box;
        VxVector m_Normal;
        XArray<int> m_OriginalFaces;
        XArray<int> m_OriginalVertices;
        Edge *m_Edges;
    };

    typedef XArray<Polyhedron *> PolyhedronArray;

    struct Edge
    {
        int a;
        int b;
        union
        {
            struct
            {
                Polyhedron *p1;
                Polyhedron *p2;
            } v;
            Polyhedron *p[2];
        };
        Edge() : a(-1), b(-1)
        {
            v.p1 = NULL;
            v.p2 = NULL;
        }
    };

    CKMesh *m_Mesh;

    Edge *m_Edges; // all the edges
    int m_EdgeCount;

    PolyhedronArray m_Polyhedrons;		  // All the polyhedron
    PolyhedronArray m_DeletedPolyhedrons; // All the deleted polyhedron

    XArray<CKWORD> m_FaceAdjacency;
    XArray<CKWORD> m_FaceIndices;
    void BuildFaceAdjacency();
    void WeldVertices();

    // Methods
    void RSimplify(float threshold, Polyhedron &reference, Edge *edgefrom, Polyhedron &p) throw();
};

#endif // POLYHEDRONCONSTRUCTOR
