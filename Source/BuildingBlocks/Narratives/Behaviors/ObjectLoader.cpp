#include "CKAll.h"

CKERROR CreateObjectLoaderProto(CKBehaviorPrototype **pproto);
int ObjectLoader(const CKBehaviorContext &context);
CKERROR ObjectLoaderCB(const CKBehaviorContext &behcontext);

extern void CleanUp(CKBehavior *beh);
extern void Load(CKBehavior *beh, CKScene *scn);

CKObjectDeclaration *FillObjectLoaderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Load");
    od->SetDescription("Loads a local file, containing one or more objects");
    /* rem:
    <SPAN CLASS=in>Load: </SPAN>loads the content of the given local filename.<BR>
    <SPAN CLASS=in>Unload: </SPAN>unload what the behavior previously loaded.<BR>
    <SPAN CLASS=out>Loaded: </SPAN>is activated when the behavior was triggered by the input Load.<BR>
    <SPAN CLASS=out>Unloaded: </SPAN>is activated when the behavior was triggered by the input Unload.<BR>
    <SPAN CLASS=out>Failed: </SPAN>is activated when the loading has failed.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>.<BR>
    <SPAN CLASS=pin>Master Object Name: </SPAN>name that should be seek into the loaded objects.<BR>
    <SPAN CLASS=pin>Filter Class: </SPAN>specific class the master object should derive from.<BR>
    <BR>
    <SPAN CLASS=pout>Loaded Objects: </SPAN>Object Array containing all the objects loaded.<BR>
    <SPAN CLASS=pout>Master Object: </SPAN>Master object loaded, found with the <SPAN CLASS=pin>Master Name</SPAN> and the <SPAN CLASS=pin>Filter Class</SPAN> inputs, or only
    with the <SPAN CLASS=pin>Filter Class</SPAN> input if no name was provided. If a 3D or 2D entity is desired, a root object is taken when no name are provided.<BR>
    <BR>
    <SPAN CLASS=setting>Dynamic: </SPAN>specifies if the objects loaded must be dynamics.<BR>
    <BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7bd977d7, 0x26396c0c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectLoaderProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateObjectLoaderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Load");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Load");
    proto->DeclareInput("Unload");

    proto->DeclareOutput("Loaded");
    proto->DeclareOutput("Unloaded");
    proto->DeclareOutput("Failed");

    proto->DeclareInParameter("File", CKPGUID_STRING);
    proto->DeclareInParameter("Master Object Name", CKPGUID_STRING);
    proto->DeclareInParameter("Filter Class", CKPGUID_CLASSID, "3D Object");
    proto->DeclareInParameter("Add To Current Scene", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Reuse Meshes", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Reuse Materials/Textures", CKPGUID_BOOL, "TRUE");

    proto->DeclareSetting("Dynamic", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Loaded Objects", CKPGUID_OBJECTARRAY);
    proto->DeclareOutParameter("Master Object", CKPGUID_OBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetFunction(ObjectLoader);
    proto->SetBehaviorCallbackFct(ObjectLoaderCB);

    *pproto = proto;
    return CK_OK;
}

CKERROR ObjectLoaderCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESET:
    {
        XObjectArray *oarray = *(XObjectArray **)beh->GetOutputParameterWriteDataPtr(0);

        /* Should but, what for the not dynamic???
        BOOL dynamic = TRUE;
        beh->GetLocalParameterValue(0,&dynamic);
        if (dynamic)
        */

        oarray->Clear();
        beh->GetOutputParameter(0)->DataChanged();
    }
    break;

    // Delete Every Loaded Object on a delete or detach

    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        //	Load(beh,CKGetCurrentScene());
    }
    break;

    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        //	CleanUp(beh);
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
    }
    break;

    } // Switch

    return CK_OK;
}

