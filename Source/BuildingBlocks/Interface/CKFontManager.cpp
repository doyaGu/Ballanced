/*************************************************************************/
/*	File : CKFontManager.cpp											 */
/*																		 */
/*************************************************************************/
#include "CKAll.h"

#include "CKFontManager.h"

#define FONTMANAGERVERSION1 1

#define FONTSIZE128 (-5)
#define FONTSIZE256 (-10)
#define FONTSIZE512 (-20)
#define FONTSIZE1024 (-40)

const char *CKFontManager::Name = "Font Manager";

// Post render callback
CKFontManager::CKFontManager(CKContext *ctx) : CKBaseManager(ctx, FONT_MANAGER_GUID, (char *)Name)
{
	ctx->RegisterNewManager(this);

    m_EntityUnderMouse = NULL;
    m_FontAttribute = 0;
	m_TextureLoaded = FALSE;
	m_VersionUpdate = FALSE;

#ifndef FONTMANAGER_NOSYSFONT
	// System font device context initialisation
	m_DC = ::CreateCompatibleDC(NULL);
	if (m_DC == NULL)
		throw "Font Manager -- Unable to create a screen compatible DC";
#endif
}

CKFontManager::~CKFontManager()
{
	for (CKTextureFont **it = m_FontArray.Begin(); it != m_FontArray.End(); it++)
	{
		CKTextureFont *font = *it;
		DeleteFontHandle(font->m_FontName);
		delete font;
	}

	m_FontArray.Clear();

	ClearTextData();

#ifndef FONTMANAGER_NOSYSFONT
	// Release memory device context
	::DeleteDC(m_DC);
#endif
}

void CKFontManager::DeleteFont(int fontIndex)
{
	CKTextureFont *font = GetFont(fontIndex);
	if (!font)
		return;

	m_FontArray.RemoveAt(fontIndex - 1);
	DeleteFontHandle(font->m_FontName);
	CKTexture *tex = font->GetFontTexture();
	if (tex->IsDynamic())
	{
		// we now search if the texture is used anywhere else
		CKTextureFont **it = m_FontArray.Begin();
		for (; it != m_FontArray.End(); it++)
		{
			if ((*it)->GetFontTexture() == tex)
				break;
		}

		if (it == m_FontArray.End()) // Not used : we delete it
		{
			CKDestroyObject(tex);
		}
	}
	delete font;

	RebuildFontEnumeration();
}

// Delete the font data
void CKFontManager::ClearFontData()
{
	// delete the texture font and the font handle if exists
	for (CKTextureFont **it = m_FontArray.Begin(); it != m_FontArray.End();)
	{
		CKTextureFont *font = *it;
		CKTexture *texture = (CKTexture *)m_Context->GetObject(font->m_FontTexture);
		if (!texture || texture->IsDynamic())
		{
			DeleteFontHandle(font->m_FontName);
			delete font;
			it = m_FontArray.Remove(it);
		}
		else
			++it;
	}
}

// Delete the text data struct
void CKFontManager::ClearTextData()
{
	// Delete the text data for multiple settings if exists
	// this tables contains only references
	m_2DTexts.Resize(0);
	m_3DTexts.Resize(0);
	// the real data are here, we delete them
	for (TextDataTableIterator textIt = m_Texts.Begin(); textIt != m_Texts.End(); textIt++)
		delete *textIt;
	m_Texts.Clear();
}

CKERROR CKFontManager::PostClearAll()
{
	RegisterAttributes();
	ClearFontData();
	ClearTextData();

#ifndef FONTMANAGER_NOSYSFONT
	// Delete all font objects...
	FontIterator it = m_Fonts.Begin();
	FontIterator fend = m_Fonts.End();

	// And we release the handle
	while (it != fend)
	{
		::DeleteObject(*it);
		++it;
	}
	m_Fonts.Clear();
#endif

	return CK_OK;
}

void CKFontManager::RegisterAttributes()
{
	CKParameterManager *pm = m_Context->GetParameterManager();
	pm->RegisterNewEnum(CKPGUID_FONT, "Font Type", "No Font=0");
	pm->ChangeEnumDeclaration(CKPGUID_FONT, "No Font=0");
	CKParameterTypeDesc *pt = pm->GetParameterTypeDescription(CKPGUID_FONT);
	pt->Saver_Manager = FONT_MANAGER_GUID;
}

CKERROR CKFontManager::OnCKInit()
{
	RegisterAttributes();
	RegenerateFontEnumeration();

	return CK_OK;
}

CKERROR CKFontManager::OnCKEnd()
{
	return CK_OK;
}

CKERROR CKFontManager::OnPostRender(CKRenderContext *dev)
{
	DrawTextCallback3D(dev, this);

	return CK_OK;
}

CKERROR CKFontManager::OnPostSpriteRender(CKRenderContext *dev)
{
	// 2.5 change
	// changes for text to appear after the 2D entities
	DrawTextCallback2D(dev, this);

	DrawRectanglesCallback(dev);
	return CK_OK;
}

CKERROR CKFontManager::PreProcess()
{
    // Clear the reference table for next use
    m_2DTexts.Resize(0);
    m_3DTexts.Resize(0);
    m_Rectangles.Resize(0);

	///
	// we need to find the 2DEntity under the mouse cursor
	m_EntityUnderMouse = NULL;

	CKInputManager *im = (CKInputManager *)m_Context->GetManagerByGuid(INPUT_MANAGER_GUID);
	if (!im)
		return CK_OK;

	Vx2DVector mp;
	im->GetMousePosition(mp, FALSE);

	CKRenderContext *rc = m_Context->GetPlayerRenderContext();
	if (rc)
	{
		VxRect screen;
		rc->GetWindowRect(screen, FALSE);
		mp += screen.GetTopLeft();
		m_EntityUnderMouse = rc->Pick2D(mp);
	}

	return CK_OK;
}

CKERROR CKFontManager::OnCKReset()
{
	ClearFontData();
	ClearTextData();
	return CK_OK;
}

