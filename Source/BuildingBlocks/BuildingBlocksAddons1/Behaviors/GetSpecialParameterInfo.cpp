/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Get Special Parameter Info
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetSpecialParameterInfoDecl();
CKERROR CreateGetSpecialParameterInfoProto(CKBehaviorPrototype **);
int GetSpecialParameterInfo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetSpecialParameterInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Special Parameter Info");
    od->SetDescription("Get Special Parameter Info.");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e0a767b, 0x707536a4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetSpecialParameterInfoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives");
    return od;
}

CKERROR CreateGetSpecialParameterInfoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Special Parameter Info");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("File Name", CKPGUID_STRING, "D:/Dev/Nemo AddOns/Max2Nmo/Special Parameter Registration.h");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetSpecialParameterInfo);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CK_BEHAVIORPROTOTYPE_NORMAL);

    *pproto = proto;
    return CK_OK;
}

int GetSpecialParameterInfo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    XString fileName = (char *)beh->GetInputParameterReadDataPtr(0);

    FILE *file = fopen(fileName.Str(), "wt");

    CKParameterManager *pm = ctx->GetParameterManager();

    int enumCount = pm->GetNbEnumDefined();
    int flagCount = pm->GetNbFlagDefined();
    int structCount = pm->GetNbStructDefined();

    int paramTypeCount = pm->GetParameterTypesCount();

    fprintf(file, "inline void RegisterSpecialParameters( CKParameterManager *pm ){\n");
    fprintf(file, "XString str;\n");

    for (int a = 0; a < paramTypeCount; ++a)
    {

        CKEnumStruct *enumStruct = pm->GetEnumDescByType(a);

        if (enumStruct)
        { //--- Parameter is an Enum

            CKGUID guid = pm->ParameterTypeToGuid(a);

            int valueCount = enumStruct->GetNumEnums();
            if (valueCount <= 16)
            {

                fprintf(file, "pm->RegisterNewEnum( CKDEFINEGUID(0x%08x,0x%08x), \"%s\", \"", guid.d1, guid.d2, pm->ParameterTypeToName(a));
                for (int b = 0; b < valueCount; ++b)
                {
                    fprintf(file, "%s=%d", enumStruct->GetEnumDescription(b), enumStruct->GetEnumValue(b));
                    if (b != valueCount - 1)
                    {
                        fprintf(file, ",");
                    }
                }
                fprintf(file, "\" );\n");
            }
            else
            { // maybe too big strings
                fprintf(file, "str = \"\";\n");
                int b, c = 0;
                do
                {
                    fprintf(file, "str += \"");
                    for (b = 0; b < 16; ++b)
                    {
                        fprintf(file, "%s=%d", enumStruct->GetEnumDescription(b + c), enumStruct->GetEnumValue(b + c));
                        if (b + c + 1 == valueCount)
                            break;
                        if (b != 15)
                        {
                            fprintf(file, ",");
                        }
                    }
                    c += 16;
                    fprintf(file, "\";\n");
                } while (b == 16);
                fprintf(file, "pm->RegisterNewEnum( CKDEFINEGUID(0x%08x,0x%08x), \"%s\", str );\n", guid.d1, guid.d2, pm->ParameterTypeToName(a));
            }
        }
        else
        {
            CKFlagsStruct *flagStruct = pm->GetFlagsDescByType(a);

            if (flagStruct)
            { //--- Parameter is an Flags

                CKGUID guid = pm->ParameterTypeToGuid(a);

                fprintf(file, "pm->RegisterNewFlags( CKDEFINEGUID(0x%08x,0x%08x), \"%s\", \"", guid.d1, guid.d2, pm->ParameterTypeToName(a));
                int valueCount = flagStruct->GetNumFlags();
                for (int b = 0; b < valueCount; ++b)
                {
                    fprintf(file, "%s=%d", flagStruct->GetFlagDescription(b), flagStruct->GetFlagValue(b));
                    if (b != valueCount - 1)
                    {
                        fprintf(file, ",");
                    }
                }
                fprintf(file, "\" );\n");
            }
            else
            {
                CKStructStruct *structStruct = pm->GetStructDescByType(a);

                if (structStruct)
                { //--- Parameter is an Flags

                    CKGUID guid = pm->ParameterTypeToGuid(a);

#ifdef __GNUC__
                    CKGUID ck_o = CKPGUID_OBSTACLEPRECISION;
                    CKGUID ck_b = CKPGUID_BOOL;
                    pm->RegisterNewStructure(CKPGUID_OBSTACLE, "Obstacle", "Obstacle Type,Use Hierarchy?", &ck_o, &ck_b);
#else
                    pm->RegisterNewStructure(CKPGUID_OBSTACLE, "Obstacle", "Obstacle Type,Use Hierarchy?", CKPGUID_OBSTACLEPRECISION, CKPGUID_BOOL);
#endif
                    fprintf(file, "pm->RegisterNewStructure( CKDEFINEGUID(0x%08x,0x%08x), \"%s\", \"", guid.d1, guid.d2, pm->ParameterTypeToName(a));
                    int valueCount = structStruct->GetNumSubParam();
                    for (int b = 0; b < valueCount; ++b)
                    {
                        fprintf(file, "%s", structStruct->GetSubParamDescription(b));
                        if (b != valueCount - 1)
                        {
                            fprintf(file, ",");
                        }
                    }
                    fprintf(file, "\", ");

                    int subParamCount = structStruct->GetNumSubParam();
                    for (int c = 0; c < valueCount; ++c)
                    {
                        CKGUID subGuid = structStruct->GetSubParamGuid(c);
                        fprintf(file, "CKDEFINEGUID(0x%08x,0x%08x)", subGuid.d1, subGuid.d2);
                        if (c != subParamCount - 1)
                        {
                            fprintf(file, ",");
                        }
                    }
                    fprintf(file, " );\n");
                }
            }
        }
    }

    fprintf(file, "}\n");

    fclose(file);

    return CKBR_OK;
}
