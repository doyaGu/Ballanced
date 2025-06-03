/**************************************************/
/* Nearest Point Grid Class
/*
/*
/* Step 0: Set the grid dimensions
/*
/* Step 1: Store 3d points in the grid
/*
/* Step 2: Set the Threshold value
/*
/* Step 3: Give a 3d point to test if it's near (dist<Threshold)
/* some stored 3d points.
/*
/**************************************************/
#include "CKAll.h"

// This class is already implemented in CK2_3D
// no need to compile it in static Lib mode

#include "NearestPointGrid.h"

using namespace BBAddons1;

//--- Constructor
NearestPointGrid::NearestPointGrid() : m_SizeX(0), m_SizeY(0), m_SizeZ(0), m_Threshold(0.1f)
{
    m_Threshold2 = m_Threshold * m_Threshold;
}

//--- Destructor
NearestPointGrid::~NearestPointGrid()
{
    for (int a = 0; a < m_SizeXYZ; ++a)
    {
        delete m_Grid[a];
    }
}

//--- Set Grid Dimensions
void NearestPointGrid::SetGridDimensions(const int sizeX, const int sizeY, const int sizeZ)
{

    m_SizeX = sizeX;
    m_SizeY = sizeY;
    m_SizeZ = sizeZ;
    m_SizeXY = m_SizeX * m_SizeY;
    m_SizeXYZ = m_SizeXY * m_SizeZ;

    m_Grid.Resize(m_SizeXYZ);
    m_Grid.Memset(NULL);
}

//--- Add Point
void NearestPointGrid::AddPoint(const VxVector &p, int index)
{

    Cell *&cell = GetCell((int)p.x, (int)p.y, (int)p.z);
    if (cell == NULL)
    {
        cell = new Cell;
    }
    Point point;
    point.pt = p;
    point.index = index;
    cell->PushBack(point);
}

//--- Find Near Point
// warning: the returned vector is valid only
// as long as the NearestPointGrid object exists !
int NearestPointGrid::FindNearPoint(const VxVector &p)
{

    int cellMinX = (int)(p.x - m_Threshold);
    if (cellMinX < 0)
        cellMinX = 0;
    int cellMinY = (int)(p.y - m_Threshold);
    if (cellMinY < 0)
        cellMinY = 0;
    int cellMinZ = (int)(p.z - m_Threshold);
    if (cellMinZ < 0)
        cellMinZ = 0;

    int cellMaxX = (int)(p.x + m_Threshold);
    if (cellMaxX >= m_SizeX)
        cellMaxX = m_SizeX - 1;
    int cellMaxY = (int)(p.y + m_Threshold);
    if (cellMaxY >= m_SizeY)
        cellMaxY = m_SizeY - 1;
    int cellMaxZ = (int)(p.z + m_Threshold);
    if (cellMaxZ >= m_SizeZ)
        cellMaxZ = m_SizeZ - 1;

    for (int cellZ = cellMinZ; cellZ <= cellMaxZ; ++cellZ)
    {
        for (int cellY = cellMinY; cellY <= cellMaxY; ++cellY)
        {
            for (int cellX = cellMinX; cellX <= cellMaxX; ++cellX)
            {

                Cell *cell = GetCell(cellX, cellY, cellZ);
                if (cell)
                {
                    int cellPtCount = cell->Size();
                    for (int a = 0; a < cellPtCount; ++a)
                    {
                        VxVector &cellPt = (*cell)[a].pt;
                        if (fabsf(cellPt.x - p.x) > m_Threshold)
                            continue;
                        if (fabsf(cellPt.y - p.y) > m_Threshold)
                            continue;
                        if (fabsf(cellPt.z - p.z) > m_Threshold)
                            continue;
                        return (*cell)[a].index;
                    }
                }
            }
        }
    }
    return -1;
}
