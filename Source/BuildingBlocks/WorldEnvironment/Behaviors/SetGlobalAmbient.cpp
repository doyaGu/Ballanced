/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetGlobalAmbient
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetGlobalAmbientDecl();
CKERROR CreateSetGlobalAmbientProto(CKBehaviorPrototype **);
int SetGlobalAmbient(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGlobalAmbientDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Ambient Light Color");
    od->SetDescription("Sets the ambient light color.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Color: </SPAN>ambient light color in RGBA.<BR>
    <BR>
    This behavior can be applied to any object in a scene.
    */
    od->SetCategory("World Environments/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x136b3f22, 0xdc12786));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGlobalAmbientProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGlobalAmbientProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Ambient Light Color");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Color", CKPGUID_COLOR, "15,15,15,255");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGlobalAmbient);

    *pproto = proto;
    return CK_OK;
}

int SetGlobalAmbient(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get Background color
    VxColor col;
    beh->GetInputParameterValue(0, &col);

    // Get material
    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    rcontext->SetAmbientLight(col.r, col.g, col.b);

    return CKBR_OK;
}
