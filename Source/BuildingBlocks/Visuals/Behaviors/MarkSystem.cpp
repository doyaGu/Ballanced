/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MarkSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include <cfloat>
#include <cassert>

CKObjectDeclaration *FillBehaviorMarkSystemDecl();
CKERROR CreateMarkSystemProto(CKBehaviorPrototype **);
int MarkSystemFun(const CKBehaviorContext &behcontext);
CKERROR MarkSystemCallback(const CKBehaviorContext &behcontext);

#define MARKSINGLE    0
#define MARKBANDSTART 1
#define MARKBANDIN    2
#define MARKBANDEND   3

#define MARKSTYLE     3

#define STANDARDUVS   4

class Mark
{
public:
    int GetStyle() { return (m_Mode & MARKSTYLE); }

    VxVector m_Positions[4]; // position relative to the entity of the 4 vertices compounding the mark
    VxColor m_Color;		 // color of the mark

    CKDWORD m_ColorPacked;	   // contains the color packed into one DWORD
    CKDWORD m_Mode;			   // mode of the mark, between single and band
    CK_ID m_Entity;			   // entity porting the mark
    CK3dEntity *m_CheckEntity; // check to be sure the entity is still the same

    float m_DeltaAlpha; // evolution of the alpha of the color
    float m_Lifespan;
    class Mark *m_Next;
    class Mark *m_Prev;

    VxRect m_UVs;	  // uvs of the mark (the band use the first 2, the single uses the four)
    float m_FadeTime; // time when the mark will begin to fade
};

class MarkSystem
{
public:
    // Ctor
    MarkSystem()
    {
        m_MarksNumber = 0;
        m_FreeMarks = NULL;
    }

    void Clear()
    {
        m_FreeMarks = m_MarksPool.Begin();
        m_MarksNumber = 0;
        for (int loop = 0; loop < m_MarksPool.Size() - 1; loop++)
        {
            m_MarksPool[loop].m_Next = &m_MarksPool[loop + 1];
        }
        if (m_MarksPool.Size())
            m_MarksPool[m_MarksPool.Size() - 1].m_Next = NULL;

        XSHashTableMarksIt end = m_MaterialMarks.End();
        for (XSHashTableMarksIt begin = m_MaterialMarks.Begin(); begin != end; ++begin)
        {
            MaterialMark &mm = *begin;

            mm.m_Marks = NULL;
            mm.m_LastMark = NULL;
        }
        m_MaterialMarks.Clear();
    }

    // Check the size of the allocated marks
    void CheckSize(int n)
    {
        if (n != m_MarksPool.Size())
        {
            m_MarksPool.Resize(n);
            Clear();
        }
    }

    static void RenderMarkSystem(CKRenderContext *dev, void *arg);

    // Add a new mark
    void AddSingleMark(const VxVector &p, const VxVector &n, CK3dEntity *ref, const VxColor &c, float size, float life, CK_ID mat, float fadetime, const VxRect &uvs, float iOffset);
    void AddContinuousMark(const VxVector &p, const VxVector &n, CK3dEntity *ref, const VxColor &c, float size, float life, float fadetime, CKBOOL last, CK_ID mat, float iOffset);

    // Updates the marks
    void Update(float deltat);

    // the marks pool
    XSArray<Mark> m_MarksPool;

    // number of active marks
    int m_MarksNumber;

    class MaterialMark
    {
    public:
        // Methods
        MaterialMark() : m_Marks(NULL), m_LastMark(NULL) {}
        CKBOOL Update(MarkSystem &ms, float deltat);

        ///
        // Members

        // the marks to draw
        Mark *m_Marks;
        Mark *m_LastMark;
    };

    Mark *FreeLastMark(MaterialMark &iMark);
    void AddNewMark(MaterialMark &iMark, Mark *m);

    typedef XSHashTable<MaterialMark, CK_ID> XSHashTableMarks;
    typedef XSHashTableMarks::Iterator XSHashTableMarksIt;

    XSHashTableMarks m_MaterialMarks;

    Mark *m_FreeMarks;
};

CKBOOL RenderMarkFrame(CKRenderContext *Dev, CKRenderObject *ent, void *Argument)
{
    MarkSystem::RenderMarkSystem(Dev, Argument);
    return TRUE;
}

