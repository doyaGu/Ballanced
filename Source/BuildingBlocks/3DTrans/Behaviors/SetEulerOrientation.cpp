/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        Set Euler Orientation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetEulerOrientationDecl();
CKERROR CreateSetEulerOrientationProto(CKBehaviorPrototype **pproto);
int SetEulerOrientation(const CKBehaviorContext &behcontext);
CKERROR SetEulerCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetEulerOrientationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Euler Orientation");
    od->SetDescription("Sets the orientation of a 3D Entity with Euler angles.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Orientation: </SPAN>rotation angle around X,Y and Z axis.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>referential in which the angles are expressed (-NULL- in most cases).<BR>
    <BR>
    <SPAN CLASS=setting>Use Euler Angles: </SPAN>Use Euler Angles parameter type instead of 3 angle parameters.<BR>
    See Also: GetEulerOrientation.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xc4966d8, 0x6c0c6d14));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetEulerOrientationProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetEulerOrientationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Euler Orientation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Orientation", CKPGUID_EULERANGLES);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);

    proto->DeclareSetting("Merge Angles Params", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetEulerOrientation);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(SetEulerCallBack);

    *pproto = proto;
    return CK_OK;
}

int SetEulerOrientation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKParameterIn *pin = beh->GetInputParameter(0);
    int offset = 0;
    VxVector ea;

    CKBOOL useeuler = FALSE;
    beh->GetLocalParameterValue(0, &useeuler);

    if (beh->GetInputParameterCount() == 3)
        useeuler = TRUE;

    if (!useeuler)
    { // Old Version
        offset = 2;
        beh->GetInputParameterValue(0, &ea.x);
        beh->GetInputParameterValue(1, &ea.y);
        beh->GetInputParameterValue(2, &ea.z);
    }
    else
    {
        beh->GetInputParameterValue(0, &ea);
    }

    // Stick children
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(1 + offset, &k);
    k = !k;

    // Referential
    CK3dEntity *ref = NULL;
    ref = (CK3dEntity *)beh->GetInputParameterObject(2 + offset);

    VxVector dir, up, right;

    VxMatrix mat;
    Vx3DMatrixFromEulerAngles(mat, ea.x, ea.y, ea.z);
    dir = (VxVector)mat[2];
    up = (VxVector)mat[1];
    right = (VxVector)mat[0];
    ent->SetOrientation(&dir, &up, &right, ref, k);

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SetEulerCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKParameterManager *pm = behcontext.ParameterManager;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL useeuler = FALSE;
        beh->GetLocalParameterValue(0, &useeuler);

        CKParameterIn *pin = beh->GetInputParameter(0);
        if (!useeuler)
        {
            // Already well set
            if (pin->GetGUID() != CKPGUID_EULERANGLES)
                return CKBR_OK;

            pin->SetGUID(CKPGUID_ANGLE);
            pin->SetName("X");

            // we have to insert 2 more
            beh->InsertInputParameter(1, "Z", pm->ParameterGuidToType(CKPGUID_ANGLE));
            beh->InsertInputParameter(1, "Y", pm->ParameterGuidToType(CKPGUID_ANGLE));
        }
        else
        {
            // Already well set
            if (pin->GetGUID() == CKPGUID_EULERANGLES)
                return CKBR_OK;

            pin->SetGUID(CKPGUID_EULERANGLES);
            pin->SetName("Orientation");

            CKDestroyObject(beh->RemoveInputParameter(1));
            CKDestroyObject(beh->RemoveInputParameter(1));
        }
    }
    break;
    }
    return CKBR_OK;
}