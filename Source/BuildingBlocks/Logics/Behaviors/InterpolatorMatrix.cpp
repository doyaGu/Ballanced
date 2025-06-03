/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Interpolator Matrix
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorInterpolatorMatrixDecl();
CKERROR CreateInterpolatorMatrixProto(CKBehaviorPrototype **);
int InterpolatorMatrix(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInterpolatorMatrixDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator Matrix");
    od->SetDescription("Performs a linear interpolation between matrices.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first matrix.<BR>
    <SPAN CLASS=pin>B: </SPAN>second matrix.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated matrix.<BR>
    <BR>
    This building block interpolates the orientation information stored inside the two given matrices, as well as the scale.<BR>
    If Value=0% then C=A, if Value=100% then C=B.<BR>
    */
    /* warning:
    - This building block perform a quaternion slerp, and a scaling interpolation.
    Therefore, you might find it a bit slow for a massive use (see "Set Orientation", "Look At", and "Interpolate Orientation" building blocks).<BR>
    - Beware to use a LEFT-HAND oriented basis for your object. Interpolation will not work correctly if the basis of your object if RIGHT-HAND oriented.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3184d55, 0x7d7a0509));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorMatrixProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateInterpolatorMatrixProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator Matrix");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_MATRIX);
    proto->DeclareInParameter("B", CKPGUID_MATRIX);
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE);
    proto->DeclareOutParameter("C", CKPGUID_MATRIX);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(InterpolatorMatrix);

    *pproto = proto;
    return CK_OK;
}