CKBOOL
MarkSystem::MaterialMark::Update(MarkSystem &ms, float deltat)
{
    Mark *m = m_Marks;
    while (m)
    {
        if (m->m_Lifespan < 0)
        { // The mark should die
            if (m->GetStyle() == MARKBANDSTART)
            { // we are removing a band start
                if (m->m_Prev)
                    m->m_Prev->m_Mode = MARKBANDSTART;
            }

            Mark *tmp = m->m_Next;
            if (m->m_Prev)
                m->m_Prev->m_Next = m->m_Next;
            else
                m_Marks = m->m_Next; // the fist mark is dying
            if (m->m_Next)
                m->m_Next->m_Prev = m->m_Prev;
            else
                m_LastMark = m->m_Prev;
            // we update the free marks
            m->m_Next = ms.m_FreeMarks;
            ms.m_FreeMarks = m;
            m = tmp;

            --ms.m_MarksNumber;
        }
        else
        { // Not die just yet
            if (XAbs(m->m_Lifespan) > EPSILON)
            { // it s not an infinite mark
                m->m_Lifespan -= deltat;
                if (XAbs(m->m_Lifespan) < EPSILON)
                    m->m_Lifespan -= 1.0f;

                // Maybe this is not what the user want... Choose ?
                if (m->m_Lifespan < m->m_FadeTime)
                {
                    m->m_Color.a += m->m_DeltaAlpha * deltat;
                    m->m_ColorPacked = RGBAFTOCOLOR(&m->m_Color);
                }
            }

            // Next Mark
            m = m->m_Next;
        }
    }
    return (m_Marks != NULL); // No more marks, we must return FALSE to be destroyed
}

void MarkSystem::Update(float deltat)
{
    XSHashTableMarksIt begin = m_MaterialMarks.Begin();
    XSHashTableMarksIt end = m_MaterialMarks.End();

    while (begin != end)
    {
        if ((*begin).Update(*this, deltat))
        {
            ++begin;
        }
        else
        { // the mark material is empty
            begin = m_MaterialMarks.Remove(begin);
        }
    }
}

#define ISZERO(a) XAbs(a) < EPSILON

Mark *
MarkSystem::FreeLastMark(MaterialMark &iMark)
{
    if (!iMark.m_Marks)
        return NULL; // nothing to free in this list

    if (iMark.m_LastMark == iMark.m_Marks)
    { // there is 1 mark
        Mark *m = iMark.m_Marks;
        assert(m->m_Prev == NULL);
        assert(m->m_Next == NULL);
        iMark.m_Marks = NULL;
        iMark.m_LastMark = NULL;
        return m;
    }
    else
    { // at least 2 marks
        Mark *m = iMark.m_LastMark;
        if (m->GetStyle() == MARKBANDSTART)
        { // we are removing a band start
            if (m->m_Prev)
                m->m_Prev->m_Mode = MARKBANDSTART;
        }
        iMark.m_LastMark->m_Prev->m_Next = m->m_Next;
        iMark.m_LastMark = iMark.m_LastMark->m_Prev;
        if (m->m_Next)
            m->m_Next->m_Prev = m->m_Prev;
        m->m_Next = NULL;
        m->m_Prev = NULL;
        return m;
    }
}

void MarkSystem::AddNewMark(MaterialMark &iMark, Mark *m)
{
    if (iMark.m_Marks)
    {
        iMark.m_Marks->m_Prev = m;
        m->m_Prev = NULL;
        m->m_Next = iMark.m_Marks;
        iMark.m_Marks = m;
    }
    else
    { // First mark
        m->m_Next = NULL;
        m->m_Prev = NULL;
        iMark.m_Marks = m;
        iMark.m_LastMark = m;
    }
}

