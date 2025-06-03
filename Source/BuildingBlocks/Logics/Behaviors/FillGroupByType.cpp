/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					FillGroupByType
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFillGroupByTypeDecl();
CKERROR CreateFillGroupByTypeProto(CKBehaviorPrototype **);
int FillGroupByType(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFillGroupByTypeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Fill Group By Class");
    od->SetDescription("Add all objects of a given type to the group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Class: </SPAN>type of objects you want in the group.<BR>
    <SPAN CLASS=pin>Derived Class: </SPAN>if you want the objects of derived type to be added in the group too.<BR>
    eg:
    - lets say Type=3DEntity, and Derived=TRUE, then 3DEntities and 3DObjects will be added to the group.<BR>
    - if Type=3DEntity, and Derived=FALSE, then 3Dentities will be added to the group, but not the 3DObjects.<BR>
    <SPAN CLASS=pin>Only In Current Scene: </SPAN>if TRUE, the building block will only parse the objects of the current scene.
    Otherwise, all the objects of the entire level (with the given type) will be parsed.<BR>
    See also : Add To Group, Remove From Group.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4445257b, 0x70016c57));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFillGroupByTypeProto);
    od->SetCompatibleClassId(CKCID_GROUP);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateFillGroupByTypeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Fill Group By Class");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Class", CKPGUID_CLASSID, "Behavioral Object");
    proto->DeclareInParameter("Derived Classes", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Only In Current Scene", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(FillGroupByType);

    *pproto = proto;
    return CK_OK;
}

int FillGroupByType(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKBeObject *objscene;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKGroup *group = (CKGroup *)beh->GetTarget();
    if (!group)
        return CKBR_PARAMETERERROR;

    int type = 0; // Tyte
    beh->GetInputParameterValue(0, &type);

    CKBOOL derived = FALSE; // Derived
    beh->GetInputParameterValue(1, &derived);

    CKBOOL thisscene = TRUE; // This Scene
    beh->GetInputParameterValue(2, &thisscene);

    CKScene *scene = behcontext.CurrentScene;

    int j, count;

    int start = 0, end = CKCID_MAXCLASSID;
    if (!derived)
    {
        start = type;
        end = type + 1;
    }

    for (int i = start; i <= end; i++)
    {
        if (CKIsChildClassOf((CK_CLASSID)i, type))
        {

            count = ctx->GetObjectsCountByClassID(i);
            CK_ID *ol = ctx->GetObjectsListByClassID(i);
            // we add objects in the group
            if (thisscene)
            {
                for (j = 0; j < count; j++)
                {
                    CKObject *o = ctx->GetObject(ol[j]);
                    if ((!o->IsPrivate()) && scene->IsObjectHere(o))
                    {
                        group->AddObject((CKBeObject *)o);
                    }
                }
            }
            else
            {
                for (j = 0; j < count; j++)
                {
                    objscene = (CKBeObject *)ctx->GetObject(ol[j]);
                    if (!objscene->IsPrivate())
                        group->AddObject(objscene);
                }
            }
        }
    }

    return CKBR_OK;
}
