/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            HierarchyHide
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHierarchyHideDecl();
CKERROR CreateHierarchyHideProto(CKBehaviorPrototype **pproto);
int HierarchyHide(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHierarchyHideDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Hide Hierarchy");
    od->SetDescription("Hides hierarchically an entity");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    This behavior will hide the object and all its children in one operation. This makes the rendering
    faster than hiding each object in the hierarchy separately, like the behavior Hide with the hierarchy
    flag does.<BR>
    <BR>
    See Also: Show.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x27447f6b, 0x22aa0c59));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHierarchyHideProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/Show-Hide");
    return od;
}

CKERROR CreateHierarchyHideProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Hide Hierarchy");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(HierarchyHide);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int HierarchyHide(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *obj = (CKBeObject *)beh->GetTarget();
    if (!obj)
        return CKBR_OWNERERROR;

    obj->Show(CKHIERARCHICALHIDE);

    return CKBR_OK;
}
