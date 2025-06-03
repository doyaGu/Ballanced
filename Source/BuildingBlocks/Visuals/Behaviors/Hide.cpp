/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Hide
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHideDecl();
CKERROR CreateHideProto(CKBehaviorPrototype **pproto);
int Hide(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHideDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Hide");
    od->SetDescription("Hides a 2D/3D Entity, a Mesh or a Group");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    See Also: Show 3D Entity, Hide Hierarchy.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x31d97d82, 0x78d54d98));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHideProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/Show-Hide");
    return od;
}

CKERROR CreateHideProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Hide");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Hide);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

void ShowHierarchy(CKBeObject *beo, CK_OBJECT_SHOWOPTION b)
{
    // Hide / Show
    beo->Show(b);

    // Recursion
    if (CKIsChildClassOf(beo, CKCID_3DENTITY))
    { // 3D recursion
        CK3dEntity *ent = (CK3dEntity *)beo;

        int i;
        for (i = 0; i < ent->GetChildrenCount(); i++)
        {
            CK3dEntity *child = ent->GetChild(i);
            ShowHierarchy(child, b);
        }
    }
    else
    {
        if (CKIsChildClassOf(beo, CKCID_2DENTITY))
        { // 2D recursion
            CK2dEntity *ent = (CK2dEntity *)beo;

            int i;
            for (i = 0; i < ent->GetChildrenCount(); i++)
            {
                CK2dEntity *child = ent->GetChild(i);
                ShowHierarchy(child, b);
            }
        }
    }
}

int Hide(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    beh->ActivateOutput(0);

    // Stick children ?
    CKBOOL k = FALSE;
    beh->GetInputParameterValue(0, &k);

    CKBeObject *beo = beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    if (k)
    {
        ShowHierarchy(beo, CKHIDE);
    }
    else
    {
        beo->Show(CKHIDE);
    }

    return CKBR_OK;
}
