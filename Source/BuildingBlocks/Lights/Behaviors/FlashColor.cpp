/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//				FlashColor
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFlashColorDecl();
CKERROR CreateFlashColorProto(CKBehaviorPrototype **);
int FlashColor(const CKBehaviorContext &behcontext);
int FlashColorTimeBased(const CKBehaviorContext &behcontext);
int FlashColorTimeBasedOld(const CKBehaviorContext &behcontext);
CKERROR FlashColorCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFlashColorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Light Color Progression");
    od->SetDescription("Makes a Light progressively change from one color to another.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in a behavior's process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the behavior's process needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression of the 3D Entity along its movement.<BR>
    <SPAN CLASS=pin>Start Color: </SPAN>initial color in RGBA.<BR>
    <SPAN CLASS=pin>End Color: </SPAN>final color in RGBA.<BR>
    <BR>
    <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame Rate dependant. Making this building block Time dependant has the advantage that compositions will run at the same rate on all computer configurations.<BR>
    <BR>
    */
    /* warning:
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is that the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x20171e01, 0x234a1b62));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateFlashColorProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/FX");
    return od;
}

CKERROR CreateFlashColorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Light Color Progression");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");
    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Duration", CKPGUID_TIME, "0m 0s 100ms");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Start Color", CKPGUID_COLOR, "0,0,0,255");
    proto->DeclareInParameter("End Color", CKPGUID_COLOR, "255,255,255,255");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); //"Previous"
    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(FlashColorTimeBased);
    proto->SetBehaviorCallbackFct(FlashColorCallBack);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*          Flash Color Not Time Based                 */
/*******************************************************/
int FlashColor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_PARAMETERERROR;

    int elapsed = 0;

    if (beh->IsInputActive(0))
    { // On rentre dans le comportement par l'entre initiale
        beh->ActivateInput(0, FALSE);
        // On initialise les variables locales.
        beh->SetLocalParameterValue(0, &elapsed);
    }
    else
    { // On rentre dans le comportement par la loop
        beh->GetLocalParameterValue(0, &elapsed);
        beh->ActivateInput(1, FALSE);
    }

    int frames;
    beh->GetInputParameterValue(0, &frames);

    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);

    VxColor color;
    beh->GetInputParameterValue(2, &color);

    float value = curve->GetY((float)elapsed / frames);
    color.r *= value;
    color.g *= value;
    color.b *= value;

    light->SetColor(color);

    elapsed++;
    if (elapsed >= frames)
    {
        beh->ActivateOutput(0);
    }
    else
    {
        beh->SetLocalParameterValue(0, &elapsed);
        beh->ActivateOutput(1);
    }
    return CKBR_OK;
}

/*******************************************************/
/*       Flash Color (TIME Based new version (float))  */
/*******************************************************/
int FlashColorTimeBased(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_PARAMETERERROR;

    float elapsed = 0;

    if (beh->IsInputActive(0))
    { // On rentre dans le comportement par l'entre initiale
        beh->ActivateInput(0, FALSE);
        // On initialise les variables locales.
        beh->SetLocalParameterValue(0, &elapsed);
    }
    else
    { // On rentre dans le comportement par la loop
        beh->GetLocalParameterValue(0, &elapsed);
        beh->ActivateInput(1, FALSE);
    }

    float frames;
    beh->GetInputParameterValue(0, &frames);

    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);

    VxColor A, B;
    beh->GetInputParameterValue(2, &A);
    beh->GetInputParameterValue(3, &B);

    float value = curve->GetY(elapsed / frames);
    A.r += (B.r - A.r) * value;
    A.g += (B.g - A.g) * value;
    A.b += (B.b - A.b) * value;

    light->SetColor(A);

    elapsed += behcontext.DeltaTime;
    if (elapsed >= frames)
    {
        beh->ActivateOutput(0);
    }
    else
    {
        beh->SetLocalParameterValue(0, &elapsed);
        beh->ActivateOutput(1);
    }
    return CKBR_OK;
}

/*******************************************************/
/*       Flash Color (TIME Based old version (int))    */
/*******************************************************/
int FlashColorTimeBasedOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_PARAMETERERROR;

    float elapsed = 0;

    if (beh->IsInputActive(0))
    { // On rentre dans le comportement par l'entre initiale
        beh->ActivateInput(0, FALSE);
        // On initialise les variables locales.
        beh->SetLocalParameterValue(0, &elapsed);
    }
    else
    { // On rentre dans le comportement par la loop
        beh->GetLocalParameterValue(0, &elapsed);
        beh->ActivateInput(1, FALSE);
    }

    int frames;
    beh->GetInputParameterValue(0, &frames);

    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);

    VxColor A, B;
    beh->GetInputParameterValue(2, &A);
    beh->GetInputParameterValue(3, &B);

    float value = curve->GetY(elapsed / frames);
    A.r += (B.r - A.r) * value;
    A.g += (B.g - A.g) * value;
    A.b += (B.b - A.b) * value;

    light->SetColor(A);

    elapsed += (int)behcontext.DeltaTime;
    if (elapsed >= frames)
    {
        beh->ActivateOutput(0);
    }
    else
    {
        beh->SetLocalParameterValue(0, &elapsed);
        beh->ActivateOutput(1);
    }
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR FlashColorCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL time = FALSE;
        beh->GetLocalParameterValue(1, &time);

        CKParameterIn *pin = beh->GetInputParameter(0);

        CKGUID currentGuid = pin->GetGUID();
        CKBOOL isOldVersion = (beh->GetVersion() < 0x00010005);

        // Time based or not ?
        if (time)
        {
            if (isOldVersion && (currentGuid != CKPGUID_TIME))
            {
                beh->SetFunction(FlashColorTimeBasedOld);
                pin->SetGUID(CKPGUID_INT, TRUE, "Duration (ms)");
            }
            else
            {
                beh->SetFunction(FlashColorTimeBased);
                pin->SetGUID(CKPGUID_TIME, TRUE, "Duration");
                if (isOldVersion)
                    beh->SetVersion(0x00010005);
            }
        }
        else
        {
            beh->SetFunction(FlashColor);
            pin->SetGUID(CKPGUID_INT, TRUE, "Steps");
        }
    }
    }

    return CKBR_OK;
}