/*************************************************
Name: SequenceToBeDeleted

Summary: System font could be created from dynamical texture.
We must destroy these font if the texture is deleted

Arguments: the objects deleted

Return Value:

Remarks:

See also:

*************************************************/
CKERROR CKFontManager::SequenceToBeDeleted(CK_ID *objIds, int count)
{
	// Check the texture
	OnTextureToBeDeleted();

	// We check the compiled data
	for (CompiledTextTableIterator compIt = m_CompiledText.Begin(); compIt != m_CompiledText.End();)
	{
		CKObject *obj = m_Context->GetObject(compIt.GetKey());
		if (!obj || obj->IsToBeDeleted())
		{
			delete *compIt;
			compIt = m_CompiledText.Remove(compIt);
		}
		else
			compIt++;
	}

	// check the 3D texts
	TextData **td;
	for (td = m_3DTexts.Begin(); td != m_3DTexts.End();)
	{
		CK3dEntity *ent = (CK3dEntity *)m_Context->GetObject((*td)->m_Entity);
		if (!ent || ent->IsToBeDeleted())
		{
			td = m_3DTexts.Remove(td);
		}
		else
			++td;
	}

	// check the 2D texts
	for (td = m_2DTexts.Begin(); td != m_2DTexts.End();)
	{
		CK2dEntity *ent = (CK2dEntity *)m_Context->GetObject((*td)->m_Entity);
		if (!ent || ent->IsToBeDeleted())
		{
			td = m_2DTexts.Remove(td);
		}
		else
			++td;
	}

	// We check the text Data
	for (TextDataTableIterator textIt = m_Texts.Begin(); textIt != m_Texts.End();)
	{
		CKObject *obj = m_Context->GetObject(textIt.GetKey());
		if (!obj || obj->IsToBeDeleted())
		{
			delete *textIt;
			textIt = m_Texts.Remove(textIt);
		}
		else
			textIt++;
	}

	return CK_OK;
}

// Conveniences for SequenceToBeDeleted
void CKFontManager::OnTextureToBeDeleted()
{
	int index = 0;
	CKTextureFont **it = m_FontArray.Begin();
	while (it != m_FontArray.End())
	{
		CKTextureFont *font = *it;
		CKTexture *tex = (CKTexture *)m_Context->GetObject(font->m_FontTexture);
		if (tex && tex->IsToBeDeleted())
		{
			// the texture is destroyed : delete the associated logical font if any
			DeleteFontHandle(font->m_FontName);
			// we delete the texture
			delete font;
			// and we remove it from the array
			it = m_FontArray.Remove(it);

			// We update the local parameters using this deleted texture
			CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_PARAMETERLOCAL);
			int j, idCount = m_Context->GetObjectsCountByClassID(CKCID_PARAMETERLOCAL);

			for (j = 0; j < idCount; ++j)
			{
				CKParameterLocal *pl = (CKParameterLocal *)m_Context->GetObject(ids[j]);
				if (pl && (pl->GetGUID() == CKPGUID_FONT))
				{
					int *value = (int *)pl->GetWriteDataPtr();
					if (*value == (index + 1))
						*value = 0;
					else if (*value > (index + 1))
						(*value)--;
				}
			}

			ids = m_Context->GetObjectsListByClassID(CKCID_PARAMETEROUT);
            idCount = m_Context->GetObjectsCountByClassID(CKCID_PARAMETEROUT);

			for (j = 0; j < idCount; ++j)
			{
				CKParameterOut *pl = (CKParameterOut *)m_Context->GetObject(ids[j]);
				if (pl && (pl->GetGUID() == CKPGUID_FONT))
				{
					int *value = (int *)pl->GetWriteDataPtr();
					if (*value == (index + 1))
						*value = 0;
					else if (*value > (index + 1))
						(*value)--;
				}
			}
		}
		else
		{
			++it;
			++index;
		}
	}
}

/*************************************************
Name: RebuildFontEnumeration

Summary: Build the enumeration representing the available fonts

Arguments:

Return Value:

Remarks: Called at font creation or font deletion

See also:

*************************************************/
void CKFontManager::RebuildFontEnumeration()
{
	XString xenumstring;
	XString xtmp;

	int index = 1;
	// Scan all available fonts
	for (CKTextureFont **it = m_FontArray.Begin(); it != m_FontArray.End(); it++, ++index)
	{
		CKTextureFont *font = *it;
		xtmp.Format("%s=%d", font->m_FontName, index);
		xenumstring += xtmp;
		if (index != m_FontArray.Size())
			xenumstring += ",";
	}

	if (!xenumstring.Length())
		xenumstring = "No Font=0";

	// Register the new enumeration
	m_Context->GetParameterManager()->ChangeEnumDeclaration(CKPGUID_FONT, xenumstring.Str());
}

/*************************************************
Name: GetFontIndex

Summary: Get the index of a font from its name

Arguments:

Return Value:

Remarks:

See also:

*************************************************/
int CKFontManager::GetFontIndex(CKSTRING name)
{
	if (!name)
		return 0;

	int index = 1;

	for (CKTextureFont **it = m_FontArray.Begin(); it != m_FontArray.End(); it++, index++)
	{
		CKTextureFont *font = *it;
		if (font->m_FontName)
			if (font && !strcmp(font->m_FontName, name))
				return index;
	}

	return 0;
}

/*************************************************
Name: RegisterFont

Summary: Add a new font to the font manager

Arguments: a CKTextureFont representing the font

Return Value:

Remarks:

See also:

*************************************************/
int CKFontManager::RegisterFont(CKTextureFont *font)
{
	// Add to the font array
	m_FontArray.PushBack(font);

	// Change the enum
	RebuildFontEnumeration();

	// return Index+1
	return m_FontArray.Size();
}

int CKFontManager::CreateTextureFont(CKSTRING fontName, CKTexture *fontTexture, VxRect &textZone, Vx2DVector &charNumber, CKBOOL fixed, int firstCharacter, float iSpaceSize)
{
	if (!fontTexture)
		return -1;
	if (!fontName)
		return -1;

	// We create the new font
	CKTextureFont *font = new CKTextureFont(this, m_Context, fontName);
	font->CreateCKFont(fontTexture, textZone, charNumber, fixed, firstCharacter, iSpaceSize);
	font->m_ScreenExtents.Set(fontTexture->GetWidth(), fontTexture->GetHeight());

	// We look for existent
	int index = GetFontIndex(fontName);
	if (index) // the font was found.
	{
		// we delete the old one
		CKTextureFont *old = m_FontArray[index - 1];
		delete old;

		// we store the new one
		m_FontArray[index - 1] = font;

		return index;
	}
	else // No similar font found : we create a new one
	{
		// We add the font to the font array
		return RegisterFont(font);
	}
}

