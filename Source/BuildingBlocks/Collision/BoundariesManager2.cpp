#include "BoundariesManager2.h"

#include "CollisionManager.h"

BoundariesManager::BoundariesManager() {}

BoundariesManager::~BoundariesManager() {}

void BoundariesManager::Clear()
{
    m_BoundPool.Clear();

    m_Bounds.Resize(0);
    m_MovingBounds.Resize(0);

    m_Axis[0].Resize(0);
    m_Axis[1].Resize(0);
    m_Axis[2].Resize(0);

    Init();
}

void BoundariesManager::Init()
{
    m_Bounds.Resize(INITIALSIZE * 3);
    m_Bounds.Resize(0);

    m_MovingBounds.Resize(INITIALSIZE * 3);
    m_MovingBounds.Resize(0);

    m_Axis[0].Resize(INITIALSIZE * 2);
    m_Axis[0].Resize(0);

    m_Axis[1].Resize(INITIALSIZE * 2);
    m_Axis[1].Resize(0);

    m_Axis[2].Resize(INITIALSIZE * 2);
    m_Axis[2].Resize(0);
}

void BoundariesManager::Add(CK3dEntity *iEntity, CKBOOL iMoving)
{
    int count = m_Bounds.Size();
    int i = 0;
    for (; i < count; ++i)
    {
        if (m_Bounds[i]->object == iEntity) // Object found
            break;
    }

    if (i != count) // Object already there
        return;

    Bound *b = CreateBound(iEntity);
    m_Bounds.PushBack(b);

    if (iMoving)
    {
        b->moving = TRUE;
        m_MovingBounds.PushBack(b);
    }
}

/////////////////////////////////////////////////////////////////////
// PRIVATE PART
/////////////////////////////////////////////////////////////////////

BoundariesManager::Bound *
BoundariesManager::CreateBound(CK3dEntity *iEntity)
{
    const VxBbox &box = (iEntity->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE) ? iEntity->GetHierarchicalBox() : iEntity->GetBoundingBox();

    // Create the bound
    Bound *b = m_BoundPool.Allocate();
    b->ranges = box;
    b->object = iEntity;
    b->state = 0;
    b->moving = 0;

    SingleBound dummy;

    // we then add the single bound referencing the bound
    for (int i = 0; i < 3; ++i)
    {
        // Single Bounds

        // min
        dummy.type = SingleBound::MIN;
        dummy.bound = b;
        dummy.value = &(b->ranges.Min[i]);
        m_Axis[i].InsertSorted(dummy);

        // max
        dummy.type = SingleBound::MAX;
        dummy.bound = b;
        dummy.value = &(b->ranges.Max[i]);
        m_Axis[i].InsertSorted(dummy);
    }

    return b;
}

void BoundariesManager::Remove(CK3dEntity *iEntity)
{
    CKBOOL foundMoving = FALSE;
    Bound *b = NULL;

    int count = m_Bounds.Size();
    int i;
    for (i = 0; i < count; ++i)
    {
        b = m_Bounds[i];
        if (b->object == iEntity)
        {
            if (b->moving)
                foundMoving = TRUE;

            m_Bounds.Remove(m_Bounds.At(i));
            break;
        }
    }

    if (i == count)
        return;

    if (foundMoving)
    {
        m_MovingBounds.Remove(b);
    }

    // we now have to remove the single bounds referencing this bound
    // from the axis array

    for (int axis = 0; axis < 3; ++axis)
    {
        for (SingleBound *it = m_Axis[axis].Begin(); it != m_Axis[axis].End();)
        {
            if (it->bound == b)
                it = m_Axis[axis].Remove(it);
            else
                ++it;
        }
    }

    // Destroy the Bound
    m_BoundPool.Free(b);
}

void BoundariesManager::UpdateStartMovings()
{
    if (!m_MovingBounds.Size())
        return;

    // we set the initial position of the moving objects
    // for the current frame
    for (Bound **it = m_MovingBounds.Begin(); it != m_MovingBounds.End(); ++it)
    {
        CK3dEntity *moving = (*it)->object;
        if (!moving)
            continue; // Normally impossible

        const VxBbox &box = (moving->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE) ? moving->GetHierarchicalBox() : moving->GetBoundingBox();

        // Update the box
        (*it)->ranges = box;
    }
}

