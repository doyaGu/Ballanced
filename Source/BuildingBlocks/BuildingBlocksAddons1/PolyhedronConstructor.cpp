// PolyhedronConstructor.cpp: implementation of the PolyhedronConstructor class.
//
//////////////////////////////////////////////////////////////////////
#include "CKAll.h"

#include <assert.h>

#include "PolyhedronConstructor.h"
#include "NearestPointGrid.h"

using namespace BBAddons1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PolyhedronConstructor::~PolyhedronConstructor()
{
    m_Edges;

    for (PolyhedronArray::Iterator it = m_Polyhedrons.Begin(); it != m_Polyhedrons.End(); ++it)
    {
        Polyhedron *p = *it;
        delete p;
    }
}

struct TriList
{
    CKWORD FindShared(TriList *iList, CKWORD iTriToExclude)
    {
        for (TriList *thisList = this; thisList != 0; thisList = thisList->next)
        {
            if (thisList->triIndex != iTriToExclude)
            {

                for (TriList *otherList = iList; otherList != 0; otherList = otherList->next)
                {
                    if (otherList->triIndex == thisList->triIndex)
                        return otherList->triIndex;
                }
            }
        }
        return 0xffff;
    }

    CKWORD triIndex;
    CKWORD padding;
    TriList *next;
};

void PolyhedronConstructor::BuildFaceAdjacency()
{
    // Based on the algorithm by Christer Ericson
    // in RealTime Collision Detection
    // p 478.

    int faceCount = m_Mesh->GetFaceCount();
    int faceIndicesCount = faceCount * 3;
    CKWORD *faceIndices = m_FaceIndices.Begin();
    int vertexCount = m_Mesh->GetVertexCount();

    // create the maximum array of trilist
    XArray<TriList> triListEntry;
    triListEntry.Resize(faceIndicesCount);

    for (int j = 0; j < faceIndicesCount; ++j)
    {
        triListEntry[j].triIndex = 0xffff;
        triListEntry[j].next = 0;
    }

    // array that will contain the trilist used by each vertex
    XArray<TriList *> triListHead;
    triListHead.Resize(vertexCount);
    triListHead.Fill(0);

    //////////////////////////////////////////////////////////////////////////
    // Fill the array of adjacent triangle for each vertex

    CKWORD i;
    int cnt = 0;

    for (i = 0; i < faceCount; ++i)
    {
        for (int v = 0; v < 3; ++v)
        {
            // get the vertex index
            CKWORD vi = faceIndices[i * 3 + v];
            // fill in a new triangle for this vertex
            triListEntry[cnt].triIndex = i;
            // link new entry with the existing one for the same vertex
            triListEntry[cnt].next = triListHead[vi];
            triListHead[vi] = &triListEntry[cnt++];
        }
    }

    /*/ Debug Log
    for (int i = 0; i < vertexCount; ++i) {
        char buffer[2048];
        sprintf(buffer, "%d : ", i);

        TriList* it = triListHead[i];
        while (it) {
            sprintf(buffer, "%s,%d", buffer, it->triIndex);
            it = it->next;
        }

        m_Mesh->GetCKContext()->OutputToConsole(buffer);
    }
    */

    //////////////////////////////////////////////////////////////////////////
    // Compute the face adjacency by finding which face are shared for
    // a pair of vertices.

    m_FaceAdjacency.Resize(faceIndicesCount);
    CKWORD *adjacency = m_FaceAdjacency.Begin();

    for (i = 0; i < faceCount; ++i)
    {
        for (int v = 0; v < 3; ++v)
        {
            // get the first vertex index
            CKWORD vi1 = faceIndices[i * 3 + v];
            // get the next vertex index
            CKWORD vi2 = faceIndices[i * 3 + (v + 1) % 3];

            CKWORD otherTri = triListHead[vi1]->FindShared(triListHead[vi2], i);

            // no face is written as 0xffff
            adjacency[i * 3 + v] = otherTri;
        }
    }

    /*/ Debug Log
    for (int i = 0; i < faceIndicesCount; ++i) {
        m_Mesh->GetCKContext()->OutputToConsoleEx("%6d - %6d - %6d", i, m_FaceIndices[i], adjacency[i]);
    }
    */
}

