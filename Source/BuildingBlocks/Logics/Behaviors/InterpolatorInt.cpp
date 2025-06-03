/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Interpolator Int
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorInterpolatorIntDecl();
CKERROR CreateInterpolatorIntProto(CKBehaviorPrototype **);
int InterpolatorInt(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInterpolatorIntDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Interpolator Int");
    od->SetDescription("Performs an interpolation between 2 integers.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>A: </SPAN>first int.<BR>
    <SPAN CLASS=pin>B: </SPAN>second int.<BR>
    <SPAN CLASS=pin>Value: </SPAN>value to be taken between A and B ( 0% means A, 100% means B ).<BR>
    <BR>
    <SPAN CLASS=pout>C: </SPAN>interpolated int.<BR>
    <BR>
    If Value=0% then C=A, if Value=100% then C=B.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17171701, 0x17171701));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInterpolatorIntProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateInterpolatorIntProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Interpolator Int");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("A", CKPGUID_INT, "0");
    proto->DeclareInParameter("B", CKPGUID_INT, "100");
    proto->DeclareInParameter("Value", CKPGUID_PERCENTAGE, "50");
    proto->DeclareOutParameter("C", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(InterpolatorInt);

    *pproto = proto;
    return CK_OK;
}
