/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            InterpolatorVector
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorInterpolatorVectorDecl();
CKERROR CreateInterpolatorVectorProto(CKBehaviorPrototype **);
int InterpolatorVector(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInterpolatorVectorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator Vector");
    od->SetDescription("Performs a linear interpolation between 2 vectors.");
    /* rem:
      <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first vector.<BR>
    <SPAN CLASS=pin>B: </SPAN>second vector.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated vector.<BR>
    <BR>
    The interpolation is performed on each coordinate.<BR>
    If Value=0% then C=A, if Value=100% then C=B.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17171702, 0x17171702));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorVectorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateInterpolatorVectorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator Vector");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_VECTOR);
    proto->DeclareInParameter("B", CKPGUID_VECTOR);
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE);
    proto->DeclareOutParameter("C", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(InterpolatorVector);

    *pproto = proto;
    return CK_OK;
}