void MarkSystem::AddContinuousMark(const VxVector &p, const VxVector &n, CK3dEntity *ref, const VxColor &c, float size, float life, float fadetime, CKBOOL last, CK_ID material, float iOffset)
{
    VxVector point(p);
    VxVector normal(n);
    if (ref)
    {
        ref->Transform(&point, &p);
        ref->TransformVector(&normal, &n);
    }
    if (normal == VxVector::axis0())
    {
        normal = VxVector::axisY();
    }
    // just in case...
    normal.Normalize();

    point += normal * iOffset;

    MaterialMark &mark = *(m_MaterialMarks.InsertUnique(material, MaterialMark()));

    VxVector lastpos;
    Mark *oldmark = mark.m_Marks;
    CKBOOL first = TRUE;
    CKBOOL second = FALSE;

    if (oldmark)
    { // We have to test if we have move enough
        if (oldmark->GetStyle() != MARKBANDEND)
        { // we are not the first
            if (oldmark->GetStyle() == MARKBANDSTART)
            { // we are the second
                second = TRUE;
            }
            first = FALSE;
            lastpos = oldmark->m_Positions[2];
            if (SquareMagnitude(oldmark->m_Positions[2] - point) <= 0.01f)
            { // Same position
                // if (last) oldmark->m_Mode = MARKBANDEND; ?????????????????
                return;
            }
        }
    }

    if (first && last)
        return;

    // new mark
    Mark *m = NULL;

    if (!m_FreeMarks)
    { // No more marks free
        // we'll have to pick from oldest existing marks...
        m = FreeLastMark(mark);
    }
    else
    {
        m = m_FreeMarks;
        m_FreeMarks = m->m_Next;

        ++m_MarksNumber;
    }

    // we insert the mark in the active mark list
    AddNewMark(mark, m);
    /*
        // we insert the mark in the active mark list
        if (mark.m_Marks) mark.m_Marks->m_Prev = m;
        m->m_Next			= mark.m_Marks;
        m->m_Prev			= NULL;
        mark.m_Marks		= m;
        */

    // the halfsize is enough
    size *= 0.5f;

    VxVector dir;
    if (first)
    {
        dir = VxVector::axisY();
    }
    else
    { // not the first
        dir = Normalize(point - lastpos);
    }

    VxVector v = Normalize(CrossProduct(dir, normal));

    // adjustement with size
    v *= size;

    // Positions
    m->m_Positions[0] = point + v;
    m->m_Positions[1] = point - v;
    m->m_Positions[2] = point; // for future use

    // Life
    m->m_Lifespan = life;
    m->m_FadeTime = fadetime;

    // we must now reshape the oldposition
    if (second)
    {
        oldmark->m_Positions[0] = oldmark->m_Positions[2] + v;
        oldmark->m_Positions[1] = oldmark->m_Positions[2] - v;
    }

    // Uvs
    if (!first)
    {
        float uvstart = (mark.m_Marks->m_Next->m_UVs.left + mark.m_Marks->m_Next->m_UVs.right) * 0.5f;
        VxVector startpos = mark.m_Marks->m_Next->m_Positions[2];
        VxVector dir = Normalize(m->m_Positions[2] - startpos);
        m->m_UVs.left = uvstart + DotProduct(m->m_Positions[1] - startpos, dir);
        m->m_UVs.right = uvstart + DotProduct(m->m_Positions[0] - startpos, dir);
    }
    else
    { // first point
        m->m_UVs.left = 0.0f;
        m->m_UVs.right = 0.0f;
    }

    // colors
    m->m_Color = c;
    m->m_ColorPacked = RGBAFTOCOLOR(&c);
    m->m_DeltaAlpha = -c.a / fadetime;

    // Position in the band
    if (first)
        m->m_Mode = MARKBANDSTART;
    else if (last)
        m->m_Mode = MARKBANDEND;
    else
        m->m_Mode = MARKBANDIN;
}

void MarkSystem::AddSingleMark(const VxVector &p, const VxVector &n, CK3dEntity *ref, const VxColor &c, float size, float life, CK_ID material, float fadetime, const VxRect &uvs, float iOffset)
{
    // Getting the material mark
    MaterialMark &mark = *(m_MaterialMarks.InsertUnique(material, MaterialMark()));

    // new mark
    Mark *m = NULL;

    if (!m_FreeMarks)
    { // No more marks free
        // we'll have to pick from oldest existing marks...
        m = FreeLastMark(mark);
    }
    else
    {
        m = m_FreeMarks;
        m_FreeMarks = m->m_Next;
        ++m_MarksNumber;
    }

    // we insert the mark in the active mark list
    AddNewMark(mark, m);

    VxVector point(p);
    VxVector normal(n);

    // we store the referential, for the mark to be attached to it
    m->m_CheckEntity = ref;
    m->m_Entity = CKOBJID(ref);

    VxVector entscale(1.0f, 1.0f, 1.0f);
    if (ref)
    {
        ref->GetScale(&entscale, FALSE);
        iOffset /= Magnitude(entscale);
        entscale = VxVector(1.0f) / entscale;
    }

    point += normal * iOffset;

    // the halfsize is enough
    size *= 0.5f;

    VxVector v;

    if (ISZERO(normal.x) && ISZERO(normal.z))
        v = VxVector::axisZ();
    else
        v = Normalize(CrossProduct(VxVector::axisY(), normal));
    VxVector w = Normalize(CrossProduct(normal, v));
    // adjustement with size
    v *= size;
    w *= size;
    if (ref)
    {
        v *= entscale;
        w *= entscale;
    }

    // Positions
    m->m_Positions[0] = point + v + w;
    m->m_Positions[1] = point - v + w;
    m->m_Positions[2] = point + v - w;
    m->m_Positions[3] = point - v - w;

    // Life
    m->m_Lifespan = life;
    m->m_FadeTime = fadetime;

    // colors
    m->m_Color = c;
    m->m_ColorPacked = RGBAFTOCOLOR(&c);
    m->m_DeltaAlpha = -c.a / fadetime;

    // Last One ? for the moment no
    m->m_Mode = MARKSINGLE;

    // UVs
    if (uvs.IsEmpty())
    {
        m->m_Mode |= STANDARDUVS;
    }
    else
    {
        m->m_UVs = uvs;
    }
}

