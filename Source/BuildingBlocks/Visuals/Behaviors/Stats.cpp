/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Stats
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define FRAME_RATE          0
#define INTERFACETIME       1
#define PROCESSING          2
#define RENDERING           3
#define PO                  4
#define ANIMATION           5
#define IK                  6
#define TOTAL_BEHAVIOR      7
#define BEHAVIOR            8
#define ACTIVE_OBJECT       9
#define BEHAVIOR_EXECUTED   10
#define BB_EXECUTED         11
#define LINK_PARSED         12
#define LINK_DELAYED        13
#define FACE_DRAWN          14
#define LINES_DRAWN         15
#define POINTS_DRAWN        16
#define OBJECTS_DRAWN       17
#define VERTICES_PROCESSED  18

// General Statistics Flags
#define GF_INTERFACE        0x00000001
#define GF_PROCESSING       0x00000002
#define GF_RENDERING        0x00000004
#define GF_PO               0x00000008
#define GF_ANIMATION        0x00000010
#define GF_IK               0x00000020
#define GF_TOTAL_BEHAVIOR   0x00000040
#define GF_BEHAVIOR         0x00000080

// Behavior Statistics Flags
#define BF_ACTIVE_OBJECT        0x00000001
#define BF_BEHAVIOR_EXECUTED    0x00000002
#define BF_BB_EXECUTED          0x00000004
#define BF_LINK_PARSED          0x00000008
#define BF_LINK_DELAYED         0x00000010

// Render Statistics Flags
#define RF_FACE_DRAWN           0x00000001
#define RF_LINES_DRAWN          0x00000002
#define RF_POINTS_DRAWN         0x00000004
#define RF_OBJECTS_DRAWN        0x00000008
#define RF_VERTICES_PROCESSED   0x00000010

// The GUID we need to define
#define CKPGUID_GENERALSTAT     CKDEFINEGUID(0xfe82e7c, 0x98173f)
#define CKPGUID_BEHAVIORSTAT    CKDEFINEGUID(0x3b542289, 0x2f627abb)
#define CKPGUID_RENDERSTAT      CKDEFINEGUID(0x54a578a6, 0x53f144d4)

