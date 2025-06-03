/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set ZBuffer
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetZBufferDecl();
CKERROR CreateSetZBufferProto(CKBehaviorPrototype **);
int SetZBuffer(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetZBufferDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Material Z Buffer");
    od->SetDescription("Sets the Z buffer rendering options of the material.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Z Buffer Write: </SPAN>if TRUE, the material will force ZBuffer writing for all faces using this material.<BR>
    <SPAN CLASS=pin>Z Compare Function: </SPAN>select the Z Compare function the material should use (i.e. sets the Z Reading properties of faces using this material).<BR>
    NEVER = Always fail the test.<BR>
    LESS = Accept if value if less than current value.<BR>
    EQUAL = Accept if value if equal than current value.<BR>
    LESSEQUAL = Accept if value if less or equal than current value.<BR>
    GREATER = Accept if value if greater than current value.<BR>
    NOTEQUAL = Accept if value if different than current value.<BR>
    GREATEREQUAL = Accept if value if greater or equal current value.<BR>
    ALWAYS = Always accept the test.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1144022f, 0x68fd055c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetZBufferProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetZBufferProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Material Z Buffer");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Z Buffer Write", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Z Compare Function", CKPGUID_ZFUNC, "Less Equal");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetZBuffer);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetZBuffer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    CKBOOL zwrite = TRUE;
    beh->GetInputParameterValue(0, &zwrite);

    VXCMPFUNC zfunc = VXCMP_LESSEQUAL;
    beh->GetInputParameterValue(1, &zfunc);

    mat->EnableZWrite(zwrite);
    mat->SetZFunc(zfunc);

    return CKBR_OK;
}