CKObjectDeclaration *FillBehaviorMarkSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mark System");
    od->SetDescription("Creates marks (single or continuous) on surfaces.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the building block.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the building block.<BR>
    <SPAN CLASS=in>Mark: </SPAN>creates a new mark.<BR>
    <SPAN CLASS=in>End Mark: </SPAN>ends a mark band.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when On is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when Off is activated.<BR>
    <SPAN CLASS=out>Exit Mark: </SPAN>is activated when Mark is activated.<BR>
    <SPAN CLASS=out>Exit End Mark: </SPAN>is activated when End Mark is activated.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>Position of the mark.<BR>
    <SPAN CLASS=pin>Normal: </SPAN>Normal of the mark.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>Referential in which the position and normal are provided.<BR>
    <SPAN CLASS=pin>Color and Alpha: </SPAN>Color and alpha of the mark.<BR>
    <SPAN CLASS=pin>Size: </SPAN>Color and alpha of the mark.<BR>
    <SPAN CLASS=pin>Material: </SPAN>Material used by the marks (not considering the colors).<BR>
    <SPAN CLASS=pin>Lifespan: </SPAN>Life span of the marks.<BR>
    <SPAN CLASS=pin>Texture Factor:</SPAN>Texture mapping factor.<BR>
    <SPAN CLASS=pin>Fading Time:</SPAN>Duration of the mark fading.<BR>
    <SPAN CLASS=pin>UVs:</SPAN>Texture coordinates of the mark. (0,0,0,0) means the whole texture.<BR>
    <BR>
    <SPAN CLASS=setting>Maximum Marks:</SPAN>Maximum number of simultaneous marks.<BR>
    <SPAN CLASS=setting>Continuous Marks:</SPAN>Should the mark be continuous or not (it adds inputs and parameters).<BR>
    <SPAN CLASS=setting>Render Frame:</SPAN>If a 3D Frame is given, the marks will be rendered just after it, otherwise, rendering will be done
    after all the objects.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9a06075, 0x760a4720));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMarkSystemProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

#define I_POSITION 0
#define I_NORMAL 1
#define I_REFERENTIAL 2
#define I_COLOR 3
#define I_SIZE 4
#define I_MATERIAL 5
#define I_LIFESPAN 6
#define I_FADINGTIME 7
#define I_UVS 8
#define I_OFFSET 9
#define I_FACTOR 10

CKERROR CreateMarkSystemProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Mark System");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Mark");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Exit Mark");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Normal", CKPGUID_VECTOR, "0,1,0");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Color and Alpha", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Size", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Lifespan", CKPGUID_TIME, "0m 5s 0ms");
    proto->DeclareInParameter("Fading Time", CKPGUID_TIME, "0m 1s 0ms");
    proto->DeclareInParameter("UVs", CKPGUID_RECT, "0,0,0,0");
    proto->DeclareInParameter("Position Offset", CKPGUID_FLOAT, "0");

    proto->DeclareLocalParameter(NULL, CKPGUID_POINTER); // Marks Data
    proto->DeclareSetting("Maximum Marks", CKPGUID_INT, "200");
    proto->DeclareSetting("Continuous Marks", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Supporting Entity", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MarkSystemFun);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));

    proto->SetBehaviorCallbackFct(MarkSystemCallback);

    *pproto = proto;
    return CK_OK;
}

