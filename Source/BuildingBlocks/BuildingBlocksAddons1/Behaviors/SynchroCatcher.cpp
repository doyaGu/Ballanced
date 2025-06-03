/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SynchroCatcher
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSynchroCatcherDecl();
CKERROR CreateSynchroCatcherProto(CKBehaviorPrototype **);
int SynchroCatcher(const CKBehaviorContext &context);
CKERROR SynchroCatcherCallBackObject(const CKBehaviorContext &behcontext);

// Inner struct
typedef struct
{
    CKDataArray *da; // ptr to synchro array
    float last_time; // last real absolute time
    float time;      // initial real absolute time
    float delay;     // duration
    CKBOOL state;
    // TRUE = we wait for a key to be pressed
    // FALSE = we wait any the key to be released
} A_SYNCHROCATCHER;
//---

CKObjectDeclaration *FillBehaviorSynchroCatcherDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Synchro Recorder");
    od->SetDescription("Records some input keys events by putting them in a data array which could used with 'Synchro Player' building block.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>triggers the process (use [ESC] key to stop the synchro recording, or Reset Initial conditions).<BR>
    <BR>
    <SPAN CLASS=pin>Synchro Array Name: </SPAN>name of the created data array in which synchro informations are stored.<BR>
    <SPAN CLASS=pin>Min: </SPAN>lower end of the range covered by the input keys value.<BR>
    <SPAN CLASS=pin>Max: </SPAN>higher end of the range covered by the input keys value.<BR>
    <SPAN CLASS=pin>Key 0: </SPAN>first key of the synchroneous process.<BR>
    <SPAN CLASS=pin>Key 1: </SPAN>secind key of the synchroneous process.<BR>
    <SPAN CLASS=pin>... </SPAN><BR>
    <SPAN CLASS=pin>Key N: </SPAN>...<BR>
    <BR>
    Records some input keys events by putting them in a data array.
    [ESC] = stops synchro recording.<BR>
    A new line is added in the created data array each time you press one of the keys specified as parameters. <BR>
    Colums of the synchro array:<BR>
    - Time: the time ellapsed between the ON activation and the key pressed.<BR>
    - Amplitude: value between 'Min' and 'Max' depending on the key pressed. If 'Key 0'=E, 'Key 1'=R and 'Key 2'=T, then if Min=0 and Max=3, pressing 'E' value=1, pressing 'R' value=2, pressing 'T' value=3.<BR>
    Of course, 'amplitude' value is a Float, therefore you can have non-integer values.<BR>
    - Length: Duration of key pressing (in milliseconds).<BR>
    <BR>
    <BR>
    See Also: "Synchro Player" BB.<BR>
    */
    /* warning:
    - if [ESC] is not pressed at all, the last line (the one with a null amplitude ) is added when the RESET event occures.<BR>
    - if you want to produce music synchronizations, you should make sure you didn't record your synchronization sequence with a Time Multiplication Factor different to 1.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetCategory("Narratives/Streaming Recording");
    od->SetGuid(CKGUID(0x62ea3812, 0x1ed661a6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSynchroCatcherProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateSynchroCatcherProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Synchro Recorder");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareInParameter("Synchro Array Name", CKPGUID_STRING, "Synchro Array");
    proto->DeclareInParameter("Min", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Max", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Key 0", CKPGUID_KEY);

    proto->DeclareOutParameter("Synchro Array", CKPGUID_DATAARRAY);

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // inner struct SYNCHROCATCHER

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(SynchroCatcher);
    proto->SetBehaviorCallbackFct(SynchroCatcherCallBackObject);

    *pproto = proto;
    return CK_OK;
}

/***********************************************/
/*         A_GenerateSynchroCurve              */
/***********************************************/
void A_GenerateSynchroCurve(CKContext *ctx, A_SYNCHROCATCHER *sc, CKDataArray *da, float val_min, float val_max, CKBehavior *beh)
{
    CKTimeManager *tm = ctx->GetTimeManager();

    float final_time = sc->last_time - sc->time;
    int count = da->GetRowCount();
    /* we generate no more curve
    //---
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(0,&curve);

    CK2dCurvePoint *cp;
    int count = curve->GetControlPointCount();

    for( int a=1 ; a<(count-1) ; a++ ){
    cp = curve->GetControlPoint( 1 );
    curve->RemoveControlPoint( cp );
    }

    Vx2DVector pos(0,0);
    cp = curve->GetControlPoint( 0 );
    cp->SetPosition( &pos );
    pos.x = 1;
    cp = curve->GetControlPoint( 1 );
    cp->SetPosition( &pos );
    //---

    count = da->GetLineCount();
    int time, delay;

    float amplitude = 0;
    float s = 1.0f / ( val_max - val_min );
    Vx2DVector tangent_in( -0.1f, 0 );
    Vx2DVector tangent_out( 0.1f, 0 );

    float inv_max_time = 1.0f / final_time;

    for( int i=0 ; i<count ; i++ ){

    da->GetElement( i, 0, &time );
    pos.x = (float)time * inv_max_time;
    pos.y = 0;
    cp = CreateCK2dCurvePoint( NULL );
    cp->UseTCB( FALSE );
    cp->SetTangents( &tangent_in, &tangent_out );
    cp->SetPosition( &pos );
    curve->AddControlPoint( cp );

    da->GetElement( i, 2, &delay );
    pos.x = (float)(time+delay*0.33f) * inv_max_time;
    da->GetElement( i, 1, &amplitude );
    pos.y = (amplitude - val_min) * s ;
    cp = CreateCK2dCurvePoint( NULL );
    cp->UseTCB( FALSE );
    cp->SetTangents( &tangent_in, &tangent_out );
    cp->SetPosition( &pos );
    curve->AddControlPoint( cp );

    pos.x = (float)(time+delay) * inv_max_time;;
    pos.y = 0;
    cp = CreateCK2dCurvePoint( NULL );
    cp->UseTCB( FALSE );
    cp->SetTangents( &tangent_in, &tangent_out );
    cp->SetPosition( &pos );
    curve->AddControlPoint( cp );
    }
    */
    da->InsertRow();
    da->SetElementValue(count, 0, &final_time);
    float amplitude = 0;
    da->SetElementValue(count, 1, &amplitude);
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR SynchroCatcherCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        A_SYNCHROCATCHER *sc = new A_SYNCHROCATCHER;
        sc->da = NULL;
        beh->SetLocalParameterValue(0, &sc, sizeof(sc));
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        A_SYNCHROCATCHER *sc = NULL;
        beh->GetLocalParameterValue(0, &sc);
        if (sc)
        {

            delete sc;
            sc = NULL;
        }
    }
    break;

    case CKM_BEHAVIOREDITED:
    {
        char str[64];
        int c_pin = beh->GetInputParameterCount();
        CKParameterIn *pin;
        CKParameter *pout;

        for (int a = 3; a < c_pin; a++)
        { // we check the type of each 'Input Parameter'
            pin = beh->GetInputParameter(a);
            if (pin->GetGUID() != CKPGUID_KEY)
            {
                pin->SetGUID(CKPGUID_KEY);
                sprintf(str, "Key %d", a - 3);
                pin->SetName(str);
                if (pout = pin->GetRealSource())
                {
                    pout->SetGUID(CKPGUID_KEY);
                }
            }
        }
    }
    break;

    case CKM_BEHAVIORRESET:
    {
        A_SYNCHROCATCHER *sc = NULL;
        beh->GetLocalParameterValue(0, &sc);
        CKDataArray *da;
        if (!sc || !(da = sc->da))
            return CKBR_OK;

        int amplitude = 0;
        int count = da->GetRowCount();
        if (!count)
            return CKBR_OK;
        da->GetElementValue(da->GetRowCount() - 1, 1, &amplitude);
        if (amplitude)
        {

            float val_min = 0.0f; // get val min
            beh->GetInputParameterValue(1, &val_min);

            float val_max = 1.0f; // get val max
            beh->GetInputParameterValue(2, &val_max);

            A_GenerateSynchroCurve(beh->GetCKContext(), sc, da, val_min, val_max, beh);
        }
    }
    break;
    }

    return CKBR_OK;
}