void PolyhedronConstructor::WeldVertices()
{
    NearestPointGrid ng;

    VxBbox box = m_Mesh->GetLocalBox();

    VxVector scale = box.GetSize();
    if (scale.x > EPSILON)
        scale.x = (10.f - 0.0001f) / scale.x;
    else
        scale.x = 1.f;
    if (scale.y > EPSILON)
        scale.y = (10.f - 0.0001f) / scale.y;
    else
        scale.y = 1.f;
    if (scale.z > EPSILON)
        scale.z = (10.f - 0.0001f) / scale.z;
    else
        scale.z = 1.f;

    ng.SetGridDimensions(10, 10, 10);

    ng.SetThreshold(EPSILON);

    int faceCount = m_Mesh->GetFaceCount();

    XArray<int> usedVertices;

    VxVector p, sp;
    for (int i = 0; i < faceCount; ++i)
    {
        for (int v = 0; v < 3; ++v)
        {
            CKWORD vi = m_FaceIndices[i * 3 + v];

            m_Mesh->GetVertexPosition(vi, &p);

            sp = p - box.Min;
            sp *= scale;

            int weldIndex = ng.FindNearPoint(sp);
            if (weldIndex == -1)
            { // new point
                ng.AddPoint(sp, vi);
                usedVertices.PushBack(vi);
            }
            else
            {
                // existing point : weld
                m_FaceIndices[i * 3 + v] = weldIndex;
            }
        }
    }
}

#ifdef ADJACENCYMETHOD

PolyhedronConstructor::PolyhedronConstructor(CKMesh *mesh)
{
    if (!mesh)
        return;

    m_Mesh = mesh;

    // recopy the face indices
    CKWORD *faceIndices = mesh->GetFacesIndices();
    int faceCount = m_Mesh->GetFaceCount();
    m_FaceIndices.Resize(faceCount * 3);
    memcpy(m_FaceIndices.Begin(), faceIndices, faceCount * 3 * sizeof(CKWORD));

    // first weld the vertices
    WeldVertices();

    // compute mesh adjacency
    BuildFaceAdjacency();
}

void PolyhedronConstructor::Simplify(float threshold)
{
    int faceCount = m_Mesh->GetFaceCount();

    XClassArray<Polyhedron> face2poly;
    face2poly.Resize(faceCount);

    CKWORD *faceIndices = m_FaceIndices.Begin();

    int i;

    for (i = 0; i < faceCount; ++i)
    {
        CKWORD refFace = face2poly[i].m_ReferenceFace;
        if (refFace == 0xffff)
        {
            face2poly[i].m_ReferenceFace = i;
            face2poly[i].m_OriginalFaces.PushBack(i);

            refFace = i;
        }

        const VxVector &fn = m_Mesh->GetFaceNormal(refFace);

        for (int j = 0; j < 3; ++j)
        {
            int neighborFace = m_FaceAdjacency[i * 3 + j];
            if (neighborFace == 0xffff)
                continue;

            if (face2poly[neighborFace].m_ReferenceFace == 0xffff)
            {
                // not parsed yet
                const VxVector &nn = m_Mesh->GetFaceNormal(neighborFace);
                if (DotProduct(fn, nn) > threshold)
                {
                    // coplanar
                    face2poly[neighborFace].m_ReferenceFace = refFace;

                    // add the face info to the ref face
                    face2poly[refFace].m_OriginalFaces.PushBack(neighborFace);
                }
            }
        }
    }

    // we now have to compute all the informations
    // and regroup the valid (i == m_ReferenceFace)
    // into a smaller array

    for (i = 0; i < faceCount; ++i)
    {
        if (i == face2poly[i].m_ReferenceFace)
        {
            Polyhedron *p = new Polyhedron;

            // Face Indices
            p->m_OriginalFaces = face2poly[i].m_OriginalFaces;

            // Normal
            p->m_Normal = m_Mesh->GetFaceNormal(i);

            int fCount = p->m_OriginalFaces.Size();
            for (int j = 0; j < fCount; ++j)
            {
                int f = p->m_OriginalFaces[j];

                // Bounding Box
                p->m_Box.Merge(m_Mesh->GetFaceVertex(f, 0));
                p->m_Box.Merge(m_Mesh->GetFaceVertex(f, 1));
                p->m_Box.Merge(m_Mesh->GetFaceVertex(f, 2));

                // Edges
                CKWORD a, b, c;
                XMinMax(faceIndices[f * 3 + 0], faceIndices[f * 3 + 1], faceIndices[f * 3 + 2], a, b, c);

                int originalIndices[3];
                m_Mesh->GetFaceVertexIndex(f, originalIndices[0], originalIndices[1], originalIndices[2]);

                // Smoothing
                for (int k = 0; k < 3; ++k)
                {
                    VxVector vertexNormal;
                    m_Mesh->GetVertexNormal(originalIndices[k], &vertexNormal);
                    if (DotProduct(vertexNormal, p->m_Normal) < 0.99f)
                    { // The vertex normal is different than the face normal
                        // hence the face is smooth
                        p->m_Smooth = TRUE;
                        break;
                    }
                }

                // Vertices indices
                if (!p->m_OriginalVertices.IsHere(a))
                    p->m_OriginalVertices.PushBack(a);
                if (!p->m_OriginalVertices.IsHere(b))
                    p->m_OriginalVertices.PushBack(b);
                if (!p->m_OriginalVertices.IsHere(c))
                    p->m_OriginalVertices.PushBack(c);
            }

            m_Polyhedrons.PushBack(p);
        }
    }
    XASSERT(m_Polyhedrons.Size() > 0);
}

