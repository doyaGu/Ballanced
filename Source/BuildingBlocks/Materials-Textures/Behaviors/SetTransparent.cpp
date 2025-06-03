/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Transparent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetTransparentDecl();
CKERROR CreateSetTransparentProto(CKBehaviorPrototype **);
int SetTransparent(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTransparentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Transparent");
    od->SetDescription("Puts the Transparent flag to a Mesh (Allow Transparency to be applied with no ZBuffer problems)");
    /* rem:
    <SPAN CLASS=in>Set: </SPAN>set the transparent flag to TRUE.<BR>
    <SPAN CLASS=in>UnSet: </SPAN>set the transparent flag to FLAG.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    Activating the transparency flag, tells the 3D Render to draw this concerned object last.<BR>
    Therefore, if you specified given an Alpha value throw the 'Set Diffuse' Building block, the object will be rendered as any other object except it will be transparent.<BR>
    Otherwise, you'll have unexpected Z-buffer effects.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2fc37564, 0x299a2c9d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetTransparentProto);
    od->SetCompatibleClassId(CKCID_MESH);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetTransparentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Transparent");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Set");
    proto->DeclareInput("UnSet");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetTransparent);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetTransparent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMesh *mesh = (CKMesh *)beh->GetTarget();
    if (!mesh)
        return CKBR_OWNERERROR;

    if (beh->IsInputActive(0))
    {
        mesh->SetTransparent(TRUE);
        beh->ActivateInput(0, FALSE);
    }
    else
    {
        mesh->SetTransparent(FALSE);
        beh->ActivateInput(1, FALSE);
    }

    beh->ActivateOutput(0);

    return CKBR_OK;
}
