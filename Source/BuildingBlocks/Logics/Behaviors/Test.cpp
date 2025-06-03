/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Test
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTestDecl();
CKERROR CreateTestProto(CKBehaviorPrototype **);
int Test(const CKBehaviorContext &behcontext);

CKERROR TestCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

typedef CKBOOL (*OpFctTest)(float a, float b);
typedef CKBOOL (*StringOpFctTest)(CKSTRING a, CKSTRING b);

CKObjectDeclaration *FillBehaviorTestDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Test");
    od->SetDescription("Activates the appropriate output, according to the test between A and B.");
    /* rem:
    <SPAN CLASS=pin>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated when Test succeed.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated when Test failed.<BR>
    <BR>
    <SPAN CLASS=pin>Test: </SPAN>Test to be done between A and B.<BR>
    <SPAN CLASS=pin>A: </SPAN>first value.<BR>
    <SPAN CLASS=pin>B: </SPAN>second value.<BR>
    <BR>
    A and B could be either FLOAT values, or INTEGER values, or the ID of an object.<BR>
    When you can, use this building block rather than equivalent Parameter Operation that binds you to combine a 'Binary Switch' to activate a output.<BR>
    For instance, if <FONT COLOR=#000077>Test</FONT> is 'Greater Than', <FONT COLOR=#000077>A</FONT>=5 and <FONT COLOR=#000077>B</FONT>=2, then Behavior <FONT COLOR=#000077>True</FONT> is activated.<BR>
    */
    /* warning:
    - If you want to test equality between two objects like Meshes, Vectors or other things, you'd better use the 'Switch On Parameter' Building Block.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17d66d26, 0x726b7dec));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTestProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Test");
    return od;
}

/************************************************/
/*            Functions of Test                 */
/************************************************/
//______________________________________________/
// Equal=1, Not Equal=2, Lesser than=3, Lesser or equal than=4, Greater than=5, Greater or equal than=6;
OpFctTest fcttabtest[6];
StringOpFctTest string_fcttabtest[6];

//____________________________________/ EQUAL
CKBOOL fcttabtest_Equal(float a, float b)
{
    if (a == b)
        return TRUE;
    return FALSE;
}

//____________________________________/ NOT EQUAL
CKBOOL fcttabtest_NotEqual(float a, float b)
{
    if (a != b)
        return TRUE;
    return FALSE;
}

//____________________________________/ LESS THAN
CKBOOL fcttabtest_Less(float a, float b)
{
    if (a < b)
        return TRUE;
    return FALSE;
}

//____________________________________/ LESS OR EQUAL
CKBOOL fcttabtest_LessEqual(float a, float b)
{
    if (a <= b)
        return TRUE;
    return FALSE;
}

//____________________________________/ GREATER THAN
CKBOOL fcttabtest_Greater(float a, float b)
{
    if (a > b)
        return TRUE;
    return FALSE;
}

//____________________________________/ GREATER THAN
CKBOOL fcttabtest_GreaterEqual(float a, float b)
{
    if (a >= b)
        return TRUE;
    return FALSE;
}

// String

int ICompare(CKSTRING a, CKSTRING b)
{
    if (!a)
    {
        if (!b)
        {
            return 0;
        }
        else
            return -1;
    }
    else
    {
        if (!b)
        {
            return 1;
        }
        else
        {
            return strcmp(a, b);
        }
    }
}

CKBOOL string_fcttabtest_Equal(CKSTRING a, CKSTRING b)
{
    if (ICompare(a, b) == 0)
        return TRUE;
    return FALSE;
}

//____________________________________/ NOT EQUAL
CKBOOL string_fcttabtest_NotEqual(CKSTRING a, CKSTRING b)
{
    if (ICompare(a, b) != 0)
        return TRUE;
    return FALSE;
}

//____________________________________/ LESS THAN
CKBOOL string_fcttabtest_Less(CKSTRING a, CKSTRING b)
{
    if (ICompare(a, b) < 0)
        return TRUE;
    return FALSE;
}

//____________________________________/ LESS OR EQUAL
CKBOOL string_fcttabtest_LessEqual(CKSTRING a, CKSTRING b)
{
    if (ICompare(a, b) <= 0)
        return TRUE;
    return FALSE;
}