#else

PolyhedronConstructor::PolyhedronConstructor(CKMesh *mesh)
{
    if (!mesh)
        return;

    m_Mesh = mesh;

    CKVINDEX *faceindices = mesh->GetFacesIndices();
    // Construction
    int vcount = mesh->GetVertexCount();
    m_EdgeCount = vcount * (vcount - 1);
    m_Edges = new Edge[m_EdgeCount];
    if (!m_Edges)
        VXTHROW "Not enough memory for Lighmap Calculation : try work on smaller meshes";

    int fcount = mesh->GetFaceCount();
    WORD a, b, c;
    for (int i = 0; i < fcount; ++i, faceindices += 3)
    {
        Polyhedron *p = new Polyhedron;
        m_Polyhedrons.PushBack(p);

        // Normal
        p->m_Normal = mesh->GetFaceNormal(i);

        // Bounding Box
        p->m_Box.Merge(mesh->GetFaceVertex(i, 0));
        p->m_Box.Merge(mesh->GetFaceVertex(i, 1));
        p->m_Box.Merge(mesh->GetFaceVertex(i, 2));

        // Face Index
        p->m_OriginalFaces.PushBack(i);

        // Edges
        XMinMax(faceindices[0], faceindices[1], faceindices[2], a, b, c);

        // Smoothing
        for (int j = 0; j < 3; ++j)
        {
            VxVector vertexNormal;
            mesh->GetVertexNormal(faceindices[j], &vertexNormal);
            if (DotProduct(vertexNormal, p->m_Normal) < 0.99f)
            { // The vertex normal is different than the face normal
                // hence the face is smooth
                p->m_Smooth = TRUE;
                break;
            }
        }

        // Vertices indices
        p->m_OriginalVertices.PushBack(a);
        p->m_OriginalVertices.PushBack(b);
        p->m_OriginalVertices.PushBack(c);

        Edge *edge = &m_Edges[a * vcount + b];
        edge->a = a;
        edge->b = b;

        if (edge->v.p1)
            edge->v.p2 = p;
        else
            edge->v.p1 = p;
        p->m_Edges.PushBack(edge);

        edge = &m_Edges[a * vcount + c];
        edge->a = a;
        edge->b = c;
        if (edge->v.p1)
            edge->v.p2 = p;
        else
            edge->v.p1 = p;
        p->m_Edges.PushBack(edge);

        edge = &m_Edges[b * vcount + c];
        edge->a = b;
        edge->b = c;
        if (edge->v.p1)
            edge->v.p2 = p;
        else
            edge->v.p1 = p;
        p->m_Edges.PushBack(edge);
    }
}