int MarkSystemFun(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    MarkSystem *marks = *(MarkSystem **)beh->GetLocalParameterReadDataPtr(0);

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        marks->Clear();
    }
    else
    { // we enter by off
        if (beh->IsInputActive(1))
        { // We enter by 'ON'
            beh->ActivateInput(1, FALSE);
            beh->ActivateOutput(1);

            return CKBR_OK;
        }
    }

    CK3dEntity *renderframe = (CK3dEntity *)beh->GetLocalParameterObject(3);
    if (renderframe)
    {

        if (renderframe->GetClassID() == CKCID_3DENTITY)
        {
            VxBbox box;
            box.Max.Set(10000.0f, 10000.0f, 10000.0f);
            box.Min.Set(-10000.0f, -10000.0f, -10000.0f);
            renderframe->SetBoundingBox(&box, TRUE);
        }
        renderframe->AddPostRenderCallBack(RenderMarkFrame, (void *)beh->GetID(), TRUE);
    }
    else
    {
        // we add the render callback
        behcontext.CurrentRenderContext->AddPostRenderCallBack(MarkSystem::RenderMarkSystem, (void *)beh->GetID(), TRUE);
    }

    CKBOOL endmark = FALSE;

    if (beh->IsInputActive(3))
    { // Finish a mark
        beh->ActivateInput(3, FALSE);
        beh->ActivateOutput(3);

        endmark = TRUE;
    }

    if (endmark || beh->IsInputActive(2))
    { // New mark
        if (beh->IsInputActive(2))
        {
            beh->ActivateInput(2, FALSE);
            beh->ActivateOutput(2);
        }

        VxVector p;
        beh->GetInputParameterValue(I_POSITION, &p);

        CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(I_REFERENTIAL);

        VxVector n;
        beh->GetInputParameterValue(I_NORMAL, &n);

        VxColor c(1.0f, 1.0f, 1.0f, 1.0f);
        beh->GetInputParameterValue(I_COLOR, &c);

        float size = 1.0f;
        beh->GetInputParameterValue(I_SIZE, &size);

        float life = 5000.0f;
        beh->GetInputParameterValue(I_LIFESPAN, &life);

        float offset = 0.0f;
        beh->GetInputParameterValue(I_OFFSET, &offset);

        CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(I_MATERIAL);

        // fading Time
        float ftime = 1000.0f;
        beh->GetInputParameterValue(I_FADINGTIME, &ftime);

        // UVs
        VxRect uvs;
        beh->GetInputParameterValue(I_UVS, &uvs);

        CKBOOL cm = FALSE;
        beh->GetLocalParameterValue(2, &cm);

        if (cm)
            marks->AddContinuousMark(p, n, ref, c, size, life, ftime, endmark, CKOBJID(mat), offset);
        else
            marks->AddSingleMark(p, n, ref, c, size, life, CKOBJID(mat), ftime, uvs, offset);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR MarkSystemCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        MarkSystem *marks = new MarkSystem;
        beh->SetLocalParameterValue(0, &marks);
    }
    case CKM_BEHAVIORRESET:
    {
        MarkSystem *marks = *(MarkSystem **)beh->GetLocalParameterReadDataPtr(0);
        marks->Clear();
    }
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        MarkSystem *marks = *(MarkSystem **)beh->GetLocalParameterReadDataPtr(0);

        int n = 200;
        beh->GetLocalParameterValue(1, &n);

        marks->CheckSize(n);

        int cm = FALSE;
        beh->GetLocalParameterValue(2, &cm);

        if (cm)
        { // we want continuous marks
            if (beh->GetInputCount() == 3)
            { // there wasn't created yet
                beh->AddInput("End Mark");
                beh->AddOutput("Exit End Mark");

                beh->CreateInputParameter("Texture Scale", CKPGUID_FLOAT);
            }
        }
        else
        { // single marks
            if (beh->GetInputCount() == 4)
            { // there is something to delete
                beh->DeleteInput(2);
                beh->DeleteOutput(2);
                CKParameterIn *pin = beh->RemoveInputParameter(I_FACTOR);
                behcontext.Context->DestroyObject(CKOBJID(pin));
            }
        }
    }
    break;
    case CKM_BEHAVIORDELETE:
    {
        MarkSystem *marks = *(MarkSystem **)beh->GetLocalParameterReadDataPtr(0);
        delete marks;
    }
    break;
    }
    return CKBR_OK;
}

