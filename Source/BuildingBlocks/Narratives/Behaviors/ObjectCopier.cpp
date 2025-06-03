#include "CKAll.h"

CKERROR CreateObjectCopierProto(CKBehaviorPrototype **pproto);
int ObjectCopier(const CKBehaviorContext &context);
CKERROR MatchInOutParamCB(const CKBehaviorContext &behcontext);

extern void CleanUp(CKBehavior *beh);
extern void Load(CKBehavior *beh, CKScene *scn);

CKObjectDeclaration *FillObjectCopierDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Copy");
    od->SetDescription("Duplicates objects");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Dependency Options: </SPAN>options of copy, sorted by object class.<BR>
    <SPAN CLASS=pin>Original: </SPAN>object(s) to copy. You can add inputs to copy more objects in one command.<BR>
    <BR>
    <SPAN CLASS=pin>Copy: </SPAN>copy(s) of the object. If you added inputs, matching outputs will be created.<BR>
    <BR>
    <SPAN CLASS=setting>Dynamic: </SPAN>specifies if the objects duplicated must be dynamics.<BR>
    <BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3f6b0ac7, 0x47d20f78));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateObjectCopierProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObjectCopierProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Copy");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Dependency Options", CKPGUID_COPYDEPENDENCIES);
    proto->DeclareInParameter("Original", CKPGUID_BEOBJECT, NULL);

    proto->DeclareSetting("Dynamic", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Awake Object", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Copy", CKPGUID_BEOBJECT, NULL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS));

    proto->SetFunction(ObjectCopier);
    proto->SetBehaviorCallbackFct(MatchInOutParamCB, CKCB_BEHAVIOREDITED);

    *pproto = proto;
    return CK_OK;
}

CKERROR MatchInOutParamCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        int firstindex = 1;
        CKParameterIn *pin;
        CKParameterOut *pout;
        for (int i = firstindex; i < beh->GetInputParameterCount(); ++i)
        {
            pin = beh->GetInputParameter(i);
            pout = beh->GetOutputParameter(i - firstindex);
            if (pout) // the output exist, we only have to check its type
            {
                if (pout->GetType() != pin->GetType())
                {
                    pout->SetType(pin->GetType());
                    pout->SetName(pin->GetName());
                }
            }
            else // we have to create the output
            {
                beh->CreateOutputParameter(pin->GetName(), pin->GetType());
            }
        }
    }
    break;
    }

    return CK_OK;
}

int ObjectCopier(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    // IOs
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Copy Objects
    int count = beh->GetInputParameterCount();

    // Dynamic ?
    CKBOOL dynamic = TRUE;
    beh->GetLocalParameterValue(0, &dynamic);

    XObjectArray SrcObjects;
    int i;
    for (i = 1; i < count; ++i)
    {
        // Input Reading
        CKObject *src = beh->GetInputParameterObject(i);
        if (!src)
            continue;

        // we do not want to duplicate level
        if (CKIsChildClassOf(src, CKCID_LEVEL))
            continue;

        SrcObjects.PushBack(src->GetID());
    }

    // We get the dependencies options
    CKDependencies *dep = *(CKDependencies **)beh->GetInputParameterReadDataPtr(0);

    CKBOOL needToActivateObject = TRUE;
    beh->GetLocalParameterValue(1, &needToActivateObject);
    if (beh->GetVersion() < 0x00020000)
    {
        needToActivateObject = TRUE;
    }

    CKDWORD options = 0;
    if (needToActivateObject)
        options |= CK_OBJECTCREATION_ACTIVATE;
    if (dynamic)
        options |= CK_OBJECTCREATION_DYNAMIC;

    // The COPY
    const XObjectArray &DstObjects = context.Context->CopyObjects(SrcObjects, dep, (CK_OBJECTCREATION_OPTIONS)options);

    for (i = 0; i < DstObjects.Size(); ++i)
    {
        CKObject *obj = DstObjects.GetObject(context.Context, i);

        beh->SetOutputParameterObject(i, obj);
    }

    return CKBR_OK;
}
