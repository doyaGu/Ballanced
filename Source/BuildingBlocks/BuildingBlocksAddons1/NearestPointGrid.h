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
namespace BBAddons1
{
    class NearestPointGrid
    {
    protected:
        struct Point
        {
            VxVector pt;
            int index;
        };

        typedef XArray<Point> Cell;

        // Grid
        XArray<Cell *> m_Grid;

        // Dimensions
        int m_SizeX;
        int m_SizeY;
        int m_SizeZ;

        int m_SizeXY;
        int m_SizeXYZ;

        // Threshold
        float m_Threshold;
        float m_Threshold2;

        //--- Get Cell
        Cell *&GetCell(const int x, const int y, const int z)
        {
            return m_Grid[x + m_SizeX * y + m_SizeXY * z];
        }

    public:
        //--- Constructor
        NearestPointGrid();

        //--- Destructor
        ~NearestPointGrid();

        //--- Set Grid Dimensions
        void SetGridDimensions(const int sizeX, const int sizeY, const int sizeZ);

        //--- Set Threshold
        void SetThreshold(const float threshold)
        {
            m_Threshold = threshold;
            m_Threshold2 = threshold * threshold;
        }

        //--- Add Point
        void AddPoint(const VxVector &p, int index);

        //--- Find Near Point
        // warning: the returned vector is valid only
        // as long as the NearestPointGrid object exists !
        int FindNearPoint(const VxVector &p);
    };
} // namespace BBAddons1