CKTextureFont *CKFontManager::GetFont(unsigned int fontIndex)
{
	if (fontIndex <= 0)
		return NULL;
	if (fontIndex > (unsigned int)m_FontArray.Size())
		return NULL;

	CKTextureFont *tf = m_FontArray[fontIndex - 1];
	if (tf->m_SpacingProperties & CKTextureFont::CREATED)
		return tf;
	else
		return NULL;
}

// Text Lines

void CKFontManager::ClearLines()
{
	m_Lines.Resize(0);
}

void CKFontManager::AddLine(LineData &data)
{
	m_Lines.PushBack(data);
}

int CKFontManager::GetLineCount()
{
	return m_Lines.Size();
}

LineData *CKFontManager::GetLine(int i)
{
	if (i < m_Lines.Size())
	{
		return &m_Lines[i];
	}
	return NULL;
}

#define FONT_VERSION1 0x8001
#define FONT_VERSION2 0x8002

CKStateChunk *CKFontManager::SaveData(CKFile *SavedFile)
{
	if (!m_FontArray.Size())
		return NULL;

	///////////////////////////////////
	// FONTS SAVING
	//////////////////////////////////

	int textureCount = 0;
	CKTextureFont **it;
	for (it = m_FontArray.Begin(); it != m_FontArray.End(); it++)
	{
		CKTextureFont *font = *it;

		CKTexture *texture = (CKTexture *)m_Context->GetObject(font->m_FontTexture);
		if (!texture)
			continue; // does not exist
		if (texture->IsDynamic())
			continue; // does not need to be saved
		if (!SavedFile->IsObjectToBeSaved(font->m_FontTexture))
			continue; // the texture is not going to be saved

		textureCount++;
	}

	if (!textureCount)
		return NULL;

	CKStateChunk *chunk = CreateCKStateChunk((CK_CLASSID)FONTMANAGERVERSION1, SavedFile);
	chunk->StartWrite();

	// Font Identifier
	chunk->WriteIdentifier(FONT_VERSION2);

	// we write the number of fonts
	chunk->WriteInt(textureCount);

	// Fonts processing
	for (it = m_FontArray.Begin(); it != m_FontArray.End(); it++)
	{
		CKTextureFont *font = *it;
		CKTexture *texture = (CKTexture *)m_Context->GetObject(font->m_FontTexture);
		if (!texture)
			continue; // does not exist
		if (texture->IsDynamic())
			continue; // does not need to be saved
		if (!SavedFile->IsObjectToBeSaved(font->m_FontTexture))
			continue; // the texture is not going to be saved

		// FontName
		chunk->WriteString(font->m_FontName);
		// CharNumber
		chunk->WriteBuffer(sizeof(Vx2DVector), &(font->m_CharNumber));
		// FontZone
		chunk->WriteBuffer(sizeof(VxRect), &(font->m_FontZone));
		// Texture ID
		chunk->WriteObjectID(font->m_FontTexture);
		// Spacing Properties
		chunk->WriteInt(font->m_SpacingProperties & CKTextureFont::TOBESAVED);
		// First Character
		chunk->WriteInt(font->m_FirstCharacter);
		// we now maybe have to save the spacing properties for the given font
		if (font->m_SpacingProperties & CKTextureFont::SPACINGTOBESAVED)
		{
			chunk->WriteBufferNoSize_LEndian(256 * sizeof(CharacterTextureCoordinates), &(font->m_FontCoordinates));
		}
	}

	chunk->CloseChunk();
	return chunk;
}

// {secret}
CKERROR CKFontManager::LoadData(CKStateChunk *chunk, CKFile *LoadedFile)
{
	if (!chunk)
		return CKERR_INVALIDPARAMETER;

	chunk->StartRead();

	///////////////////////////////////
	// FONTS LOADING
	//////////////////////////////////

	// font Identifier
	int version = 2;
	if (!chunk->SeekIdentifier(FONT_VERSION2))
	{
		if (chunk->SeekIdentifier(FONT_VERSION1))
			version = 1;
		else
			version = 0;
	}

	if (version)
	{
		// we read the number of fonts
		int fn = chunk->ReadInt();

		int *ConversionTable = new int[fn + 1];
		// No Remapping for the first one
		memset(ConversionTable, 0, 4 * (fn + 1));

		char buffer[32];
		// we read the font
		for (int h = 0; h < fn; h++)
		{
			// Font Name
			chunk->ReadAndFillBuffer(&buffer);
			// char Number
			Vx2DVector cn;
			chunk->ReadAndFillBuffer_LEndian(&cn);
			// Font Zone
			VxRect fz;
			chunk->ReadAndFillBuffer_LEndian(&fz);
			// Texture ID
			CK_ID id = chunk->ReadObjectID();
			CKObject *obj = CKGetObject(id);
			if (!CKIsChildClassOf(obj, CKCID_TEXTURE))
				id = 0;

			// Spacing
			int spacing = chunk->ReadInt();
			// First character
			int firstCharacter = chunk->ReadInt();

			int index = GetFontIndex(buffer);
			if (!index && id) // Font does not already exist and the texture is present
			{
				// New font
				CKTextureFont *font = new CKTextureFont(this, m_Context, buffer);
				font->m_FontZone = fz;
				font->m_CharNumber = cn;

				if (version == 1)
				{
					if (spacing == 1)
						font->m_SpacingProperties = CKTextureFont::FIXED;
					else
						font->m_SpacingProperties = 0;
				}
				else // version2
				{
					font->m_SpacingProperties = spacing;

					if (spacing & CKTextureFont::SPACINGTOBESAVED)
					{ // We have saved spacing data too
						chunk->ReadAndFillBuffer_LEndian(256 * sizeof(CharacterTextureCoordinates), (&font->m_FontCoordinates));
						// the font is marked as created
						font->m_SpacingProperties |= CKTextureFont::CREATED;
					}
				}
				font->m_FontTexture = id;
				font->m_FirstCharacter = firstCharacter;

				// We add the font to the existing one
				index = RegisterFont(font);
				m_TextureLoaded = TRUE;
			}
			else // if version 2, we need to skip the spacing if they were saved
			{
				if ((version == 2) && (spacing & CKTextureFont::SPACINGTOBESAVED)) // We have saved spacing data too
				{
					chunk->Skip(256 * sizeof(CharacterTextureCoordinates) / 4);
				}
			}
			// Remapping [oldIndex] = newIndex
			ConversionTable[h + 1] = index;
		}

		///
		// Remapping
		LoadedFile->RemapManagerInt(FONT_MANAGER_GUID, ConversionTable, fn + 1);

		delete[] ConversionTable;
	}

	return CK_OK;
}

