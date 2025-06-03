/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            LoadString
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLoadStringDecl();
CKERROR CreateLoadStringProto(CKBehaviorPrototype **);
int LoadString(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLoadStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Load String");
    od->SetDescription("Loads a string from a file.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>name of the file containing the string to load. Must be given as an absolute path or relative to the .cmo or the .exe.<BR>
    <BR>
    <SPAN CLASS=pout>String: </SPAN>the loaded string.<BR>
    */
    od->SetCategory("Logics/Strings");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x391555d6, 0x42f2500e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLoadStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLoadStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Load String");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");
    proto->DeclareOutput("File Error");

    proto->DeclareInParameter("File", CKPGUID_STRING);

    proto->DeclareOutParameter("String", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LoadString);

    *pproto = proto;
    return CK_OK;
}

int LoadString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    ///
    // Input / Outputs
    beh->ActivateInput(0, FALSE);

    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    XString filename(string);
    behcontext.Context->GetPathManager()->ResolveFileName(filename, DATA_PATH_IDX, -1);

    FILE *file = fopen(filename.Str(), "r");
    if (file)
    {
        beh->ActivateOutput(0);

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        CKSTRING text = new char[size + 1];
        // we read from the file
        int realcount = 0;
        if (size)
            realcount = fread(text, 1, size, file);

        // eof
        text[realcount] = '\0';

        beh->SetOutputParameterValue(0, text, realcount + 1);
        fclose(file);
        delete[] text;
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}