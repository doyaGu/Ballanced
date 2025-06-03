/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetSpecularFlag
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetSpecularFlagDecl();
CKERROR CreateSetSpecularFlagProto(CKBehaviorPrototype **);
int SetSpecularFlag(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetSpecularFlagDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Specular Flag");
    od->SetDescription("Tells whether a light should be specular or not.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Specular Highlight: </SPAN>Specifies if you want the light to produce a specular highlight.<BR>
    <BR>
    To make highlights appear on an object, it should be defined as having the specular property.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1ae11f1a, 0x1a1e1f11));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetSpecularFlagProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetSpecularFlagProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Specular Flag");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Specular Highlight", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetSpecularFlag);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetSpecularFlag(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    CKBOOL k = TRUE;
    beh->GetInputParameterValue(0, &k);

    light->SetSpecularFlag(k);

    return CKBR_OK;
}