/*******************************************************/
/****************** RENDER FUNCTIONS *******************/
void MarkSystem::RenderMarkSystem(CKRenderContext *dev, void *arg)
{
    static float singleuvs[8] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

    CKContext *ctx = dev->GetCKContext();
    CKBehavior *beh = (CKBehavior *)ctx->GetObject((CK_ID)arg);
    if (!beh)
        return;

    MarkSystem *marks = *(MarkSystem **)beh->GetLocalParameterReadDataPtr(0);
    if (!marks)
        return;

    // we update the marks
    marks->Update(ctx->GetTimeManager()->GetLastDeltaTime());

    ///
    // Prepare the drawPrimitive
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * marks->m_MarksNumber);
    CKWORD *IndicesPtr = dev->GetDrawPrimitiveIndices(6 * marks->m_MarksNumber);
    CKWORD *indices = IndicesPtr;

    dev->SetWorldTransformationMatrix(VxMatrix::Identity());

    // We render all the marks of each material

    XSHashTableMarksIt end = marks->m_MaterialMarks.End();
    for (XSHashTableMarksIt begin = marks->m_MaterialMarks.Begin(); begin != end; ++begin)
    {
        MaterialMark &mm = *begin;

        ///
        // Render STATES
        CKMaterial *mat = (CKMaterial *)ctx->GetObject(begin.GetKey());

        if (mat)
            mat->SetAsCurrent(dev, FALSE);
        dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
        dev->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        VxUV *uvs = (VxUV *)data->TexCoordPtr;
        VxVector4 *positions = (VxVector4 *)data->PositionPtr;
        CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
        VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
        VxVector4 *positions = (VxVector4 *)data->Positions.Ptr;
        CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

        Mark *m = mm.m_Marks;
        int vertexcount = 0;
        int indicescount = 0;
        CKBOOL waslast = TRUE;

        float factor = 0.2f;
        beh->GetInputParameterValue(I_FACTOR, &factor);
        // TODO read from parameter

        VxStridedData src, dst;
        src.Stride = sizeof(VxVector);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        dst.Stride = data->PositionStride;
#else
        dst.Stride = data->Positions.Stride;
#endif

        // the filling
        while (m)
        {
            if (m->GetStyle() == MARKSINGLE)
            {
                // Positions
                if (m->m_Entity)
                { // Local
                    CK3dEntity *ref = (CK3dEntity *)ctx->GetObject(m->m_Entity);
                    if (ref != m->m_CheckEntity)
                    {						   // The referential has been destroyed
                        m->m_Lifespan = -1.0f; // Mark as dead
                        m = m->m_Next;		   // next mark
                        continue;
                    }

                    if (ref->IsAllOutsideFrustrum())
                    {				   // If the entity is not rendered, not render the mark neither
                        m = m->m_Next; // next mark
                        continue;
                    }

                    src.Ptr = m->m_Positions;
                    dst.Ptr = positions;

                    Vx3DMultiplyMatrixVectorStrided(&dst, &src, ref->GetWorldMatrix(), 4);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    positions = (VxVector4 *)((CKBYTE *)positions + 4 * data->PositionStride);
#else
                    positions = (VxVector4 *)((CKBYTE *)positions + 4 * data->Positions.Stride);
#endif
                }
                else
                {
                    // World position
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    VxCopyStructure(4, positions, data->PositionStride, sizeof(VxVector), &m->m_Positions[0], sizeof(VxVector));
                    positions = (VxVector4 *)((CKBYTE *)positions + 4 * data->PositionStride);
#else
                    VxCopyStructure(4, positions, data->Positions.Stride, sizeof(VxVector), &m->m_Positions[0], sizeof(VxVector));
                    positions = (VxVector4 *)((CKBYTE *)positions + 4 * data->Positions.Stride);
#endif
                }

                // Colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                VxFillStructure(4, colors, data->ColorStride, sizeof(CKDWORD), &m->m_ColorPacked);
                colors = (CKDWORD *)((CKBYTE *)colors + 4 * data->ColorStride);
#else
                VxFillStructure(4, colors, data->Colors.Stride, sizeof(CKDWORD), &m->m_ColorPacked);
                colors = (CKDWORD *)((CKBYTE *)colors + 4 * data->Colors.Stride);
#endif
                // Uvs
                if (m->m_Mode & STANDARDUVS)
                {
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    VxCopyStructure(4, uvs, data->TexCoordStride, sizeof(VxUV), singleuvs, sizeof(VxUV));
                    uvs = (VxUV *)((CKBYTE *)uvs + 4 * data->TexCoordStride);
#else
                    VxCopyStructure(4, uvs, data->TexCoord.Stride, sizeof(VxUV), singleuvs, sizeof(VxUV));
                    uvs = (VxUV *)((CKBYTE *)uvs + 4 * data->TexCoord.Stride);
#endif
                }
                else
                {
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                    const unsigned int stride = data->TexCoordStride;
#else
                    const unsigned int stride = data->TexCoord.Stride;
#endif
                    uvs->u = m->m_UVs.left;
                    uvs->v = m->m_UVs.top;
                    uvs = (VxUV *)((CKBYTE *)uvs + stride);
                    uvs->u = m->m_UVs.right;
                    uvs->v = m->m_UVs.top;
                    uvs = (VxUV *)((CKBYTE *)uvs + stride);
                    uvs->u = m->m_UVs.left;
                    uvs->v = m->m_UVs.bottom;
                    uvs = (VxUV *)((CKBYTE *)uvs + stride);
                    uvs->u = m->m_UVs.right;
                    uvs->v = m->m_UVs.bottom;
                    uvs = (VxUV *)((CKBYTE *)uvs + stride);
                }

                // Indices
                indices[indicescount++] = vertexcount;
                indices[indicescount++] = vertexcount + 1;
                indices[indicescount++] = vertexcount + 2;
                indices[indicescount++] = vertexcount + 1;
                indices[indicescount++] = vertexcount + 3;
                indices[indicescount++] = vertexcount + 2;

                vertexcount += 4;
            }
            else
            {
                // Positions
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                VxCopyStructure(2, positions, data->PositionStride, sizeof(VxVector), &m->m_Positions[0], sizeof(VxVector));
                positions = (VxVector4 *)((CKBYTE *)positions + 2 * data->PositionStride);
#else
                VxCopyStructure(2, positions, data->Positions.Stride, sizeof(VxVector), &m->m_Positions[0], sizeof(VxVector));
                positions = (VxVector4 *)((CKBYTE *)positions + 2 * data->Positions.Stride);
#endif

                // Colors
                CKDWORD col = RGBAFTOCOLOR(&m->m_Color);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                VxFillStructure(2, colors, data->ColorStride, sizeof(CKDWORD), &col);
                colors = (CKDWORD *)((CKBYTE *)colors + 2 * data->ColorStride);
#else
                VxFillStructure(2, colors, data->Colors.Stride, sizeof(CKDWORD), &col);
                colors = (CKDWORD *)((CKBYTE *)colors + 2 * data->Colors.Stride);
#endif

                // Uvs
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                // Vertex 0
                uvs->u = 0.0f;
                uvs->v = factor * m->m_UVs.left;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
                // Vertex 1
                uvs->u = 1.0f;
                uvs->v = factor * m->m_UVs.right;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
                // Vertex 0
                uvs->u = 0.0f;
                uvs->v = factor * m->m_UVs.left;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
                // Vertex 1
                uvs->u = 1.0f;
                uvs->v = factor * m->m_UVs.right;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

                // indices
                if (m->GetStyle() != MARKBANDSTART)
                {
                    indices[indicescount++] = vertexcount;
                    indices[indicescount++] = vertexcount + 2;
                    indices[indicescount++] = vertexcount + 1;
                    indices[indicescount++] = vertexcount + 2;
                    indices[indicescount++] = vertexcount + 3;
                    indices[indicescount++] = vertexcount + 1;
                }
                else
                {
                    int a = 5;
                }

                vertexcount += 2;
            }

            m = m->m_Next;
        }

        // the drawing itself
        if (vertexcount > 2)
        {
            VxMatrix projmat = dev->GetProjectionTransformationMatrix();
            float oldproj = projmat[3].z;
            projmat[3].z = oldproj * 1.001f; // to disable Z buffer fighting
            dev->SetProjectionTransformationMatrix(projmat);

            data->VertexCount = vertexcount;
            dev->DrawPrimitive(VX_TRIANGLELIST, IndicesPtr, indicescount, data);

            projmat[3].z = oldproj; // restore projection matrix
            dev->SetProjectionTransformationMatrix(projmat);
        }
    }
}