CKERROR CKFontManager::PostLoad()
{
	if (m_TextureLoaded)
	{
		// We now create the font that were only prepared by the loading
		for (CKTextureFont **it = m_FontArray.Begin(); it != m_FontArray.End(); it++)
		{
			CKTextureFont *font = *it;
			font->CreateFromTexture();
		}
		m_TextureLoaded = FALSE;
	}
	return CK_OK;
}

CompiledTextData *CKFontManager::AddCompiledText(CK_ID id)
{
	CompiledTextData *ctdata = new CompiledTextData;
	m_CompiledText.Insert(id, ctdata, TRUE);
	return ctdata;
}

void CKFontManager::RemoveCompiledText(CK_ID id)
{
	CompiledTextData *ctdata;
	if (m_CompiledText.LookUp(id, ctdata))
	{
		delete ctdata;
		m_CompiledText.Remove(id);
	}
}

CompiledTextData *CKFontManager::GetCompiledText(CK_ID id)
{
	CompiledTextData *ctdata = NULL;
	m_CompiledText.LookUp(id, ctdata);
	return ctdata;
}

void CKFontManager::DrawRectangle(const RectangleData &rd)
{
	m_Rectangles.PushBack(rd);
}

/*************************************************
Name: DrawText

Summary: Store text data in manager : used by the texts block with multiple setting checked

Arguments: entity : ID of the targeted frame (2D/3D)
		 fontIndex : index of the font to use
		 string : text to store
		 scale, endColor, align, textZone, mat, textFlags : parameter associated with data : see 2D/3D Text blocks

Return Value: none

Remarks: the data are stored in m_Texts, a reference is added to m_2DTexts or m_3DTexts
		they are used when we retrieve the data for an entity, so we don't scan all the data
		in case we use 2D and 3D texts blocks

See also:

*************************************************/
void CKFontManager::DrawText(CK_ID entity, int fontIndex, CKSTRING string, Vx2DVector scale, CKDWORD startColor, CKDWORD endColor, int align, VxRect textZone, CKMaterial *mat, int textFlags)
{
	// No text : do nothing
	if (!string)
		return;

	// If we already have a TextData struct for this entity, re-use it
	TextData *td = NULL;

	if (m_Texts.LookUp(entity, td))
	{
		// we don't use COMPILED setting
		if (!(textFlags & TEXT_COMPILED))
		{
			// overwrite old values
			td->m_FontIndex = fontIndex;
			CKDeletePointer(td->m_String); // The string length can change, don't overwrite directly
			td->m_String = CKStrdup(string);
			td->m_Scale = scale;
			td->m_StartColor = startColor;
			td->m_EndColor = endColor;
			td->m_Align = align;
			td->m_TextZone = textZone;
			td->m_Mat = mat;
		}
		// for now, if COMPILED setting is used, we don't change the text data content
		// changes goes here
	}
	else
	{
		// No TextData for this entity, create a new structure and add it to the list
		td = new TextData(entity, fontIndex, string, scale, startColor, endColor, align, textZone, mat, textFlags);
		m_Texts.Insert(entity, td, TRUE);
	}

	CKTextureFont *fn = GetFont(fontIndex);
	if (td && fn)
	{
		td->m_Margins = fn->m_Margins;
		td->m_Offset = fn->m_Offset;
		td->m_ParagraphIndentation = fn->m_ParagraphIndentation;
		td->m_CaretMaterial = fn->m_CaretMaterial;
		td->m_CaretSize = fn->m_CaretSize;
		td->m_ClippingRect = fn->m_ClippingRect;
	}

	// Store the reference in the good table, these array are cleared when render are done,
	// so we can add the data without check
	if (textFlags & TEXT_3D)
		m_3DTexts.PushBack(td);
	else
		m_2DTexts.PushBack(td);
}

void CKFontManager::DrawText(CKRenderContext *iRC, int iFontIndex, const char *iText, const Vx2DVector &iPosition, const Vx2DVector &iScale, CKDWORD iStartColor, CKDWORD iEndColor)
{
	CKTextureFont *font = NULL;
	if (iFontIndex)
	{
		font = GetFont(iFontIndex);
	}

	if (!font)
		return;

	font->m_Scale = iScale;
	font->m_StartColor = iStartColor;
	font->m_EndColor = iEndColor;
	font->m_Leading.x = -3.0f;
	font->m_Properties |= FONT_DISABLEFILTER;

	// draw the text
	font->DrawStringEx(iRC, iText, VxRect(iPosition.x, iPosition.y, (float)iRC->GetWidth(), (float)iRC->GetHeight()), 0);
}

void CKFontManager::DrawRectanglesCallback(CKRenderContext *dev)
{
	if (!m_Rectangles.Size())
		return;

	VxRect oldclip;
	dev->GetViewRect(oldclip);
	VxRect clip;
	dev->GetWindowRect(clip);
	clip.Translate(-clip.GetTopLeft());
	dev->SetViewRect(clip);

	// We draw all the rectangles
	for (int i = 0; i < m_Rectangles.Size(); ++i)
	{
		float size = m_Rectangles[i].m_BorderSize;
		if (size == 0.0f) // No Border
		{
			DrawFillRectangle(dev, m_Rectangles[i].m_InColor, m_Rectangles[i].m_Material, m_Rectangles[i].m_Screen, m_Rectangles[i].m_InUVs);
		}
		else
		{
			if (size < 0.0f) // Border
			{
				DrawBorderRectangle(dev, m_Rectangles[i].m_BorderColor, m_Rectangles[i].m_Material, m_Rectangles[i].m_Screen, -size);
			}
			else // Interior + Border
			{
				VxRect rect = m_Rectangles[i].m_Screen;
				rect.Inflate(Vx2DVector(-size, -size));
				DrawFillRectangle(dev, m_Rectangles[i].m_InColor, m_Rectangles[i].m_Material, rect, m_Rectangles[i].m_InUVs);
				DrawBorderRectangle(dev, m_Rectangles[i].m_BorderColor, m_Rectangles[i].m_Material, m_Rectangles[i].m_Screen, size);
			}
		}
	}

	dev->SetViewRect(oldclip);
}

