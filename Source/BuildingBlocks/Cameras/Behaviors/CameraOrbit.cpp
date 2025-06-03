/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Camera Orbit
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorOrbitDecl();
CKERROR CreateOrbitProto(CKBehaviorPrototype **pproto);
int Orbit(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorOrbitDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Camera Orbit");
    od->SetDescription("Makes a Camera orbit round a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Target: </SPAN>3D Entity to target.<BR>
    <SPAN CLASS=pin>Distance: </SPAN>current distance between the Camera and its target.<BR>
    <BR>
    The following keys are used by default to move the Camera around its target:<BR>
    <BR>
    <FONT COLOR=#FFFFFF>Page Up: </FONT>Zoom in.<BR>
    <FONT COLOR=#FFFFFF>Page Down: </FONT>Zoom out.<BR>
    <FONT COLOR=#FFFFFF>Up and Down Arrows: </FONT>Rotate around the view X axis.<BR>
    <FONT COLOR=#FFFFFF>Left and Right Arrows: </FONT>Rotate around the view Y axis.<BR>
    <FONT COLOR=#FFFFFF>RIGHT SHIFT: </FONT>Velocity x 2.<BR>
    The arrow keys are the inverted T arrow keys and not the ones of the numeric keypad.<BR>
    <BR>
    <SPAN CLASS=setting>Key Zoom in: </SPAN>Key used to zoom in.<BR>
    <SPAN CLASS=setting>Key Zoom out: </SPAN>Key used to zoom in.<BR>
    <SPAN CLASS=setting>Key Rotate Up: </SPAN>Key used to rotate up.<BR>
    <SPAN CLASS=setting>Key Rotate Down: </SPAN>Key used to rotate down.<BR>
    <SPAN CLASS=setting>Key Rotate Left: </SPAN>Key used to rotate left.<BR>
    <SPAN CLASS=setting>Key Rotate Right: </SPAN>Key used to rotate right.<BR>
    <SPAN CLASS=setting>Key Speed x2: </SPAN>Key used to speed up motion by 2.<BR>
    <BR>
    */
    od->SetCategory("Cameras/Movement");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x777d999e, 0xdef777d8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateOrbitProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateOrbitProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Camera Orbit");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Target", CKPGUID_3DENTITY);

    proto->DeclareOutParameter("Distance", CKPGUID_FLOAT);

    proto->DeclareSetting("Key Zoom In", CKPGUID_KEY, "Numpad -");
    proto->DeclareSetting("Key Zoom Out", CKPGUID_KEY, "Numpad +");
    proto->DeclareSetting("Key Rotate Up", CKPGUID_KEY, "Up Arrow");
    proto->DeclareSetting("Key Rotate Down", CKPGUID_KEY, "Down Arrow");
    proto->DeclareSetting("Key Rotate Left", CKPGUID_KEY, "Left Arrow");
    proto->DeclareSetting("Key Rotate Right", CKPGUID_KEY, "Right Arrow");
    proto->DeclareSetting("Key Speed x2", CKPGUID_KEY, "Right Shift");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Orbit);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Orbit(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    // Get Key Codes
    int key_zoomin = 0;
    beh->GetLocalParameterValue(0, &key_zoomin);
    if (!key_zoomin)
        key_zoomin = CKKEY_PRIOR;
    int key_zoomout = 0;
    beh->GetLocalParameterValue(1, &key_zoomout);
    if (!key_zoomout)
        key_zoomout = CKKEY_NEXT;
    int key_rotateup = 0;
    beh->GetLocalParameterValue(2, &key_rotateup);
    if (!key_rotateup)
        key_rotateup = CKKEY_UP;
    int key_rotatedown = 0;
    beh->GetLocalParameterValue(3, &key_rotatedown);
    if (!key_rotatedown)
        key_rotatedown = CKKEY_DOWN;
    int key_rotateleft = 0;
    beh->GetLocalParameterValue(4, &key_rotateleft);
    if (!key_rotateleft)
        key_rotateleft = CKKEY_LEFT;
    int key_rotateright = 0;
    beh->GetLocalParameterValue(5, &key_rotateright);
    if (!key_rotateright)
        key_rotateright = CKKEY_RIGHT;
    int key_speedx2 = 0;
    beh->GetLocalParameterValue(6, &key_speedx2);
    if (!key_speedx2)
        key_speedx2 = CKKEY_RSHIFT;

    // Object to follow
    CK3dEntity *ent = (CK3dEntity *)beh->GetInputParameterObject(0);

    VxVector pos_tar;
    float radius;

    if (ent)
    {
        ent->GetPosition(&pos_tar);
        radius = ent->GetRadius();
    }
    else
    {
        radius = 10.0f;
        pos_tar = VxVector::axis0();
    }

    if (radius <= 0.01f)
        radius = 0.01f;

    VxVector pos;

    // Get the Target of camera
    CK3dEntity *tar;
    CKBOOL CameraHasTarget = CKIsChildClassOf(cam, CKCID_TARGETCAMERA) && (tar = ((CKTargetCamera *)cam)->GetTarget());
    if (CameraHasTarget)
    {
        tar->SetPosition(&pos_tar);
    }
    else
    {
        tar = ent;
    }

    //________________// Left / Right / Up / Down
    cam->GetPosition(&pos, NULL);
    pos -= pos_tar;

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
        return CKBR_GENERICERROR;

    float d = Magnitude(pos);
    float ratio = (input->IsKeyDown(key_speedx2)) ? .003f : .001f;
    //	float s = d * radius * ratio;
    float s = d * behcontext.DeltaTime * ratio;

    VxVector dep(0.0f);
    if (input->IsKeyDown(key_rotateup))
        dep.y = s;
    if (input->IsKeyDown(key_rotatedown))
        dep.y += -s;
    if (input->IsKeyDown(key_rotateleft))
        dep.x = -s;
    if (input->IsKeyDown(key_rotateright))
        dep.x += s;

    cam->Translate(&dep, cam, FALSE);

    //________________// Forward / Backward
    cam->GetPosition(&pos, NULL);
    pos -= pos_tar;

    float tmp = Magnitude(pos);
    dep.x = 0.0f;
    dep.y = 0.0f;
    dep.z = tmp - d;

    if (input->IsKeyDown(key_zoomin))
        dep.z += s;
    if (input->IsKeyDown(key_zoomout))
        dep.z += -s;

    cam->Translate(&dep, cam, FALSE);

    //________________// Output Distance
    d = tmp - dep.z;
    beh->SetOutputParameterValue(0, &d);

    //________________// Look At target
    if (!CameraHasTarget)
    {
        cam->LookAt(&pos_tar);
    }

    return CKBR_OK;
}
