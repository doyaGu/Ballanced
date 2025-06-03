/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Ambient
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetAlphaTestDecl();
CKERROR CreateSetAlphaTestProto(CKBehaviorPrototype **);
int SetAlphaTest(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetAlphaTestDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Alpha Test");
    od->SetDescription("Setups the Alpha Test of a Material.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Alpha Test Enable: </SPAN>The Alpha Test is enabled or not.
    <SPAN CLASS=pin>Alpha Test Function: </SPAN>select the Alpha Compare function the material should use.
    <SPAN CLASS=pin>Alpha Reference Value: </SPAN>select the Alpha reference value to use.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1cb5661e, 0x5b5d3fda));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetAlphaTestProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetAlphaTestProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Alpha Test");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Alpha Test Enable", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Alpha Test Function", CKPGUID_ZFUNC, "Greater Equal");
    proto->DeclareInParameter("Alpha Reference Value", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetAlphaTest);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetAlphaTest(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    CKBOOL zwrite = TRUE;
    if (beh->GetInputParameterValue(0, &zwrite) == CK_OK)
        mat->EnableAlphaTest(zwrite);

    VXCMPFUNC afunc = VXCMP_GREATEREQUAL;
    if (beh->GetInputParameterValue(1, &afunc) == CK_OK)
        mat->SetAlphaFunc(afunc);

    int ref = 1;
    if (beh->GetInputParameterValue(2, &ref) == CK_OK)
        mat->SetAlphaRef(ref);

    return CKBR_OK;
}