void BoundariesManager::UpdateEndMovings()
{
    if (!m_MovingBounds.Size())
        return;

    // we set the initial position of the moving objects
    // for the current frame
    for (Bound **it = m_MovingBounds.Begin(); it != m_MovingBounds.End(); ++it)
    {
        CK3dEntity *moving = (*it)->object;
        if (!moving)
            continue; // Normally impossible

        const VxBbox &box = (moving->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE) ? moving->GetHierarchicalBox() : moving->GetBoundingBox();

        (*it)->ranges.Merge(box);
    }

    // we now sorts each axis
    SortOpenCloseBoundaries(m_Axis[0]);
    SortOpenCloseBoundaries(m_Axis[1]);
    SortOpenCloseBoundaries(m_Axis[2]);
}

void BoundariesManager::UpdateAll()
{
    // we set the initial position of the moving objects
    // for the current frame
    for (Bound **it = m_Bounds.Begin(); it != m_Bounds.End(); ++it)
    {
        CK3dEntity *moving = (*it)->object;
        if (!moving)
            continue; // Normally impossible

        const VxBbox &box = (moving->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE) ? moving->GetHierarchicalBox() : moving->GetBoundingBox();

        // Update the ranges
        (*it)->ranges = box;
    }

    // we now sorts each axis
    SortOpenCloseBoundaries(m_Axis[0]);
    SortOpenCloseBoundaries(m_Axis[1]);
    SortOpenCloseBoundaries(m_Axis[2]);
}

#define ADDVALUE(x, y, a) m_Matrix[y * m_MatrixWidth + x] |= a
#define GETVALUE(x, y) m_Matrix[y * m_MatrixWidth + x]

const XArray<BoundariesManager::CPair> &BoundariesManager::FindOverlapping()
{
    // Clear the pair
    m_CollidingPairs.Resize(0);
    // Clearing the open list
    m_Opened.Resize(0);

    //	m_BoundsMap.Clear();

    // Axis Overlapping
    int mask = 1;
    for (int axis = 0; axis < 3; ++axis, mask <<= 1)
    {

        SingleBound *sbound = m_Axis[axis].Begin();
        SingleBound *ebound = m_Axis[axis].End();

        // Axis Overlapping
        for (; sbound != ebound; ++sbound)
        {

            if (sbound->type == SingleBound::MIN)
            {
                // add in open list
                m_Opened.PushBack(sbound->bound);
            }
            else
            { // it's an end
                // remove this
                m_Opened.Remove(sbound->bound);
                // we add all the opened
                Bound *minb;
                Bound *maxb;
                for (Bound **it = m_Opened.Begin(); it != m_Opened.End(); ++it)
                {
                    XMinMax(sbound->bound, *it, minb, maxb);
                    m_BoundsMap[minb].InsertSorted(maxb);
                }
            }
        }
    }

    HashBound2Bound::Iterator it = m_BoundsMap.Begin();
    HashBound2Bound::Iterator itend = m_BoundsMap.End();

    while (it != itend)
    {
        XArray<Bound *> &array = (*it);
        //		array.Sort();
        int c = 0;
        Bound *old = NULL;
        for (Bound **ait = array.Begin(); ait != array.End(); ++ait)
        {
            if (old == *ait)
            {
                if (++c == 3)
                {
                    m_CollidingPairs.Expand();
                    m_CollidingPairs.Back().object1 = it.GetKey()->object->GetID();
                    m_CollidingPairs.Back().object2 = (*ait)->object->GetID();
                }
            }
            else
            {
                c = 1;
            }
            old = *ait;
        }

        array.Resize(0);
        ++it;
    }

    return m_CollidingPairs;
}