/***********************************************/
/*               Main Function                 */
/***********************************************/
int SynchroCatcher(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKTimeManager *tm = ctx->GetTimeManager();

    A_SYNCHROCATCHER *sc;
    beh->GetLocalParameterValue(0, &sc);
    if (!sc)
        return CKBR_OK;

    /*
    CKDataArray *da = (CKDataArray *)beh->GetInputParameterObject(0);
    if( !da ) return CKBR_PARAMETERERROR;
    */
    CKDataArray *da = sc->da;

    if (beh->IsInputActive(0))
    {
        // enter by ON
        beh->ActivateInput(0, FALSE);

        char *array_name = (char *)beh->GetInputParameterReadDataPtr(0);
        if (!sc->da || strcmp(sc->da->GetName(), array_name))
        {
            sc->da = (CKDataArray *)ctx->GetObjectByNameAndClass(array_name, CKCID_DATAARRAY, NULL);
            if (!sc->da)
            {
                sc->da = (CKDataArray *)ctx->CreateObject(CKCID_DATAARRAY, array_name);
                behcontext.CurrentLevel->AddObject(sc->da);
            }
            da = sc->da;
        }
        // clear data array
        da->Clear();
        while (da->GetColumnCount())
            da->RemoveColumn(0);

        // create data array format
        da->InsertColumn(-1, CKARRAYTYPE_FLOAT, "Time");
        da->InsertColumn(-1, CKARRAYTYPE_FLOAT, "Amplitude");
        da->InsertColumn(-1, CKARRAYTYPE_FLOAT, "Length");

        // init inner variable
        sc->time = tm->GetTime();
        sc->state = TRUE;
    }

    // memorize last time (for Reset event)
    sc->last_time = tm->GetTime();

    // get input key
    CKInputManager *input = (CKInputManager *)ctx->GetManagerByGuid(INPUT_MANAGER_GUID);
    int key;

    float val_min = 0.0f; // get val min
    beh->GetInputParameterValue(1, &val_min);

    float val_max = 1.0f; // get val max
    beh->GetInputParameterValue(2, &val_max);

    // test if End [ ESCAPE ]
    //---
    if (input->IsKeyDown(0x01))
    {
        A_GenerateSynchroCurve(ctx, sc, da, val_min, val_max, beh);
        return CKBR_OK;
    }
    //---

    int c_pin = beh->GetInputParameterCount();
    float amplitude = 0;

    CKBOOL all_keys_up = TRUE;

    for (int a = 3; a < c_pin; a++)
    {
        beh->GetInputParameterValue(a, &key);

        if (input->IsKeyDown(key))
        {
            all_keys_up = FALSE;
            if (sc->state)
            {
                sc->state = FALSE;
                sc->delay = tm->GetTime();

                amplitude = val_min + (val_max - val_min) * (a - 2) / (c_pin - 3);

                int i = da->GetRowCount();
                da->InsertRow();

                float time = tm->GetTime() - sc->time;
                da->SetElementValue(i, 0, &time);
                da->SetElementValue(i, 1, &amplitude);
            }
            break;
        }
    }

    if (all_keys_up && !sc->state)
    {
        sc->state = TRUE;
        int i = da->GetRowCount() - 1;
        float delay = tm->GetTime() - sc->delay;
        da->SetElementValue(i, 2, &delay);
    }

    return CKBR_ACTIVATENEXTFRAME;
}
