/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Load
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayLoadDecl();
CKERROR CreateArrayLoadProto(CKBehaviorPrototype **);
int ArrayLoad(const CKBehaviorContext &behcontext);
CKERROR ArrayLoadCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayLoadDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Array Load");
    od->SetDescription("Loads elements into an array from a formatted file.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated if the file is loaded succesfully.<BR>
    <SPAN CLASS=out>File Error: </SPAN>is activated if the file couldn't be loaded correctly.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>name of the file containing the array to load. Must be in given as an absolute path or relative to the .cmo or the .exe.<BR>
    <SPAN CLASS=pin>Append: </SPAN>if TRUE, appends the file contents to your existing array.<BR>
    <SPAN CLASS=pin>First Column: </SPAN>indicates the column in your array that should receive the first column of your file
    letting you load information without destroying the first columns.<BR>
    <BR>
    This behavior, when attached to an array or when given an array as a target,<BR>
    read the input of the given file, formatted in this style :
    <pre>
    value&lt;tab&lt;value&lt;tab&lt;value&crarr;
    value&lt;tab&lt;value&lt;tab&lt;value&crarr;
    value&lt;tab&lt;value&lt;tab&lt;value&crarr;
    etc...
    </pre>
    Native supported types are INTEGER, FLOATS, STRINGS, OBJECTS.<BR>
    for all other types, a PARAMETER will be created, with the type of the analysed parameter.<BR>
    eg: if the string "0,1,0" is found in a Parameter Column, then the vector (0,1,0) will be added to the array as a Parameter containing vectors.<BR>
    Objects are referenced by their names. The name is read from the file and then we search in the current level an object with the same name, whatever its class can be:
    therefore, use unique names for objects you want to be able to load and save.<BR>
    The floats must	be in the u.s. format (e.g: 15.234).<BR>
    The destination array must be preformatted. That means, if your file contains a Vector column and a Float Column, then the
    destination array (the owner of the building block theorically), should be compound of a Vector column and a Float Column.<BR>
    You can append the file contents to your existing array or decide
    to replace all the existing values by the new ones...
    <BR>
    */
    /* warning:
    - From the "Frist Column", all the Column of the destination array will be replaced.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x13bd2c64, 0x62db38e1));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayLoadProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayLoadProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Array Load");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("File Error");

    proto->DeclareInParameter("File", CKPGUID_STRING);
    proto->DeclareInParameter("Append", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("First Column", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayLoad);

    *pproto = proto;
    return CK_OK;
}

int ArrayLoad(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    XString filename(string);
    CKBOOL fileError = FALSE;
    if (CK_OK != behcontext.Context->GetPathManager()->ResolveFileName(filename, DATA_PATH_IDX, -1))
        fileError = TRUE;

    CKBOOL append = TRUE;
    beh->GetInputParameterValue(1, &append);

    int column = 0;
    beh->GetInputParameterValue(2, &column);

    // the loading itself
    if (!fileError && array->LoadElements(filename.Str(), append, column)) // everything was fine
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }
    else // file error
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}