void PolyhedronConstructor::RSimplify(float threshold, Polyhedron &reference, Edge *edgefrom, Polyhedron &p) �
{
    CKContext *ctx = m_Mesh->GetCKContext();

    // we add the polyhedron to the reference
    reference.m_OriginalFaces += p.m_OriginalFaces;
    reference.m_Box.Merge(p.m_Box);

    int originalface = p.m_OriginalFaces[0];
    //	ctx->OutputToConsoleEx("  RSimplify Adding %d faces = %d Edge %d %d",p.m_OriginalFaces.Size(),p.m_OriginalFaces[0],edgefrom->a,edgefrom->b);

    EdgeArray::Iterator it;
    // we first mark all the returning edges as reference return
    for (it = p.m_Edges.Begin(); it != p.m_Edges.End(); ++it)
    {
        // we find the index of the next face
        int index = 0;
        if (&p != (*it)->p[index])
            index = 1;

        // the edge is set to the reference face
        if ((*it)->p[index])
            (*it)->p[index] = &reference;
    }

    // we iterate on all the edges to propagate, if relevant
    for (it = p.m_Edges.Begin(); it != p.m_Edges.End(); ++it)
    {
        if (*it == edgefrom)
            continue; // this is were we came from, do not grow backward

        // we add the vertices
        if (!reference.m_OriginalVertices.IsHere((*it)->a))
        {
            reference.m_OriginalVertices.PushBack((*it)->a);
        }
        if (!reference.m_OriginalVertices.IsHere((*it)->b))
        {
            reference.m_OriginalVertices.PushBack((*it)->b);
        }

        // we find the index of the next face
        int index = 0;
        if (&reference == (*it)->p[index])
            index = 1;

        Polyhedron *nextpoly = (*it)->p[index];

        if (!nextpoly)
        {
            reference.m_Edges.PushBack(*it);
            continue; // this edge is a frontier
        }

        // XASSERT(nextpoly->m_Magic == 0);

        // If the next poly is the reference one, we are cycling = stop there
        if (nextpoly == &reference)
        {
            continue;
        }

        // Is the face a good candidate
        float dp = DotProduct(nextpoly->m_Normal, reference.m_Normal);
        if (XAbs(dp) < (threshold))
            continue;

        // we mark the polygon as smooth if the face added is
        reference.m_Smooth |= nextpoly->m_Smooth;

        // the edge is marked as internal
        (*it)->p[1 - index] = NULL;
        (*it)->p[index] = NULL;

        // the recursion
        RSimplify(threshold, reference, *it, *nextpoly);
    }

    // the polyhedron must be removed
    m_Polyhedrons.Remove(&p);
    XASSERT(p.m_Magic == 0);
    p.m_Magic = 1;

    //	XASSERT(p.m_Edges.Size() != 0);

    m_DeletedPolyhedrons.PushBack(&p);
    delete &p;
}

void PolyhedronConstructor::Simplify(float threshold)
{
    CKContext *ctx = m_Mesh->GetCKContext();

    XArray<Polyhedron *> poly(m_Polyhedrons.Size());

    EdgeArray tempedges(3);

    while (m_Polyhedrons.Size())
    {
        Polyhedron *reference = m_Polyhedrons.PopBack();
        poly.PushBack(reference);

        tempedges.Swap(reference->m_Edges);

        // ctx->OutputToConsoleEx("Simplify Polyhedron %d Edge %d",reference->m_OriginalFaces[0],tempedges.Size());
        for (EdgeArray::Iterator it = tempedges.Begin(); it != tempedges.End(); ++it)
        {
            // we find the index of the next face
            int index = 0;
            if (reference == (*it)->p[index])
                index = 1;

            // we clear the reference from the parsed edges
            (*it)->p[1 - index] = NULL;

            Polyhedron *nextpoly = (*it)->p[index];

            if (!nextpoly)
            {
                reference->m_Edges.PushBack(*it);
                continue; // this edge is a frontier
            }

            if (nextpoly == reference)
            {
                (*it)->p[index] = NULL;
                continue;
            }

            // Is the face a good candidate
            float dp = DotProduct(nextpoly->m_Normal, reference->m_Normal);
            if (XAbs(dp) < (threshold))
                continue;

            // we mark the polygon as smooth if the face added is not perfectly coplanar
            reference->m_Smooth |= nextpoly->m_Smooth;

            // the recursion
            RSimplify(threshold, *reference, *it, *nextpoly);
        }

        tempedges.Resize(0);
    }

    m_Polyhedrons.Swap(poly);
    for (PolyhedronArray::Iterator it = m_Polyhedrons.Begin(); it != m_Polyhedrons.End();)
    {
        if (m_DeletedPolyhedrons.IsHere(*it))
            it = m_Polyhedrons.Remove(it);
        else
            ++it;
    }
}

#endif