CKObjectDeclaration *FillBehaviorStatsDecl();
CKERROR CreateStatsProto(CKBehaviorPrototype **pproto);
int Stats(const CKBehaviorContext &behcontext);
CKERROR StatsCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorStatsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Statistics");
    od->SetDescription("Returns the needed statistics about the system.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pout>Frame Rate: </SPAN>number of frame per second.<BR>
    <SPAN CLASS=pout>Interface Time: </SPAN>estimated time for windows and interface.<BR>
    <SPAN CLASS=pout>Processing Time: </SPAN>sstimated time for processing since last frame.<BR>
    <SPAN CLASS=pout>Rendering Time: </SPAN>sstimated time for rendering since last frame.<BR>
    <SPAN CLASS=pout>Parameter Operations Time: </SPAN>time taken by parametric operations since last frame.<BR>
    <SPAN CLASS=pout>Animation Management Time: </SPAN>during Behavior Execution time - Time taken in character animations processing.<BR>
    <SPAN CLASS=pout>IK Management Time: </SPAN>during Behavior Execution time - Time taken in IK solving.<BR>
    <SPAN CLASS=pout>Total Behavior Time: </SPAN>total behavior Execution time.<BR>
    <SPAN CLASS=pout>Behavior Execution Time: </SPAN>during Behavior Execution time - Time taken in by code execution of behaviors.<BR>
    <SPAN CLASS=pout>Active Object Executed: </SPAN>number of objects executed since last frame.<BR>
    <SPAN CLASS=pout>Behaviors Executed: </SPAN>number of behaviors executed since last frame.<BR>
    <SPAN CLASS=pout>Building Blocks Executed: </SPAN>number of building blocks executed since last frame.<BR>
    <SPAN CLASS=pout>Links Parsed: </SPAN>number of behavior links parsed since last frame.<BR>
    <SPAN CLASS=pout>Links Delayed: </SPAN>number of Behavior links that have been stored as active in N frames.<BR>
    <SPAN CLASS=pout>Faces Drawn: </SPAN>number of faces drawn in the current frame.<BR>
    <SPAN CLASS=pout>Lines Drawn: </SPAN>number of lines drawn in the current frame.<BR>
    <SPAN CLASS=pout>Points Drawn: </SPAN>number of points drawn in the current frame.<BR>
    <SPAN CLASS=pout>Objects Drawn: </SPAN>number of objects drawn in the current frame.<BR>
    <SPAN CLASS=pout>Vertices Processed: </SPAN>number of vertices transformed for the current frame.<BR>
    <BR>
    This behavior gives access to several statistical informations as the frame rate, the CPU time...
    It can be used with the "Text Display" behavior to display these statistics.
    <BR>
    <SPAN CLASS=setting>Frame Rate: </SPAN>should the building block compute the frame rate ?<BR>
    <SPAN CLASS=setting>General Time: </SPAN>which time statistics should the behavior compute ?<BR>
    <SPAN CLASS=setting>Behavioral: </SPAN>which behavioral statistics should the behavior compute ?<BR>
    <SPAN CLASS=setting>Rendering: </SPAN>which renderal statistics should the behavior compute ?<BR>
    <BR>
    /* warning:
    - When the "Statistics" building block stops, the profiler window will stop displaying informations. You'll need
    to close and open again to profiler to have it working.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5fb70201, 0x65595af3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateStatsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Optimizations/System");
    return od;
}

CKERROR CreateStatsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Statistics");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Set the states of the inputs
    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    // Declare the settings in three groups
    proto->DeclareSetting("Frame Rate", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("General Time", CKPGUID_GENERALSTAT, "Interface,Processing,Rendering,Parametric Operation,Animation Management,IK Management,Total Behavior,Behavior Execution");
    proto->DeclareSetting("Behavioral", CKPGUID_BEHAVIORSTAT, "Active Objects Executed,Behaviors Executed,Building Blocks Executed,Links Parsed,Links Delayed");
    proto->DeclareSetting("Rendering", CKPGUID_RENDERSTAT, "Faces Drawn, Lines Drawn, Points Drawn, Objects Drawn, Vertices Processed");

    // Corresponding Outputs
    proto->DeclareOutParameter("Frame Rate", CKPGUID_FLOAT);

    // Time Statistics
    proto->DeclareOutParameter("Interface Time", CKPGUID_TIME);
    proto->DeclareOutParameter("Processing Time", CKPGUID_TIME);
    proto->DeclareOutParameter("Rendering Time", CKPGUID_TIME);
    proto->DeclareOutParameter("Parametric operations Time", CKPGUID_TIME);
    proto->DeclareOutParameter("Animation Management Time", CKPGUID_TIME);
    proto->DeclareOutParameter("IK Management Time", CKPGUID_TIME);
    proto->DeclareOutParameter("Total Behavior Time", CKPGUID_TIME);
    proto->DeclareOutParameter("Behavior Execution Time", CKPGUID_TIME);

    // Behavior Statistics
    proto->DeclareOutParameter("Active Object Executed", CKPGUID_INT);
    proto->DeclareOutParameter("Behaviors Executed", CKPGUID_INT);
    proto->DeclareOutParameter("Building Block Executed", CKPGUID_INT);
    proto->DeclareOutParameter("Links Parsed", CKPGUID_INT);
    proto->DeclareOutParameter("Links Delayed", CKPGUID_INT);

    // Rendering statistics
    proto->DeclareOutParameter("Faces Drawn", CKPGUID_INT);
    proto->DeclareOutParameter("Lines Drawn", CKPGUID_INT);
    proto->DeclareOutParameter("Points Drawn", CKPGUID_INT);
    proto->DeclareOutParameter("Objects Drawn", CKPGUID_INT);
    proto->DeclareOutParameter("Vertices Processed", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorCallbackFct(StatsCallback, CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORLOAD);
    proto->SetFunction(Stats);

    *pproto = proto;
    return CK_OK;
}

////////////////////////////////////
//			Main Function
////////////////////////////////////
int Stats(const CKBehaviorContext &behcontext)
{
    // First, we get the behavior and the delta time
    CKBehavior *beh = behcontext.Behavior;
    int Flag;

    // Input and Output settings
    if (beh->IsInputActive(0)) // On
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        // First, we enable the profiling to make the system updates the stats values
        behcontext.Context->EnableProfiling(true);
    }
    else if (beh->IsInputActive(1)) // Off
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        // Finally, we disable the profiling
        behcontext.Context->EnableProfiling(false);

        // Stops the behavior
        return CKBR_OK;
    }

    // Gets the informations from CK
    CKStats stats;
    behcontext.Context->GetProfileStats(&stats);

    // Gets the informations from VxMath
    VxStats vxstats;
    behcontext.CurrentRenderContext->GetStats(&vxstats);

    // Sets the frame rate information
    beh->GetLocalParameterValue(0, &Flag);

    if (Flag)
    {
        float fps;
        fps = 1000 / stats.TotalFrameTime;
        beh->SetOutputParameterValue(FRAME_RATE, &fps);
    }

    // Sets the needed outputs for general time statistics
    beh->GetLocalParameterValue(1, &Flag);

    if (Flag & GF_INTERFACE)
        beh->SetOutputParameterValue(INTERFACETIME, &stats.EstimatedInterfaceTime);

    if (Flag & GF_PROCESSING)
        beh->SetOutputParameterValue(PROCESSING, &stats.ProcessTime);

    if (Flag & GF_RENDERING)
        beh->SetOutputParameterValue(RENDERING, &stats.RenderTime);

    if (Flag & GF_PO)
        beh->SetOutputParameterValue(PO, &stats.ParametricOperations);

    if (Flag & GF_ANIMATION)
        beh->SetOutputParameterValue(ANIMATION, &stats.AnimationManagement);

    if (Flag & GF_IK)
        beh->SetOutputParameterValue(IK, &stats.IKManagement);

    if (Flag & GF_TOTAL_BEHAVIOR)
        beh->SetOutputParameterValue(TOTAL_BEHAVIOR, &stats.TotalBehaviorExecution);

    if (Flag & GF_BEHAVIOR)
        beh->SetOutputParameterValue(BEHAVIOR, &stats.BehaviorCodeExecution);

    // Sets the needed outputs for behavioral statistics
    beh->GetLocalParameterValue(2, &Flag);

    if (Flag & BF_ACTIVE_OBJECT)
        beh->SetOutputParameterValue(ACTIVE_OBJECT, &stats.ActiveObjectsExecuted);

    if (Flag & BF_BEHAVIOR_EXECUTED)
        beh->SetOutputParameterValue(BEHAVIOR_EXECUTED, &stats.BehaviorsExecuted);

    if (Flag & BF_BB_EXECUTED)
        beh->SetOutputParameterValue(BB_EXECUTED, &stats.BuildingBlockExecuted);

    if (Flag & BF_LINK_PARSED)
        beh->SetOutputParameterValue(LINK_PARSED, &stats.BehaviorLinksParsed);

    if (Flag & BF_LINK_DELAYED)
        beh->SetOutputParameterValue(LINK_DELAYED, &stats.BehaviorDelayedLinks);

    // Sets the needed outputs for renderal statistics
    beh->GetLocalParameterValue(3, &Flag);

    if (Flag & RF_FACE_DRAWN)
        beh->SetOutputParameterValue(FACE_DRAWN, &vxstats.NbTrianglesDrawn);

    if (Flag & RF_LINES_DRAWN)
        beh->SetOutputParameterValue(LINES_DRAWN, &vxstats.NbLinesDrawn);

    if (Flag & RF_POINTS_DRAWN)
        beh->SetOutputParameterValue(POINTS_DRAWN, &vxstats.NbPointsDrawn);

    if (Flag & RF_OBJECTS_DRAWN)
        beh->SetOutputParameterValue(OBJECTS_DRAWN, &vxstats.NbObjectDrawn);

    if (Flag & RF_VERTICES_PROCESSED)
        beh->SetOutputParameterValue(VERTICES_PROCESSED, &vxstats.NbVerticesProcessed);

    // Everything went fine
    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** Callback Function ****************/
CKERROR StatsCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // The Frame Rate Output
    int FrameRateFlag;
    beh->GetLocalParameterValue(0, &FrameRateFlag);
    beh->EnableOutputParameter(FRAME_RATE, FrameRateFlag);

    // The General Statistics Outputs
    int Flag;
    beh->GetLocalParameterValue(1, &Flag);
    beh->EnableOutputParameter(INTERFACETIME, Flag & GF_INTERFACE);
    beh->EnableOutputParameter(PROCESSING, Flag & GF_PROCESSING);
    beh->EnableOutputParameter(RENDERING, Flag & GF_RENDERING);
    beh->EnableOutputParameter(PO, Flag & GF_PO);
    beh->EnableOutputParameter(ANIMATION, Flag & GF_ANIMATION);
    beh->EnableOutputParameter(IK, Flag & GF_IK);
    beh->EnableOutputParameter(TOTAL_BEHAVIOR, Flag & GF_TOTAL_BEHAVIOR);
    beh->EnableOutputParameter(BEHAVIOR, Flag & GF_BEHAVIOR);

    // The Behavorial Statistics
    beh->GetLocalParameterValue(2, &Flag);
    beh->EnableOutputParameter(ACTIVE_OBJECT, Flag & BF_ACTIVE_OBJECT);
    beh->EnableOutputParameter(BEHAVIOR_EXECUTED, Flag & BF_BEHAVIOR_EXECUTED);
    beh->EnableOutputParameter(BB_EXECUTED, Flag & BF_BB_EXECUTED);
    beh->EnableOutputParameter(LINK_PARSED, Flag & BF_LINK_PARSED);
    beh->EnableOutputParameter(LINK_DELAYED, Flag & BF_LINK_DELAYED);

    // The Renderal Statistics
    beh->GetLocalParameterValue(3, &Flag);
    beh->EnableOutputParameter(FACE_DRAWN, Flag & RF_FACE_DRAWN);
    beh->EnableOutputParameter(LINES_DRAWN, Flag & RF_LINES_DRAWN);
    beh->EnableOutputParameter(POINTS_DRAWN, Flag & RF_POINTS_DRAWN);
    beh->EnableOutputParameter(OBJECTS_DRAWN, Flag & RF_OBJECTS_DRAWN);
    beh->EnableOutputParameter(VERTICES_PROCESSED, Flag & RF_VERTICES_PROCESSED);

    return CKBR_OK;
}