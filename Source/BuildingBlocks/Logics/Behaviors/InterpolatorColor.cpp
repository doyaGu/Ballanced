/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Interpolator Color
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorInterpolatorColorDecl();
CKERROR CreateInterpolatorColorProto(CKBehaviorPrototype **);
int InterpolatorColor(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInterpolatorColorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator Color");
    od->SetDescription("Performs a linear interpolation between 2 colors.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first color.<BR>
    <SPAN CLASS=pin>B: </SPAN>second color.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated color.<BR>
    <BR>
    If Value=0% then C=A, if Value=100% then C=B.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17171703, 0x17171703));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorColorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateInterpolatorColorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator Color");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_COLOR, "0,0,0,1.0");
    proto->DeclareInParameter("B", CKPGUID_COLOR, "0,0,0,1.0");
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE, "50");
    proto->DeclareOutParameter("C", CKPGUID_COLOR, "0,0,0,1.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(InterpolatorColor);

    *pproto = proto;
    return CK_OK;
}
