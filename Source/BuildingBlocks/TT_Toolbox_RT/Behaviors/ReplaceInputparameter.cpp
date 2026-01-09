////////////////////////////////////////////
////////////////////////////////////////////
//
//        TT ReplaceInputparameter
//
////////////////////////////////////////////
////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorReplaceInputparameterDecl();
CKERROR CreateReplaceInputparameterProto(CKBehaviorPrototype **pproto);
int ReplaceInputparameter(const CKBehaviorContext &behcontext);

// Helper function to find a parameter by name in a script (searches output params, local params, and recursively in sub-behaviors)
// If setMode is TRUE, it searches input parameters and links them to sourceParam
// Returns the found parameter (when getting) or NULL on error (when setting, NULL means success)
static CKParameter *FindOrLinkParameter(CKContext *ctx, CKBehavior *script, const char *paramName, CKBOOL setMode, CKParameter *sourceParam)
{
    if (!script)
        return NULL;

    if (!setMode)
    {
        // GET mode: Search for output parameter or local parameter with matching name
        
        // Search output parameters
        int outCount = script->GetOutputParameterCount();
        for (int i = 0; i < outCount; i++)
        {
            CKParameterOut *param = script->GetOutputParameter(i);
            if (param && param->GetName())
            {
                if (strcmp(param->GetName(), paramName) == 0)
                    return param;
            }
        }

        // Search local parameters
        int localCount = script->GetLocalParameterCount();
        for (int i = 0; i < localCount; i++)
        {
            CKParameterLocal *param = script->GetLocalParameter(i);
            if (param && param->GetName())
            {
                if (strcmp(param->GetName(), paramName) == 0)
                    return param;
            }
        }

        // Search recursively in sub-behaviors
        int subCount = script->GetSubBehaviorCount();
        for (int i = 0; i < subCount; i++)
        {
            CKBehavior *subBeh = script->GetSubBehavior(i);
            CKParameter *result = FindOrLinkParameter(ctx, subBeh, paramName, FALSE, NULL);
            if (result)
                return result;
        }

        return NULL;
    }
    else
    {
        // SET mode: Search for input parameters whose source has matching name and link to sourceParam
        if (!sourceParam)
            return NULL;

        int inCount = script->GetInputParameterCount();
        for (int i = 0; i < inCount; i++)
        {
            CKParameterIn *param = script->GetInputParameter(i);
            if (param)
            {
                // Skip shared parameters
                if (param->GetSharedSource())
                    continue;

                CKParameter *outSource = param->GetDirectSource();
                if (outSource && outSource->GetName())
                {
                    if (strcmp(outSource->GetName(), paramName) == 0)
                    {
                        // Found matching parameter - check type compatibility
                        if (outSource->IsCompatibleWith(sourceParam) && sourceParam->IsCompatibleWith(outSource))
                        {
                            // Mark old source as replaced
                            char newName[256];
                            sprintf(newName, "~* Replaced by: %s *~", sourceParam->GetName());
                            outSource->SetName(newName);

                            // Link to new source
                            param->SetDirectSource(sourceParam);
                        }
                        else
                        {
                            // Type mismatch
                            char buffer[512];
                            if (sourceParam->GetName() && script->GetName())
                            {
                                sprintf(buffer,
                                    "TT ReplaceInputparameter: %s (%s) have got an other parametertype then %s!\n"
                                    "Couldn't create link!",
                                    outSource->GetName(), script->GetName(), sourceParam->GetName());
                                ctx->OutputToConsole(buffer, TRUE);
                            }
                        }
                    }
                }
            }
        }

        // Search recursively in sub-behaviors
        int subCount = script->GetSubBehaviorCount();
        for (int i = 0; i < subCount; i++)
        {
            CKBehavior *subBeh = script->GetSubBehavior(i);
            CKParameter *result = FindOrLinkParameter(ctx, subBeh, paramName, TRUE, sourceParam);
            if (result)
                return result;
        }

        return NULL;
    }
}

CKObjectDeclaration *FillBehaviorReplaceInputparameterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ReplaceInputparameter");
    od->SetDescription("Links an input parameter to another parameter.");
    od->SetCategory("TT Toolbox/Advanced");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x54a776b7, 0x20fc1e7c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReplaceInputparameterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReplaceInputparameterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ReplaceInputparameter");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Replace!");

    proto->DeclareInParameter("Get-Script", CKPGUID_SCRIPT);
    proto->DeclareInParameter("Get-Parametername", CKPGUID_STRING);
    proto->DeclareInParameter("Set-Script", CKPGUID_SCRIPT);
    proto->DeclareInParameter("Set-Parametername", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReplaceInputparameter);

    *pproto = proto;
    return CK_OK;
}

int ReplaceInputparameter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    if (!beh || !ctx)
        return CKBR_PARAMETERERROR;

    // Get Get-Script (source script containing the parameter to get)
    CKBehavior *getScript = (CKBehavior *)beh->GetInputParameterObject(0);
    
    // Get Set-Script (destination script where we set/link the parameter)
    CKBehavior *setScript = (CKBehavior *)beh->GetInputParameterObject(2);

    if (!getScript)
    {
        ctx->OutputToConsole("TT ReplaceInputparameter: no Get-Script specified!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    if (!setScript)
    {
        ctx->OutputToConsole("TT ReplaceInputparameter: no Set-Script specified!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    // Get parameter names
    const char *getParamName = (const char *)beh->GetInputParameterReadDataPtr(1);
    const char *setParamName = (const char *)beh->GetInputParameterReadDataPtr(3);

    // Check Get-Parametername
    if (!getParamName || strcmp(getParamName, "") == 0)
    {
        ctx->OutputToConsole("TT ReplaceInputparameter: no Get-Parametername specified!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    // Check Set-Parametername  
    if (!setParamName || strcmp(setParamName, "") == 0)
    {
        ctx->OutputToConsole("TT ReplaceInputparameter: no Set-Parametername specified!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    // Find the source parameter in Get-Script
    CKParameter *sourceParam = FindOrLinkParameter(ctx, getScript, getParamName, FALSE, NULL);
    if (!sourceParam)
    {
        ctx->OutputToConsole("TT ReplaceInputparameter: Get-Parameter not found!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    // Link all matching input parameters in Set-Script to the source parameter
    CKParameter *result = FindOrLinkParameter(ctx, setScript, setParamName, TRUE, sourceParam);
    if (result)
    {
        ctx->OutputToConsole("TT ReplaceInputparameter: Set-Parameter not found!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    return CKBR_OK;
}