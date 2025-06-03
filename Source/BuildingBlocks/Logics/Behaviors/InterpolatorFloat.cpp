/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            InterpolatorFloat
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorInterpolatorFloatDecl();
CKERROR CreateInterpolatorFloatProto(CKBehaviorPrototype **);
int InterpolatorFloat(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInterpolatorFloatDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator Float");
    od->SetDescription("Performs a linear interpolation between 2 floats.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first float.<BR>
    <SPAN CLASS=pin>B: </SPAN>second float.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated float.<BR>
    <BR>
    If Value=0% then C=A, if Value=100% then C=B.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17171700, 0x17171700));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorFloatProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateInterpolatorFloatProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator Float");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_FLOAT);
    proto->DeclareInParameter("B", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE, "50");
    proto->DeclareOutParameter("C", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(InterpolatorFloat);

    *pproto = proto;
    return CK_OK;
}
