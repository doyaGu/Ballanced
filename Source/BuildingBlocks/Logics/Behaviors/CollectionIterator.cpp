/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					Is In Collection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCollectionIteratorDecl();
CKERROR CreateCollectionIteratorProto(CKBehaviorPrototype **);
int CollectionIterator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCollectionIteratorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Collection Iterator");
    od->SetDescription("Retrieves each element of a Collection.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Collection: </SPAN>the Collection to parse.<BR>
    <BR>
    <SPAN CLASS=pout>Element: </SPAN>current parsed element of the Collection.<BR>
    <SPAN CLASS=pout>Index: </SPAN>index of the current element.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x419602ec, 0x7822779e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCollectionIteratorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Loops");
    return od;
}

CKERROR CreateCollectionIteratorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Collection Iterator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Collection", CKPGUID_OBJECTARRAY);

    proto->DeclareOutParameter("Element", CKPGUID_OBJECT);
    proto->DeclareOutParameter("Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CollectionIterator);

    *pproto = proto;
    return CK_OK;
}

int CollectionIterator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    XObjectArray *array = NULL;
    beh->GetInputParameterValue(0, &array);

    if (!array)
    { //--- I keep this here for backcompatibility...
        beh->ActivateOutput(0);
        return CKBR_PARAMETERERROR;
    } //---

    int index = 0;
    if (beh->GetOutputParameterValue(1, &index))
        beh->GetLocalParameterValue(0, &index);

    if (beh->IsInputActive(0))
    { // IN
        beh->ActivateInput(0, FALSE);
        index = 0;
    }
    else
    { // LOOP IN
        beh->ActivateInput(1, FALSE);
        ++index;
    }

    if (index >= array->Size())
    { // no more object in Collection
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    CKBeObject *beo = (CKBeObject *)array->GetObject(behcontext.Context, index);
    beh->SetOutputParameterObject(0, beo);
    if (beh->SetOutputParameterValue(1, &index))
        beh->SetLocalParameterValue(0, &index);

    beh->ActivateOutput(1);
    return CKBR_OK;
}