const XArray<CKObject *> &BoundariesManager::FindOverlapping(CK3dEntity *iEntity)
{
    m_CrossedEntities.Resize(0);

    // test if the object is here ?
    // normally already found before

    int size = m_Bounds.Size();

    // Axis Overlapping
    int mask = 1;
    for (int axis = 0; axis < 3; ++axis, mask <<= 1)
    {

        SingleBound *sbound = m_Axis[axis].Begin();
        SingleBound *ebound = m_Axis[axis].End();

        // Before the box, we flag the min and unflag the mask
        // so that when we enter the box, the opened min are flagged
        for (; sbound != ebound; ++sbound)
        {
            // we test if it is the searched object
            if ((sbound->bound->object) == iEntity)
            {
                ++sbound; // we skip the object itself
                break;
            }
            else
            {
                if (sbound->type == SingleBound::MIN)
                {
                    sbound->bound->state |= mask;
                }
                else
                {
                    sbound->bound->state &= ~mask;
                }
            }
        }

        // during the entity, we flag the min and DO NOT unflag the max
        for (; sbound != ebound; ++sbound)
        {
            // we test if we're not outside the range
            if (sbound->bound->object == iEntity)
            {
                continue;
            }
            else
            {
                if (sbound->type == SingleBound::MIN)
                {
                    sbound->bound->state |= mask;
                }
            }
        }

        // After the box, nothing to do, we're done
    }

    // we have now flagged all the objects
    // we all we have is found the 3 flagged
    // we inactivate all the bounds TODO : maybe integrate this init in the following loops

    for (Bound **it = m_Bounds.Begin(); it != m_Bounds.End(); ++it)
    {
        if (((*it)->state & CROSSED) == CROSSED)
        {
            m_CrossedEntities.PushBack((*it)->object);
        }
        (*it)->state &= ~CROSSED;
    }

    return m_CrossedEntities;
}

const XArray<CKObject *> &BoundariesManager::FindOverlapping(VxBbox &box)
{
    // Clear the crossed entities
    m_CrossedEntities.Resize(0);

    // 3 Axis
    ///////////////

    int mask = 1;
    for (int axis = 0; axis < 3; ++axis, mask <<= 1)
    {

        SingleBound *sbound = m_Axis[axis].Begin();
        SingleBound *ebound = m_Axis[axis].End();

        // Before the box, we flag the min and unflag the mask
        // so that when we enter the box, the opened min are flagged
        {
            float minValue = box.Min[axis];

            for (; sbound != ebound; ++sbound)
            {
                // we test if it is the searched object
                if (*(sbound->value) > minValue)
                {
                    break;
                }
                else
                {
                    if (sbound->type == SingleBound::MIN)
                    {
                        sbound->bound->state |= mask;
                    }
                    else
                    {
                        sbound->bound->state &= ~mask;
                    }
                }
            }
        }

        int beforebox = sbound - m_Axis[axis].Begin();

        // during the box, we flag the min and DO NOT unflag the max

        {
            float maxValue = box.Max[axis];

            for (; sbound != ebound; ++sbound)
            {
                // we test if we're not outside the range
                if (*(sbound->value) > maxValue)
                {
                    break;
                }
                else
                {
                    if (sbound->type == SingleBound::MIN)
                    {
                        sbound->bound->state |= mask;
                    }
                }
            }
        }

        int objectsCrossed = sbound - m_Axis[axis].Begin();
        if (!objectsCrossed)
        { // No crossing on this axis, we are safe to go !

            // But we still need to reset the state of the bounds
            for (Bound **it = m_Bounds.Begin(); it != m_Bounds.End(); ++it)
            {
                (*it)->state = 0;
            }

            return m_CrossedEntities;
        }
        // After the box, nothing to do, we're done
    }

    // we have now flagged all the objects
    // we all we have is found the 3 flagged
    // we inactivate all the bounds TODO : maybe integrate this init in the following loops

    for (Bound **it = m_Bounds.Begin(); it != m_Bounds.End(); ++it)
    {
        if (((*it)->state & CROSSED) == CROSSED)
        {
            m_CrossedEntities.PushBack((*it)->object);
        }
        (*it)->state = 0;
    }

    return m_CrossedEntities;
}