//____________________________________/ GREATER THAN
CKBOOL string_fcttabtest_Greater(CKSTRING a, CKSTRING b)
{
    if (ICompare(a, b) > 0)
        return TRUE;
    return FALSE;
}

//____________________________________/ GREATER OR EQUAL
CKBOOL string_fcttabtest_GreaterEqual(CKSTRING a, CKSTRING b)
{
    if (ICompare(a, b) >= 0)
        return TRUE;
    return FALSE;
}

/************************************************/
/*                Proto                         */
/************************************************/
CKERROR CreateTestProto(CKBehaviorPrototype **pproto)
{
    fcttabtest[0] = fcttabtest_Equal;
    fcttabtest[1] = fcttabtest_NotEqual;
    fcttabtest[2] = fcttabtest_Less;
    fcttabtest[3] = fcttabtest_LessEqual;
    fcttabtest[4] = fcttabtest_Greater;
    fcttabtest[5] = fcttabtest_GreaterEqual;

    string_fcttabtest[0] = string_fcttabtest_Equal;
    string_fcttabtest[1] = string_fcttabtest_NotEqual;
    string_fcttabtest[2] = string_fcttabtest_Less;
    string_fcttabtest[3] = string_fcttabtest_LessEqual;
    string_fcttabtest[4] = string_fcttabtest_Greater;
    string_fcttabtest[5] = string_fcttabtest_GreaterEqual;

    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Test");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Test", CKPGUID_COMPOPERATOR, "Equal");
    proto->DeclareInParameter("A", CKPGUID_FLOAT);
    proto->DeclareInParameter("B", CKPGUID_FLOAT);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Test);

    proto->SetBehaviorCallbackFct(TestCallBack);

    *pproto = proto;
    return CK_OK;
}

/************************************************/
/*            Behavior Function                 */
/************************************************/
int Test(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKParameterManager *pm = behcontext.ParameterManager;

    beh->ActivateInput(0, FALSE);

    //____________________________/ Get Test
    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(0, &op);

    //____________________________/ Get Inputs
    float val[2];
    CKSTRING str[2] = {0, 0};
    CKGUID guid;
    bool string = false;
    for (int a = 0; a < 2; a++)
    {
        CKParameterIn *pin = beh->GetInputParameter(a + 1);
        guid = pin->GetGUID();
        if (!pm->IsDerivedFrom(guid, CKPGUID_FLOAT))
        {
            if (guid == CKPGUID_STRING)
            {
                str[a] = (CKSTRING)pin->GetReadDataPtr();
                string |= true;
            }
            else
            {
                int intval = 0;
                pin->GetValue(&intval);
                if (guid == CKPGUID_BOOL)
                {
                    val[a] = (intval ? 1.0f : 0.0f);
                }
                else
                {
                    val[a] = (float)intval;
                }
            }
        }
        else
        {
            pin->GetValue(&val[a]);
        }
    }

    //____________________________/ Process Test
    if (op > 0)
    {
        CKBOOL k = FALSE;

        if (!string)
        {
            k = fcttabtest[op - 1](val[0], val[1]);
        }
        else
        {
            k = string_fcttabtest[op - 1](str[0], str[1]);
        }

        if (k)
            beh->ActivateOutput(0);
        else
            beh->ActivateOutput(1);
    }
    return CKBR_OK;
}

/**************************************************/
/*                 CALLBACK                       */
/**************************************************/
CKERROR TestCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        CKParameterManager *pm = behcontext.ParameterManager;

        CKGUID guid;
        for (int a = 1; a < 3; ++a)
        {
            if (pin = beh->GetInputParameter(a))
            {
                guid = pin->GetGUID();
                if (!pm->IsDerivedFrom(guid, CKPGUID_FLOAT) && !pm->IsDerivedFrom(guid, CKPGUID_STRING) && !pm->IsDerivedFrom(guid, CKPGUID_INT) && !pm->IsDerivedFrom(guid, CKPGUID_OBJECT))
                {
                    pin->SetGUID(CKPGUID_FLOAT, TRUE);
                }
            }
        }
    }
    break;
    }

    return CKBR_OK;
}