/*************************************************
Name: parent2DHidden, parent3DHidden

Summary: look the parent of an entity to verify it's visibility

Arguments: ent : entity to check for visibility

Return Value: TRUE if a parent is hidden else FALSE

Remarks: none

See also: DrawTextCallback

*************************************************/
inline CKBOOL parent2DHidden(CK2dEntity *ent)
{
	while (ent)
	{
		if (ent->IsHierarchicallyHide())
			return TRUE;
		ent = ent->GetParent();
	}
	return FALSE;
}

inline CKBOOL parent3DHidden(CK3dEntity *ent)
{
	while (ent)
	{
		if (ent->IsHierarchicallyHide())
			return TRUE;
		ent = ent->GetParent();
	}
	return FALSE;
}

/*************************************************
Name: DrawTextCallback

Summary: Retrieve the text data stored by text blocks with
				multiple setting checked and send it to render functions

Arguments:	dev : current render context
				 arg : a pointer to the CKFontManager

Return Value: none

Remarks: arg must not be NULL and point to the good manager !

See also:

*************************************************/
void CKFontManager::DrawTextCallback2D(CKRenderContext *dev, void *arg)
{
	CKFontManager *fm = (CKFontManager *)arg;

	CK2dEntity *ent1, *ent2;
	CKBeObject *obj;
    TextData *td;

	int i, limit = fm->m_2DTexts.Size();
	// Sort by z-order, bubble sort
	for (i = 0; i < limit - 1; i++)
	{
		for (int j = i + 1; j < limit; j++)
		{
			ent1 = (CK2dEntity *)fm->m_Context->GetObject(fm->m_2DTexts[i]->m_Entity);
			if (ent1)
			{
				ent2 = (CK2dEntity *)fm->m_Context->GetObject(fm->m_2DTexts[j]->m_Entity);
				if (ent2)
				{
					// Swap data if not in the good order
					if (ent1->GetZOrder() < ent2->GetZOrder())
					{
						memcpy(&td, &(fm->m_2DTexts[i]), sizeof(struct TextData *));
						memcpy(&(fm->m_2DTexts[i]), &(fm->m_2DTexts[j]), sizeof(struct TextData *));
						memcpy(&(fm->m_2DTexts[j]), &td, sizeof(struct TextData *));
					}
				}
			}
		}
	}

	// Draw pending 2D texts
	for (i = 0; i < limit; i++)
	{
        td = fm->m_2DTexts[i];
		obj = (CKBeObject *)fm->m_Context->GetObject(td->m_Entity);
		if (obj)
		{
			// If the entity is hidden don't draw
			if (parent2DHidden((CK2dEntity *)obj))
				continue;
			// Restore entity parameters in texture font
			CKTextureFont *font = fm->GetFont(td->m_FontIndex);

			font->m_Scale = td->m_Scale;
			font->m_StartColor = td->m_StartColor;
			font->m_EndColor = td->m_EndColor;

			font->m_Margins = td->m_Margins;
			font->m_Offset = td->m_Offset;
			font->m_ParagraphIndentation = td->m_ParagraphIndentation;
			font->m_CaretMaterial = td->m_CaretMaterial;
			font->m_CaretSize = td->m_CaretSize;
			font->m_ClippingRect = td->m_ClippingRect;

			// draw the text
			font->DrawCKText(dev, obj, td->m_String, td->m_Align, td->m_TextZone, td->m_Mat, td->m_TextFlags, TRUE);
		}
	}
}

void CKFontManager::DrawTextCallback3D(CKRenderContext *dev, void *arg)
{
	CKFontManager *fm = (CKFontManager *)arg;

	CKBeObject *obj;
    TextData *td;

	CK3dEntity *ent3, *ent4;
	VxVector viewpos;
	CK3dEntity *viewpoint = dev->GetViewpoint();
	viewpoint->GetPosition(&viewpos);

	int limit = fm->m_3DTexts.Size();
	int i;
	// Sort by z-order
	for (i = 0; i < limit - 1; i++)
	{
		for (int j = i + 1; j < limit; j++)
		{
			ent3 = (CK3dEntity *)fm->m_Context->GetObject(fm->m_3DTexts[i]->m_Entity);
			if (ent3)
			{
				ent4 = (CK3dEntity *)fm->m_Context->GetObject(fm->m_3DTexts[j]->m_Entity);
				if (ent4)
				{
					// Retrieve position
					VxVector v1, v2, pos1, pos2;
					ent3->GetPosition(&pos1);
					viewpoint->InverseTransform(&v1, &pos1);
					ent4->GetPosition(&pos2);
					viewpoint->InverseTransform(&v2, &pos2);
					// Swap values if not in the good order
					if (v1.z < v2.z)
					{
						memcpy(&td, &(fm->m_3DTexts[i]), sizeof(struct TextData *));
						memcpy(&(fm->m_3DTexts[i]), &(fm->m_3DTexts[j]), sizeof(struct TextData *));
						memcpy(&(fm->m_3DTexts[j]), &td, sizeof(struct TextData *));
					}
				}
			}
		}
	}

	// Draw pending 3D texts
	for (i = 0; i < limit; i++)
	{
		td = fm->m_3DTexts[i];
		obj = (CKBeObject *)fm->m_Context->GetObject(td->m_Entity);
		if (obj)
		{
			ent3 = (CK3dEntity *)obj;
			if (!ent3->IsAllOutsideFrustrum())
			{
				// If the entity is hidden don't draw the text
				if (parent3DHidden(ent3))
					continue;

				///
				// We set the transformation to the 3D Entity (slightly transformed)
				const VxMatrix &mat = ent3->GetWorldMatrix();
				// we invert the Y of the matrix, so the text is written in a good way
				VxVector entScale;
				ent3->GetScale(&entScale, FALSE);
				VxMatrix resMat = mat;
                resMat[0] = (mat[0] / entScale.x);
                resMat[1] = -(mat[1] / entScale.y);
                resMat[2] = -(mat[2] / entScale.z);
				dev->SetWorldTransformationMatrix(resMat);

				// Restore the entity parameter in the font texture
				CKTextureFont *font = fm->GetFont(td->m_FontIndex);
				font->m_Scale = td->m_Scale;
				font->m_StartColor = td->m_StartColor;
				font->m_EndColor = td->m_EndColor;

				font->m_Margins = td->m_Margins;
				font->m_Offset = td->m_Offset;
				font->m_ParagraphIndentation = td->m_ParagraphIndentation;
				font->m_CaretMaterial = td->m_CaretMaterial;
				font->m_CaretSize = td->m_CaretSize;
				font->m_ClippingRect = td->m_ClippingRect;
				// Draw the text
				font->DrawCKText(dev, obj, td->m_String, td->m_Align, td->m_TextZone, td->m_Mat, td->m_TextFlags, TRUE);

				// Set the bounding box for clipping
				VxBbox bbox;
				bbox.Min.Set(font->m_TextExtents.left, -font->m_TextExtents.bottom, -0.01f);
				bbox.Max.Set(font->m_TextExtents.right, -font->m_TextExtents.top, 0.01f);
				// ent3->SetBoundingBox(&bbox,TRUE);
			}
		}
	}
}

