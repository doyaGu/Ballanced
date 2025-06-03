/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Slider
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorSliderDecl();
CKERROR CreateSliderProto(CKBehaviorPrototype **);
int Slider(const CKBehaviorContext &behcontext);
CKERROR SliderCallback(const CKBehaviorContext &behcontext);
CKBOOL SliderRenderCallback(CKRenderContext *context, CKRenderObject *ent, void *Argument);

CKObjectDeclaration *FillBehaviorSliderDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("Slider");
	od->SetDescription("");
	/* rem:
	<SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
	<SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
	<BR>
	<SPAN CLASS=pin>Material: </SPAN>.<BR>
	*/
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x571379bf, 0xe8233bd));
	od->SetAuthorGuid(VIRTOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSliderProto);
	od->SetCompatibleClassId(CKCID_2DENTITY);
	od->SetCategory("Interface/Controls");
	return od;
}

CKERROR CreateSliderProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Slider");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("On");
	proto->DeclareInput("Off");

	proto->DeclareOutput("Exit On");
	proto->DeclareOutput("Exit Off");
	proto->DeclareOutput("Value Changed");

	proto->DeclareInParameter("Steps", CKPGUID_INT, "10");
	proto->DeclareInParameter("Initial Value", CKPGUID_INT, "0");
	proto->DeclareInParameter("Slider", CKPGUID_2DENTITY);
	proto->DeclareInParameter("Relative", CKPGUID_BOOL, "FALSE");
	proto->DeclareInParameter("Plus Arrow", CKPGUID_2DENTITY);
	proto->DeclareInParameter("Minus Arrow", CKPGUID_2DENTITY);
	proto->DeclareInParameter("Slider Background", CKPGUID_MATERIAL);

	proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); // State
	proto->DeclareSetting("Vertical", CKPGUID_BOOL, "FALSE");

	proto->DeclareOutParameter("Value", CKPGUID_INT, "0");
	proto->DeclareOutParameter("Percentage", CKPGUID_PERCENTAGE, "0");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(Slider);

	*pproto = proto;
	return CK_OK;
}

#define RELEASED 0
#define PRESSED 1
#define ROLLOVER 2

void MoveSlider(int value, int steps, CK2dEntity *slider, VxRect &framerect, CKBOOL scale, CKBOOL vert)
{
	VxRect sliderRect;
	slider->GetRect(sliderRect);

	if (vert) // Vertical slider
	{
		// Horizontal placement (center for now)
		sliderRect.VMove(framerect.left + framerect.GetWidth() * 0.5f - sliderRect.GetWidth() * 0.5f);

		float f = (float)value / steps;
		if (f > 1.0f)
			f = 1.0f;
		else if (f < 0.0f)
			f = 0.0f;
		if (scale)
		{
            sliderRect.top = framerect.top;
            sliderRect.bottom = framerect.top + f * framerect.GetHeight();
		}
		else
		{
			sliderRect.VMove(framerect.top + f * (framerect.GetHeight() - sliderRect.GetHeight()));
		}
	}
	else // Horizontal slider
	{
		// Vertical placement (center for now)
		sliderRect.VMove(framerect.top + framerect.GetHeight() * 0.5f - sliderRect.GetHeight() * 0.5f);

		float f = (float)value / steps;
		if (f > 1.0f)
			f = 1.0f;
		else if (f < 0.0f)
			f = 0.0f;
		if (scale)
		{
            sliderRect.left = framerect.left;
            sliderRect.right = framerect.left + f * framerect.GetWidth();
		}
		else
		{
			sliderRect.HMove(framerect.left + f * (framerect.GetWidth() - sliderRect.GetWidth()));
		}
	}

	slider->SetRect(sliderRect);
}

