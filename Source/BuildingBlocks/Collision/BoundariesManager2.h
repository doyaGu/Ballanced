#ifndef BOUNDARIESMANAGER2
#define BOUNDARIESMANAGER2

#include "VxMath.h"
#include "FixedSizeAllocator.h"
#include "CK3dEntity.h"

class BoundariesManager
{
public:
    struct CPair
    {
        CK_ID object1;
        CK_ID object2;
    };

    enum
    {
        INITIALSIZE = 8
    };

    BoundariesManager();
    ~BoundariesManager();

    // (Un)Registering objects
    void Add(CK3dEntity *iEntity, CKBOOL iMoving);
    void Remove(CK3dEntity *iEntity);

    // Updating the Bboxs boundaries of the moving
    void UpdateStartMovings();
    void UpdateEndMovings();

    // Updating the Bbox of every obstacles (call this on the resume event)
    void UpdateAll();

    // We seek all the objects overlapping with the current one
    const XArray<CPair> &FindOverlapping();
    const XArray<CPair> &FindOverlapping2();

    // NOT IMPLEMENTED
    const XArray<CKObject *> &FindOverlapping(CK3dEntity *ent);
    const XArray<CKObject *> &FindOverlapping(VxBbox &box);

    // Init
    void Init();
    // Clear all
    void Clear();

    enum
    {
        XCROSSED = 1,
        YCROSSED = 2,
        ZCROSSED = 4,
        CROSSED = 7
    };

    struct Bound
    {

        VxBbox ranges; // range values
        CKWORD state;  // state of the bound
        CKWORD moving;
        CK3dEntity *object; // object index in the object table
    };

    struct SingleBound
    {
        enum BoundType
        {
            MIN,
            MAX
        };

        int operator<(const SingleBound &iS) const
        {
            return *value < *(iS.value);
        }

        float GetValue()
        {
            return (*value);
        }

        Bound *bound;
        float *value;
        BoundType type;
    };

private:
    Bound *CreateBound(CK3dEntity *iEntity);

    void RemoveBound(int iIndexRemoved);

    void RecurseFindOverlapping(const XArray<short> &iGroup, int iAxis1, int iAxis2, int iAxis3);

    void FindOpenCloseBoundaries(int iAxis, const XArray<short> &iGroup, XArray<SingleBound> &oBoundaries);
    void SortOpenCloseBoundaries(XArray<SingleBound> &oBoundaries);

    // Registered Objects
    XObjectPool<Bound> m_BoundPool;
    XArray<Bound *> m_Bounds;
    XArray<Bound *> m_MovingBounds;

    XArray<SingleBound> m_Axis[3];

    XArray<CPair> m_CollidingPairs;
    XArray<CKObject *> m_CrossedEntities;

    // Temp data
    XArray<Bound *> m_Opened;
    typedef XHashTable<XArray<Bound *>, Bound *, XHashFun<void *>> HashBound2Bound;
    HashBound2Bound m_BoundsMap;
};

#endif