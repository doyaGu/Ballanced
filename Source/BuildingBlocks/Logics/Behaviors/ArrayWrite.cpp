/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Write
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayWriteDecl();
CKERROR CreateArrayWriteProto(CKBehaviorPrototype **);
int ArrayWrite(const CKBehaviorContext &behcontext);
CKERROR ArrayWriteCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayWriteDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Array Save");
    od->SetDescription("Writes elements from an array to a text file.");
    /* rem:
    <SPAN CLASS=pin>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated if the file is writen successfully.<BR>
    <SPAN CLASS=out>Write Error: </SPAN>is activated if the file can't be written correctly.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>name of the file containing the array to save. Must be in given as an absolute path.<BR>
    <SPAN CLASS=pin>First Column: </SPAN>indicates the column in your array where the saving process should start.<BR>
    <SPAN CLASS=pin>Number of Column : </SPAN>Number of columns (from the "First Column") you want to write in the file.<BR>
    <BR>
    This building block, when attached to an array or when given an array as a target,
    writes the array to the given file, formatted in this style :
    <pre>
    value<tabulation>value<tabulation>value<carriage return>
    value<tabulation>value<tabulation>value<carriage return>
    value<tabulation>value<tabulation>value<carriage return>
    etc...
    </pre>
    See "Array Load" to know how parameters, objects and native values will be written.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x15185846, 0x41014a45));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayWriteProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayWriteProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Array Save");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Write Error");

    proto->DeclareInParameter("File", CKPGUID_STRING);
    proto->DeclareInParameter("First Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Number of Column", CKPGUID_INT, "1");
    proto->DeclareInParameter("Append", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayWrite);

    *pproto = proto;
    return CK_OK;
}

int ArrayWrite(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // We find the filename
    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    XString filename(string);
    if (!behcontext.Context->GetPathManager()->PathIsAbsolute(filename))
    { // if the path is not absolute, we complete it with the cmo path
        CKPathSplitter ps(behcontext.Context->GetLastCmoLoaded());
        CKPathMaker pm(ps.GetDrive(), ps.GetDir(), string, NULL);
        filename = pm.GetFileName();
    }

    int column = 0;
    beh->GetInputParameterValue(1, &column);

    int number = 1;
    beh->GetInputParameterValue(2, &number);

    CKBOOL append = FALSE;
    beh->GetInputParameterValue(3, &append);

    // the Writeing itself
    if (array->WriteElements(string, column, number, append))
    { // everything was fine
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }
    else
    { // file error
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}
