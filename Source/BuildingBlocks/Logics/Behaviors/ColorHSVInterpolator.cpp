/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		    Behavior Switch Camera Box
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorColorHSVInterpolatorDecl();
CKERROR CreateColorHSVInterpolatorProto(CKBehaviorPrototype **);
int ColorHSVInterpolator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorColorHSVInterpolatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator HSV Color");
    od->SetDescription("Interpolates a color Using HSV color Model");
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
    HSV Interpolation should be better in most case than classic Color Interpolations, because you can for example interpolate colors without
    modifying the Luminosity, or Saturation of your color.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x778400bb, 0x1a480d35));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateColorHSVInterpolatorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateColorHSVInterpolatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator HSV Color");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Start Color", CKPGUID_COLOR, NULL);
    proto->DeclareInParameter("End Color", CKPGUID_COLOR, NULL);
    proto->DeclareInParameter("Mix Amount", CKPGUID_PERCENTAGE, "50");

    proto->DeclareOutParameter("Interpolated Color", CKPGUID_COLOR, NULL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);

    proto->SetFunction(ColorHSVInterpolator);

    *pproto = proto;
    return CK_OK;
}