/***********************************************************************/
/***********************************************************************/
//                 System font functions
/***********************************************************************/
/***********************************************************************/

/*************************************************
Name: DeleteFont

Summary: Delete a specific system font from the manager

Arguments: the font name

Return Value:

Remarks: Release the GDI handle associated with this font
Delete unused fonts free up system resources

See also:

*************************************************/
void CKFontManager::DeleteFontHandle(CKSTRING fontName)
{
#ifndef FONTMANAGER_NOSYSFONT
	// We localize the font
	FontIterator it = m_Fonts.Find(XString(fontName));
	// And we release the handle
	if (it != m_Fonts.End())
	{
		::DeleteObject(*it);
		// Remove entry
		m_Fonts.Remove(it);
	}
#endif
}

/*************************************************
Name: CreateFont

Summary: Create a new logical font having its name and properties

Arguments: fontName - the name used to retrieve the font in the manager
				  sysFontIndex - a system font index from enumeration  (CKPGUID_FONTNAME)
				  weight - the weight of the font (a value between 100 and 900 (step by 100)) use the enumeration
							   'Font Weight' CKPGUID_FONTWEIGHT
				  italic - TRUE if we want an italic font
				  underline - TRUE if we want an underlined font
				  resolution - determine the size of the font (CKPGUID_FONTRESOLUTION enumeration)
									use 1 --> 128x128 texture
										 2 --> 256x256
										 4 --> 512x512
										 5 --> 1024x1024
										other value --> 512x512

Return Value: TRUE if success

Remarks:

See also:

*************************************************/

CKBOOL CKFontManager::CreateFont(CKSTRING fontName, int sysFontIndex, int weight, CKBOOL italic, CKBOOL underline, int resolution, int iFontSize)
{
#ifndef FONTMANAGER_NOSYSFONT
	// No font name : do nothing
	if (!fontName)
		return FALSE;

	CKParameterManager *pm = m_Context->GetParameterManager();
	if (!pm)
		return FALSE;
	CKEnumStruct *data = pm->GetEnumDescByType(pm->ParameterGuidToType(CKPGUID_FONTNAME));
	if ((sysFontIndex < 0) || (sysFontIndex > data->NbData))
		return FALSE;

	// We check if the fontName is present
	FontIterator it = m_Fonts.Find(XString(fontName));
	if (it == m_Fonts.End()) // We create it
	{
		// We select the font size according to the desired texture resolution
		int fontSize = FONTSIZE512;
		if (!iFontSize)
		{
			switch (resolution)
			{
			case 1: // 128x128
				fontSize = FONTSIZE128;
				break;
			case 2: // 256x256
				fontSize = FONTSIZE256;
				break;
			case 4: // 512x512
				fontSize = FONTSIZE512;
				break;
			case 8: // 1024x1024
				fontSize = FONTSIZE1024;
				break;
			default:
				break;
			}
		}
		else
		{
			fontSize = iFontSize;
		}

		// We create the logical font and retrieves a font handle
		FONTHANDLE fontHandle = (FONTHANDLE)VxCreateFont(data->Desc[sysFontIndex], fontSize, weight, italic, underline);

		// If the font can't be created
		if (fontHandle == NULL)
			return FALSE;

		// We add the new font to the list
		m_Fonts.Insert(XString(fontName), fontHandle);
	}

	return TRUE;
#else
	return FALSE;
#endif
}

/*************************************************
Name: GetOutlineFontMetric

Summary: Retrieve the outline font metric for the currently selected

Arguments: The name of the font in the manager and a FONT_OUTLINEMETRIC struct
					or a FONT_METRIC struct to fill

Return Value: TRUE if success

Remarks: See the Win32 documentation for the content of OUTLINETEXTMETRIC or TEXTMETRIC
				structs

See also:

*************************************************/
CKBOOL CKFontManager::GetOutlineFontMetric(FONT_OUTLINEMETRIC &metric)
{
#ifndef FONTMANAGER_NOSYSFONT
	return ::GetOutlineTextMetrics(m_DC, sizeof(FONT_OUTLINEMETRIC), &metric) != 0;
#else
	return FALSE;
#endif
}

CKBOOL CKFontManager::GetFontMetrics(FONT_METRIC &metric)
{
#ifndef FONTMANAGER_NOSYSFONT
	return ::GetTextMetrics(m_DC, &metric);
#else
	return FALSE;
#endif
}

/*************************************************
Name: IsTrueTypeFont

Summary: test if the currently selected font is a true type font

Arguments: none

Return Value: TRUE if it's a true type

Remarks:

See also:

*************************************************/
CKBOOL CKFontManager::IsTrueTypeFont()
{
#ifndef FONTMANAGER_NOSYSFONT
	TEXTMETRIC metric;
	if (::GetTextMetrics(m_DC, &metric))
		return (metric.tmPitchAndFamily & TMPF_TRUETYPE);
	return FALSE;
#else
	return FALSE;
#endif
}

/*************************************************
Name: SelectFont

Summary: Select the current font

Arguments: fontName - the font name in the manager

Return Value: TRUE if success

Remarks: must be call before use of get*fontMetrics, getchar*width

See also:

*************************************************/
CKBOOL CKFontManager::SelectFont(CKSTRING fontName)
{
#ifndef FONTMANAGER_NOSYSFONT
	// We try to localize the font
	FontIterator it = m_Fonts.Find(XString(fontName));
	if (it == m_Fonts.End())
		return FALSE;

	// If it is present, we select it in the DC and retrieves the information
	return (::SelectObject(m_DC, *it) != NULL);
#else
	return FALSE;
#endif
}