int Slider(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;

	if (beh->IsInputActive(1))
	{
		beh->ActivateInput(1, FALSE);
		return CKBR_OK;
	}

	CK2dEntity *frame = (CK2dEntity *)beh->GetTarget();
	if (!frame)
		return CKBR_ACTIVATENEXTFRAME;

	VxRect frameRect;
	frame->GetRect(frameRect);

	CK2dEntity *slider = (CK2dEntity *)beh->GetInputParameterObject(2);
	if (!slider)
		return CKBR_ACTIVATENEXTFRAME;

	int *oldValue = (int *)beh->GetOutputParameterWriteDataPtr(0);

	int steps = 10;
	beh->GetInputParameterValue(0, &steps);

	CKBOOL scale = FALSE;
	beh->GetInputParameterValue(3, &scale);

	CKBOOL vert = FALSE;
	beh->GetLocalParameterValue(1, &vert);

	if (beh->IsInputActive(0))
	{
		beh->ActivateInput(0, FALSE);
		int state = 0;
		beh->SetLocalParameterValue(0, &state);

		int initialValue = 0;
		beh->GetInputParameterValue(1, &initialValue);

		MoveSlider(initialValue, steps, slider, frameRect, scale, vert);
	}

	CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
	if (!im)
		return CKBR_ACTIVATENEXTFRAME;

	// TODO : button configurable
	int button = 0;

	VxRect rect;
	slider->GetRect(rect);

	Vx2DVector mp;
	im->GetMousePosition(mp, FALSE);

	CKBOOL inside = FALSE;
	if (rect.IsInside(mp))
		inside = TRUE;

	int state = 0;
	beh->GetLocalParameterValue(0, &state);

	int oldState = state;

	if (state & PRESSED)
	{
		// we calculate the new value, according to the mouse position
		int value = (int)((steps + 1) * (mp.x - frameRect.left) / frameRect.GetWidth());

		if (value != *oldValue)
		{
			*oldValue = value;
			MoveSlider(value, steps, slider, frameRect, scale, vert);
		}
	}

	if (im->IsMouseClicked(CK_MOUSEBUTTON(button)) && inside)
	{
		state = PRESSED;
	}
	else
	{
		if (im->IsMouseButtonDown(CK_MOUSEBUTTON(button)))
		{
			if (frameRect.IsInside(mp)) // we clicked on the slider background : direct jump
			{
				// we calculate the new value, according to the mouse position
				int value = (int)((steps + 1) * (mp.x - frameRect.left) / frameRect.GetWidth());

				if (value != *oldValue)
				{
					*oldValue = value;
					MoveSlider(value, steps, slider, frameRect, scale, vert);
				}
			}
		}
		else
		{
			if (state & PRESSED)
			{
				state = RELEASED;
			}
			else if (inside)
				state = ROLLOVER;
			else
				state = RELEASED;
		}
	}

	if (state != oldState) // State change
	{
		// we activate the corresponding output
		beh->ActivateOutput(state);

		// we store the new state
		beh->SetLocalParameterValue(0, &state);
	}

	// add pre render callback
	frame->AddPreRenderCallBack(SliderRenderCallback, (void *)beh->GetID(), TRUE);

	return CKBR_ACTIVATENEXTFRAME;
}

CKBOOL SliderRenderCallback(CKRenderContext *dev, CKRenderObject *ent, void *iArg)
{
	static VxRect fulluv(0, 0, 1.0f, 1.0f);

	CKContext *ctx = dev->GetCKContext();
	CKBehavior *beh = (CKBehavior *)ctx->GetObject((CK_ID)iArg);
	if (!beh)
		return TRUE;

	CK2dEntity *frame = (CK2dEntity *)beh->GetTarget();
	if (!frame)
		return TRUE;

	// 2D Frame Zone
	VxRect rect;
	frame->GetRect(rect);

	int state = 0;
	beh->GetLocalParameterValue(0, &state);

	CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(6);
	VxColor col(1.0f, 1.0f, 1.0f, 1.0f);
	if (mat)
		col = mat->GetDiffuse();

	// We draw the button background
	DrawFillRectangle(dev, col.GetRGBA(), mat, rect, fulluv);

	return TRUE;
}