int ObjectLoader(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    // Loading
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);

        // Dynamic ?
        CKBOOL dynamic = TRUE;
        beh->GetLocalParameterValue(0, &dynamic);

        CKBOOL addtoscene = TRUE;
        beh->GetInputParameterValue(3, &addtoscene);

        CKScene *scene = ctx->GetCurrentScene();
        if (ctx->GetCurrentLevel()->GetLevelScene() == scene)
            addtoscene = FALSE;

        CKSTRING fname = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
        CKSTRING mastername = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
        CK_CLASSID cid = CKCID_3DOBJECT;
        beh->GetInputParameterValue(2, &cid);

        CK_LOAD_FLAGS loadoptions = CK_LOAD_FLAGS(CK_LOAD_DEFAULT | CK_LOAD_AUTOMATICMODE);
        if (dynamic)
            loadoptions = (CK_LOAD_FLAGS)(loadoptions | CK_LOAD_AS_DYNAMIC_OBJECT);

        CKObjectArray *array = CreateCKObjectArray();

        CKBOOL reuseMeshes = FALSE;
        beh->GetInputParameterValue(4, &reuseMeshes);

        CKBOOL reuseMaterials = FALSE;
        beh->GetInputParameterValue(5, &reuseMaterials);

        ctx->SetAutomaticLoadMode(CKLOAD_OK, CKLOAD_OK, reuseMeshes ? CKLOAD_USECURRENT : CKLOAD_OK, reuseMaterials ? CKLOAD_USECURRENT : CKLOAD_OK);

        // We load the file
        XString filename(fname);
        behcontext.Context->GetPathManager()->ResolveFileName(filename, DATA_PATH_IDX, -1);
        if (ctx->Load(filename.Str(), array, loadoptions) != CK_OK)
        {
            beh->ActivateOutput(2);
            return CKBR_OK;
        }
        else
        {
            beh->ActivateOutput(0);
        }

        XObjectArray *oarray = *(XObjectArray **)beh->GetOutputParameterWriteDataPtr(0);
        oarray->Clear();

        CKLevel *level = behcontext.CurrentLevel;
        CKObject *masterobject = NULL;

        CKLevel *loadedLevel = NULL;

        // If there is a level in the Loaded Object
        for (array->Reset(); !array->EndOfList(); array->Next())
        {
            CKObject *o = array->GetData(ctx);
            if (CKIsChildClassOf(o, CKCID_LEVEL))
            {
                loadedLevel = (CKLevel *)o;
            }

            // We search here for the master object
            if (CKIsChildClassOf(o, cid))
            {
                if (mastername && *mastername != 0)
                {
                    char *objectName = o->GetName();
                    if (objectName && !strcmp(objectName, mastername))
                    {
                        masterobject = o;
                    }
                }
                else
                {
                    if (CKIsChildClassOf(o, CKCID_3DENTITY))
                    {
                        if (!((CK3dEntity *)o)->GetParent())
                            masterobject = o;
                    }
                    else if (CKIsChildClassOf(o, CKCID_2DENTITY))
                    {
                        if (!((CK2dEntity *)o)->GetParent())
                            masterobject = o;
                    }
                }
            }

            oarray->PushBack(o->GetID());
        }

        if (loadedLevel) // If a level is loaded, do a merge
        {

            level->Merge(loadedLevel, FALSE);
            oarray->RemoveObject(loadedLevel);
            behcontext.Context->DestroyObject(loadedLevel);
        }
        else // else add everything to the level / scene
        {

            level->BeginAddSequence(TRUE);

            for (array->Reset(); !array->EndOfList(); array->Next())
            {
                CKObject *o = array->GetData(ctx);
                if (CKIsChildClassOf(o, CKCID_SCENE))
                {
                    level->AddScene((CKScene *)o);
                }
                else
                {
                    level->AddObject(o);
                }

                if (addtoscene && CKIsChildClassOf(o, CKCID_SCENEOBJECT) && !(CKIsChildClassOf(o, CKCID_LEVEL) || CKIsChildClassOf(o, CKCID_SCENE)))
                    scene->AddObjectToScene((CKSceneObject *)o);
            }

            level->BeginAddSequence(FALSE);
        }

        DeleteCKObjectArray(array);
        int ocount = oarray->Size();
        beh->SetOutputParameterObject(1, masterobject);
    }

    // Unloading
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        XObjectArray *oarray = *(XObjectArray **)beh->GetOutputParameterWriteDataPtr(0);
        ctx->DestroyObjects(oarray->Begin(), oarray->Size());
        oarray->Clear();
    }

    beh->GetOutputParameter(0)->DataChanged();
    return CKBR_OK;
}