/*************************************************
Name: GetCharABCWidth

Summary: Retrieves the width of a range of char for the current selected font

Arguments:  startChar/endChar - define the character range to use (inclusive range)
				  ABCWidth - an array of FONT_ABC structs to store the results

Return Value: TRUE if success

Remarks: the FONT_ABC array must be large enough to store the result

See also:

*************************************************/
CKBOOL CKFontManager::GetCharABCWidths(CKBYTE startChar, CKBYTE endChar, FONT_ABC *ABCWidths)
{
#ifndef FONTMANAGER_NOSYSFONT
	return ::GetCharABCWidths(m_DC, startChar, endChar, ABCWidths);
#else
	return FALSE;
#endif
}

/*************************************************
Name: GetCharWidth

Summary: Retrieves the width of a range of char for non true-type font currently selected

Arguments:  startChar/endChar - define the character range to use (inclusive range)
				  Widths - an array of int to store the results

Return Value: TRUE if success

Remarks: the widths array must be large enough to store the result

See also:

*************************************************/
CKBOOL CKFontManager::GetCharWidths(CKBYTE startChar, CKBYTE endChar, int *widths)
{
#ifndef FONTMANAGER_NOSYSFONT
	return ::GetCharWidth32(m_DC, startChar, endChar, widths);
#else
	return FALSE;
#endif
}

/*************************************************
Name: CreateTextureFromFont

Summary: Create a CKTexture representing the characters of the designed font

Arguments: sysFontIndex - the index of the font in enumeration CKPGUID_FONTNAME
				 resolution - the size of the texture (CKPGUID_FONTRESOLUTION enumeration)
									use 1 --> 128x128 texture
										 2 --> 256x256
										 4 --> 512x512
										 8 --> 1024x1024
										other value --> 512x512
				 extended - if false, we create a half height texture with only the first 128 ASCII chars
								  else, we have a full size texture with all the chars
				  renderControls - if false, we don't draw the chars with ASCII code < 32
											(i.e. control characters are all blank)

					dynamic - if true, the texture created is a dynamical object

Return Value: the texture if sucess NULL otherwise

Remarks:

See also:

*************************************************/
CKTexture *CKFontManager::CreateTextureFromFont(int sysFontIndex, int resolution, CKBOOL extended, int bold, CKBOOL italic, CKBOOL underline, CKBOOL renderControls, CKBOOL dynamic, int iForcedSize)
{
#ifndef FONTMANAGER_NOSYSFONT
	CKParameterManager *pm = m_Context->GetParameterManager();
	if (!pm)
		return NULL;

	CKEnumStruct *data = pm->GetEnumDescByType(pm->ParameterGuidToType(CKPGUID_FONTNAME));
	if ((sysFontIndex < 0) || (sysFontIndex > data->NbData))
		return FALSE;

	// Get the font name by index
	CKSTRING fontName = data->Desc[sysFontIndex];

	// we compose the texture name
	XString textureName("FONT_");
	textureName << fontName;
	switch (resolution)
	{
	case 1:
		textureName << "_128x128";
		break;
	case 2:
		textureName << "_256x256";
		break;
	case 4:
		textureName << "_512x512";
		break;
	case 8:
		textureName << "_1024x1024";
		break;
	}

	if (iForcedSize)
	{
		textureName << "(" << iForcedSize << ")";
	}

	if (extended)
		textureName << "_E";
	if (renderControls)
		textureName << "_C";
	textureName << "_";
	switch (bold)
	{
	case 100:
		textureName << "THIN";
		break;
	case 200:
		textureName << "XLIGHT";
		break;
	case 300:
		textureName << "LIGHT";
		break;
	case 400:
		textureName << "NORMAL";
		break;
	case 500:
		textureName << "MEDIUM";
		break;
	case 600:
		textureName << "DBOLD";
		break;
	case 700:
		textureName << "BOLD";
		break;
	case 800:
		textureName << "XBOLD";
		break;
	case 900:
		textureName << "HEAVY";
		break;
	}

	if (italic)
		textureName << "_I";
	if (underline)
		textureName << "_U";

	CKTexture *texture = (CKTexture *)m_Context->GetObjectByNameAndClass(textureName.Str(), CKCID_TEXTURE);
	if (!texture)
	{
		// We try to localize the font
		FontIterator it = m_Fonts.Find(textureName);
		if (it == m_Fonts.End())
		{
			if (!CreateFont(textureName.Str(), sysFontIndex, bold, italic, underline, resolution, iForcedSize))
				return NULL;
			// exists this time because we just create it !
			it = m_Fonts.Find(textureName);
		}

		// CKTexture object creation
		if (dynamic)
			texture = (CKTexture *)m_Context->CreateObject(CKCID_TEXTURE, textureName.Str(), CK_OBJECTCREATION_DYNAMIC);
		else
			texture = (CKTexture *)m_Context->CreateObject(CKCID_TEXTURE, textureName.Str());
		// If we couldn't create it
		if (!texture)
			return NULL;

		int nbLine = 8, nbCol = 16;
		int texHeight = 64, texWidth = 128;

		// If we draw all 256 chars
		if (extended)
		{
			nbLine = 16;
			texHeight *= 2;
		}

		// Texture size from desired resolution
		texWidth *= resolution;
		texHeight *= resolution;

		int nbPixels = texHeight * texWidth, charCellSize = texWidth / nbCol;
		// We create the surface
		texture->Create(texWidth, texHeight, 32);

		// We get the texture description
		VxImageDescEx vxTexDesc;
		texture->GetSystemTextureDesc(vxTexDesc);

		vxTexDesc.Image = texture->LockSurfacePtr();

		if (vxTexDesc.Image)
		{
			// We clear the texture surface (white for the color and black for the alpha)
			CKDWORD colorDef = RGBAITOCOLOR(0xff, 0xff, 0xff, 0x00);

			CKBYTE *AlphaDest = (CKBYTE *)vxTexDesc.Image + 3;

			VxFillStructure(nbPixels, (CKBYTE *)vxTexDesc.Image, 4, 4, &colorDef);

			// We create a system memory bitmap for font rendering
			BITMAP_HANDLE hBitmap = VxCreateBitmap(vxTexDesc);
			// If it exists we get the handle
			HFONT hFont = *it;
			// We select the GDI objects
			HFONT oldFont = (HFONT)::SelectObject(m_DC, hFont);
			HBITMAP oldBitmap = (HBITMAP)::SelectObject(m_DC, (HBITMAP)hBitmap);

			// Set colors
			RECT r;
			SetRect(&r, 0, 0, texWidth, texHeight);
			::FillRect(m_DC, &r, (HBRUSH)::GetStockObject(BLACK_BRUSH));
			::SetTextColor(m_DC, RGBAITOCOLOR(255, 255, 255, 0));
			::SetBkColor(m_DC, RGBAITOCOLOR(0, 0, 0, 0));

			// Rendering rectangle
			RECT rect;
			rect.top = 0;
			rect.left = 0;
			rect.bottom = charCellSize;
			rect.right = charCellSize;

			// For each character, we draw it in the bitmap at the good position
			int i, j;
			char c;

			// Render a grid

			HPEN hp = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			HPEN bp = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

			for (i = 0; i < nbLine; i++)
			{
				for (j = 0; j < nbCol; j++)
				{
					c = i * nbCol + j;
					if (renderControls || ((unsigned char)c >= 32))
						::DrawText(m_DC, &c, 1, &rect, DT_LEFT | DT_TOP | DT_NOPREFIX);
					rect.left += charCellSize;
					rect.right += charCellSize;
				}
				rect.left = 0;
				rect.right = charCellSize;
				rect.top += charCellSize;
				rect.bottom += charCellSize;
			}

			for (i = 0; i < nbLine; i++)
			{
				(HPEN)::SelectObject(m_DC, bp);
				::MoveToEx(m_DC, i * charCellSize, 0, NULL);
				::LineTo(m_DC, i * charCellSize, texHeight);
				(HPEN)::SelectObject(m_DC, bp);
				::MoveToEx(m_DC, i * charCellSize - 1, 0, NULL);
				::LineTo(m_DC, i * charCellSize - 1, texHeight);

				(HPEN)::SelectObject(m_DC, bp);
				::MoveToEx(m_DC, 0, i * charCellSize, NULL);
				::LineTo(m_DC, texWidth, i * charCellSize);
				(HPEN)::SelectObject(m_DC, bp);
				::MoveToEx(m_DC, 0, i * charCellSize - 1, NULL);
				::LineTo(m_DC, texWidth, i * charCellSize - 1);
			}

			// For each pixel of the bitmap, we reduce to a grayscale color
			// and fill an alpha table
			BITMAP dib;
			GetObject(hBitmap, sizeof(BITMAP), &dib);
			CKDWORD Bpp = dib.bmBitsPixel >> 3;
			CKDWORD Pitch = dib.bmWidthBytes;
			CKBYTE *Colors = (CKBYTE *)dib.bmBits + (dib.bmHeight - 1) * dib.bmWidthBytes;

			CKDWORD RGBIntensity[3][256];
			for (i = 0; i < 256; ++i)
			{
				RGBIntensity[0][i] = (unsigned long)(0.299f * (float)i);
				RGBIntensity[1][i] = (unsigned long)(0.587f * (float)i);
				RGBIntensity[2][i] = (unsigned long)(0.114f * (float)i);
			}
			if (!Colors)
			{
				for (i = 0; i < texHeight; i++)
					for (j = 0; j < texWidth; j++, AlphaDest += 4)
					{
						COLORREF currentColor = ::GetPixel(m_DC, j, i);
						*AlphaDest = 0xFF & (unsigned char)(RGBIntensity[0][GetRValue(currentColor)] + RGBIntensity[1][GetGValue(currentColor)] + RGBIntensity[2][GetBValue(currentColor)]);
						// ENDIANSWAP32(*AlphaDest);
					}
			}
			else
			{
				for (i = 0; i < texHeight; i++, Colors -= Pitch)
				{
					CKBYTE *col = Colors;
					for (j = 0; j < texWidth; j++, AlphaDest += 4, col += Bpp)
					{
						*AlphaDest = 0xff & (unsigned char)(RGBIntensity[2][col[0]] + RGBIntensity[1][col[1]] + RGBIntensity[0][col[2]]);
					}
				}
			}

			// We deselect and delete the GDI objects
			::SelectObject(m_DC, oldBitmap);
			::SelectObject(m_DC, oldFont);
			VxDeleteBitmap(hBitmap);

			texture->ReleaseSurfacePtr();
		}
		// Some texture settings
		texture->SetUserMipMapMode(TRUE);
		texture->UseMipmap(-1);

		// Wie add the new texture to the current level
		m_Context->GetCurrentLevel()->AddObject(texture);
	}

	return texture;
#else
	return NULL;
#endif
}