void BoundariesManager::SortOpenCloseBoundaries(XArray<SingleBound> &oBoundaries)
{
    CKBOOL Noswap = TRUE;

    int size = oBoundaries.Size();
    for (int i = 1; i < size; i++)
    {
        for (int j = size - 1; j >= i; j--)
        {
            if (oBoundaries[j - 1].GetValue() > oBoundaries[j].GetValue())
            {
                XSwap(oBoundaries[j - 1], oBoundaries[j]);
                Noswap = FALSE;
            }
        }
        if (Noswap)
            break;
        Noswap = TRUE;
    }
}

///////////////////////////////////////////////////////
// Steve Rabin implementation : too slow so not used
///////////////////////////////////////////////////////
/*
const XArray<BoundariesManager::CPair> &BoundariesManager::FindOverlapping2()
{
    m_CollidingPairs.Resize(0);

    XArray<short> group;
    group.Resize(m_Objects.Size());
    for (int i=0; i<m_Objects.Size(); ++i) {
        group[i] = (short)i;
    }

    RecurseFindOverlapping(group,0,1,2);
    return m_CollidingPairs;
}

void BoundariesManager::RecurseFindOverlapping(const XArray<short>& iGroup,int iAxis1,int iAxis2,int iAxis3)
{
    XASSERT(iGroup.Size() != 1);

    if ((iGroup.Size()<10) || (iAxis1 == -1)) { // end of test : we have to generate the pairs
        for (short* it = iGroup.Begin(); it != iGroup.End()-1; ++it) {
            for (short* it2 = it+1; it2 != iGroup.End(); ++it2) {
                if (VxIntersect::AABBAABB(m_Objects[(*it)]->GetBoundingBox(),m_Objects[(*it2)]->GetBoundingBox())) {
                    m_CollidingPairs.Expand();
                    m_CollidingPairs.Back().object1 = m_Objects[(*it)]->GetID();
                    m_CollidingPairs.Back().object2 = m_Objects[(*it2)]->GetID();
                }
            }
        }

        return;
    }

    XArray<SingleBound> boundaries;
    FindOpenCloseBoundaries(iAxis1,iGroup,boundaries);
    SortOpenCloseBoundaries(boundaries);

    XArray<short> subGroup;
    subGroup.Reserve(iGroup.Size());
    unsigned int count = 0;
    int newAxis1 = iAxis2;
    int newAxis2 = iAxis3;
    bool groupSubdivided = false;

    for (SingleBound* it = boundaries.Begin(); it != boundaries.End(); ++it) {
        if (it->type == SingleBound::MIN) { // Open
            ++count;
            subGroup.PushBack(it->bound->object);
        } else {
            if ((--count) == 0) {
                if (subGroup.Size() > 1) {

                    if (it != (boundaries.End()-1)) { // this group is being subdivided
                        groupSubdivided = true;
                    }

                    if (groupSubdivided) { // reconsider other axis
                        switch (iAxis1) {
                        case 0:
                            newAxis1 = 1;
                            newAxis2 = 2;
                            break;
                        case 1:
                            newAxis1 = 0;
                            newAxis2 = 2;
                            break;
                        case 2:
                            newAxis1 = 0;
                            newAxis2 = 1;
                            break;
                        }
                    }
                    RecurseFindOverlapping(subGroup,newAxis1,newAxis2,-1);
                } else groupSubdivided = true;

                subGroup.Resize(0);
            }
        }
    }


}

void BoundariesManager::FindOpenCloseBoundaries(int iAxis, const XArray<short>& iGroup, XArray<SingleBound>& oBoundaries)
{
    oBoundaries.Resize(iGroup.Size()*2);

    for (int i=0; i<iGroup.Size(); ++i) {
        oBoundaries[i*2].type = SingleBound::MIN;
        oBoundaries[i*2].bound= m_Bounds.Begin()+(iGroup[i]*3+iAxis);
        oBoundaries[i*2+1].type = SingleBound::MAX;
        oBoundaries[i*2+1].bound= m_Bounds.Begin()+(iGroup[i]*3+iAxis);
    }
}
*/