/*************************************************
Name: FontEnumeratorCallBack

Summary: Add a new font name to the array font name list

Arguments: font info and the array to fill (in the param field)

Return Value: TRUE in any way

Remarks: Called automatically by the system

See also: RegenerateFontEnumeration

*************************************************/
#ifndef FONTMANAGER_NOSYSFONT
CKBOOL CALLBACK FontEnumeratorCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID param)
{
	// Retrieves the parameters
	XClassArray<XString> *array = (XClassArray<XString> *)param;
	array->PushBack(XString(lplf->lfFaceName));

	return TRUE;
}
#endif

/*************************************************
Name: RegenerateFontEnumeration

Summary: Fill an enumeration CKPGUID_FONTNAME with the list of system font names found

Arguments: -

Return Value: FALSE if fails, in this case the enum is set to empty

See also: -

*************************************************/
CKBOOL CKFontManager::RegenerateFontEnumeration()
{
#ifndef FONTMANAGER_NOSYSFONT
	CKParameterManager *pm = m_Context->GetParameterManager();
	if (!pm)
		return FALSE;

    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfFaceName[0] = '\0';
    lf.lfCharSet = ANSI_CHARSET;

    XClassArray<XString> array;
    array.Reserve(64);

    // Fill the array (automatic call to FontEnumeratorCallback for each font found)
    if (!EnumFontFamiliesEx(m_DC, &lf, (FONTENUMPROC)FontEnumeratorCallBack, (LPARAM)&array, 0))
	{
		pm->ChangeEnumDeclaration(CKPGUID_FONTNAME, "");
		return FALSE;
	}
	// Presents font in lexical order
	array.Sort();

	// Generate the new enum
	XString newEnum;
	char buffer[64];
	int i = 0;
	for (XString *it = array.Begin(); it != array.End(); it++)
	{
		if (i++ != 0)
			newEnum << ",";
		newEnum << (*it);
		sprintf(buffer, "=%d", i - 1);
		newEnum << buffer;
	}
	pm->ChangeEnumDeclaration(CKPGUID_FONTNAME, newEnum.Str());

	return TRUE;
#else
	return FALSE;
#endif
}