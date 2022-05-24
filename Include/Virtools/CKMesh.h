/*************************************************************************/
/*	File : CKMesh.h														 */
/*	Author :  Romain Sididris											 */	
/*																		 */	
/*	Virtools SDK 														 */	 
/*	Copyright (c) Virtools 2000, All Rights Reserved.					 */	
/*************************************************************************/
#ifndef CKMESH_H
#define CKMESH_H "$Id:$"

#include "CKBeObject.h"
#include "VxDefines.h"


#define CKCHANNELMASK(Channel) (1<<Channel)

/**************************************************************************
{filename:CKMesh}
Name: CKMesh

Summary: Representation of the geometry of a 3D object

Remarks:
	+ The CKMesh class is the class describing geometrical representation of an object.
It can be a mesh made up of faces or lines. Description of a mesh includes
vertices (Position,color,texture coordinates,normal),faces or lines 

	+ A CKMesh can have up to 8 additionals material channels that is textures that 
are blended together on the mesh. Material Channels can be used to produce lightmaps for example.

	+ A CKMesh can have callbacks attached to it. A PreRender or PostRender Callback can be set to
do specific operations before or after the rendering of the mesh is done (A mesh could be rendered several
times in a single frame if more than one CK3dEntity use it !). A render callback can be set
to replace the default rendering.

	+ Its class id is CKCID_MESH




See also: Using Meshes,CK3dEntity
*****************************************************************************/
class CKMesh:public CKBeObject {
public :
//------------------------------------------------------------------------
// Flags

/*************************************************
Summary: Checks whether the mesh is considered transparent

Return Value: 
	Returns TRUE if the mesh uses a transparent material, FALSE otherwise.
Remarks:
	+ The method returns TRUE if the mesh uses a transparent material or its 
	transparency is forced using SetTransparent.
See Also: SetTransparent,CKMaterial
*************************************************/
virtual CKBOOL	IsTransparent() = 0;

/*************************************************
Summary:Forces the mesh to be considered transparent.

Arguments: 
	Transparency: TRUE to force or FALSE otherwise
Remarks:
	+ A mesh that does not contain any transparent materials can be
	forced to be considered transparent by using this method. This way 
	the entities that use this mesh will be rendered after the opaque
	objects for correct results.
	+ This method sets or removes the VXMESH_FORCETRANSPARENCY flag.
See Also: IsTransparent,GetFlags
*************************************************/
virtual void	SetTransparent(CKBOOL Transparency) = 0;

/*************************************************
Summary: Sets the mesh texture wrapping mode.

Arguments: 
	Mode: wrapping mode to be set.
Remarks:
	+ This methods sets or removes the VXMESH_WRAPU and VXMESH_WRAPV
See Also: GetWrapMode,,GetFlags,VXTEXTURE_WRAPMODE
*************************************************/
virtual void SetWrapMode(VXTEXTURE_WRAPMODE Mode) = 0;

/*************************************************
Summary: Returns the mesh texture wrapping mode.

Return Value:
	Texture wrapping mode of the mesh.
See Also: SetWrapMode,VXTEXTURE_WRAPMODE
*************************************************/
virtual VXTEXTURE_WRAPMODE	GetWrapMode() = 0;

/*************************************************
Summary: Sets the mesh lighting mode.

Arguments:
	 Mode: Lighting mode of the mesh.
Remarks:	
	+ A mesh can lit using its normals or the colors of each vertex
	can be given.
	+ This method sets or removes the VXMESH_PRELITMODE flag.
See Also: GetLitMode,,GetFlags,VXMESH_LITMODE
*************************************************/
virtual void	SetLitMode(VXMESH_LITMODE Mode) = 0;

/*************************************************
Summary: Returns the mesh Lighting mode.

Return Value:
	 Lighting mode of the mesh.
See Also: SetLitMode, VXMESH_LITMODE
*************************************************/
virtual VXMESH_LITMODE 	GetLitMode() = 0;

/*************************************************
Summary: Returns the Mesh flags.

Return Value:
	 Mesh flags (VXMESH_FLAGS).
Remarks:
	+ Most of the flags can be set or asked with the appropriate method of CKMesh,
	see the VXMESH_FLAGS documentation.
See Also: SetFlags,VXMESH_FLAGS
*************************************************/
virtual	CKDWORD	GetFlags() = 0;

/*************************************************
Summary: Sets the Mesh flags.

Arguments:
	Flags: Mesh flags to be set (VXMESH_FLAGS).
Remarks:	
	+ Most of the flags can be set or asked with the appropriate method of CKMesh,
	see the VXMESH_FLAGS documentation.
See Also: GetFlags,VXMESH_FLAGS
*************************************************/
virtual	void	SetFlags(CKDWORD Flags) = 0;

/*************************************************
Summary: Returns a pointer to the vertex positions.

Arguments:
	Stride: Size in byte between each position in the returned buffer.
	iIndex: Index of the vertex to set or get.
	oPosition: Position to set. 
Return Value:
	A pointer to the vertices list
Remarks:
	+ Use this method along with GetModifierVertexCount rather than using 
	GetPositionsPtr to move the vertices so that you code will work
	on any type of mesh (CKMesh and CKPatchMesh for example).
	+ For a pure CKMesh this methods is the same than GetPositionsPtr.
Example:
	// Translates all the vertices of a mesh by a given translation vector
	
	VxVector Translation(10.0f,0,0);
	int VCount = mesh->GetModifierVertexCount();
	CKDWORD Stride;
	BYTE* VPtr = mesh->GetModifierVertices(&Stride);

	if (VPtr) {
		for (int i = 0; i < VCount; ++i ,VPtr+=Stride) {
			(*( (VxVector*) VPtr )) += Translation;
		}
	}
	// Since vertices was only translated there is no need to 
	// rebuild the normals, we only notify the mesh that the 
	// the vertices moved.
	mesh->ModifierVertexMove(FALSE,FALSE);

See Also: GetModifierUVs, ModifierVertexMove, GetPositionsPtr 
*************************************************/
virtual BYTE*	GetModifierVertices(CKDWORD* Stride) = 0;

virtual int		GetModifierVertexCount() = 0;

/****************************************************
Summary: Notifies when vertices in the mesh have been moved.

Arguments:
	RebuildNormals: TRUE if vertex normals have to be re-calculated,
					FALSE otherwise.
	RebuildFaceNormals: TRUE if face normals have to be re-calculated,
					FALSE otherwise.
See Also: GetModifierVertices,ModifierUVMove,VertexMove
*************************************************/
virtual void	ModifierVertexMove (CKBOOL RebuildNormals,CKBOOL RebuildFaceNormals) = 0;    

//---------- Use these fonctions if you intend to modify vertices texture coordinates
//---------- rather than accesing directly by  GetTextureCoordinatesPtr

/*************************************************
Summary: Returns a pointer to the vertices texture coordinates.

Arguments:
	Stride: Size in byte between each texture coordinate in the returned buffer.
	channel: Index of the texture channel whose coordinates should be returned. -1 means
	the default texture coordinates, additionnal channels are accessed from 0 to MAX_CHANNELS
Return Value:
	A pointer to the texture coordinates list
Remarks:
	+ Use this method along with GetModifierUVCount rather than using 
	GetTextureCoordinatesPtr to acces texture coordinates so that you code will work
	on any type of mesh (CKMesh and CKPatchMesh for example).
	+ For a pure CKMesh this methods is the same than GetTextureCoordinatesPtr.
Example:
	// Translates all the uvs of a mesh by a given translation vector
	
	VxUV Translation(0.01f,0);
	int VCount = mesh->GetModifierUVCount();
	CKDWORD Stride;
	BYTE* VPtr = mesh->GetModifierUVs(&Stride);

	if (VPtr) {
		for (int i = 0; i < VCount; ++i ,VPtr+=Stride) {
			(* ((VxUV*) VPtr )) += Translation;
		}
	}

	// Since texture coordinates were changed we need to notify the mesh...
	mesh->ModifierUVMove();

See Also: ModifierUVMove, GetTextureCoordinatesPtr
*************************************************/
virtual BYTE*	GetModifierUVs(CKDWORD* Stride,int channel=-1) = 0;

virtual int		GetModifierUVCount(int channel=-1) = 0;

/*************************************************
Summary: Notifies when texture coordinates have been changed.

See Also: GetModifierUVs,GetModifierUVCount
*************************************************/
virtual void	ModifierUVMove () = 0;   

//------------------------------------------------------------------------
// VERTEX ACCES 

/*************************************************
Summary: Returns the number of vertices.

Return Value:
	 Returns the number of vertices.
See Also: SetVertexCount
*************************************************/
virtual int		GetVertexCount() = 0;

/*************************************************
Summary: Sets the number of vertices.

Arguments:
	Count: Required number of vertices.
Remarks:
	+ This method updates the number of vertices 
	in the mesh while keeping any previous vertices.
Return Value:
	TRUE if successful.		
See Also: GetVertexCount
*************************************************/
virtual CKBOOL	SetVertexCount(int Count) = 0;

/*************************************************
Summary: Sets the color of a vertex.

Arguments:
	Index: Index of the vertex.
	Color: Color to be set to the vertex (CKDWORD ARGB).
Remarks:	
	+ Vertex colors are only used if the mesh is set in prelit mode
	(see SetLitMode).
See Also: GetVertexColor,SetVertexSpecularColor,SetLitMode
*************************************************/
virtual	void	SetVertexColor(int Index, CKDWORD Color) = 0;

/*************************************************
Summary: Sets the specular color of a vertex.

Arguments:
	Index: Index of the vertex.
	Color: Specular color to be set to the vertex.
Remarks:
	+ Vertex colors are only used if the mesh is set in prelit mode
	(see SetLitMode).
See Also: GetVertexSpecularColor,SetVertexColor
*************************************************/
virtual void	SetVertexSpecularColor(int Index, CKDWORD Color) = 0;

/*************************************************
Summary: Sets the normal for vertex.

Arguments:
	Index: Index of the vertex.
	Vector: A pointer to the normal vector that is to be set to the vertex.
Remarks:
	+ Vertex normals are only used if the mesh is set in lit mode
	(see SetLitMode).
See Also: GetVertexNormal,GetNormalsPtr
*************************************************/
virtual void	SetVertexNormal(int Index, VxVector *Vector) = 0;

/*************************************************
Summary: Sets the position of a vertex.

Arguments:
	Index: Index of the vertex in the mesh.
	Vector: A pointer to the position vector that is to be set to the vertex.
See Also: GetVertexPosition,GetPositionsPtr,GetModifierVertices
*************************************************/
virtual void	SetVertexPosition(int Index, VxVector *Vector) = 0;

/*************************************************
Summary: Sets the texture coordinates of a vertex.

Arguments:
	Index: Index of the vertex.
	u: U Texture coordinate .
	v: V Texture coordinate.
	channel: Index of the texture channel whose coordinates should be set. -1 means
	the default texture coordinates, additionnal channels are accessed from 0 to MAX_CHANNELS
See Also: GetVertexTextureCoordinates,GetTextureCoordinatesPtr
*************************************************/
virtual void	SetVertexTextureCoordinates(int Index, float u,float v,int channel=-1) = 0;

/************************************************
Summary: Returns the array of vertices colors

Arguments:
	Stride: A pointer to a CKDWORD that will be filled with the 
amount in bytes between each color stored in returned array.
Return Value:
	A pointer to the color list.
Remarks:
	+ The vertex color depends on the light state (VX_LITMODE) of 
	the mesh. If that state is VX_LITMESH and no call to 
	SetLitMode(VX_PRELITMESH) was done before, then there is no 
	colors to return. 
	+ If you used this method to change the vertex color you must call ColorChanged to warn the Render engine.

See also: SetVertexColor, SetLitMode,GetSpecularColorsPtr
************************************************/
virtual void*	GetColorsPtr(CKDWORD* Stride) = 0;

/************************************************
Summary: Returns the array of vertices specular colors

Arguments:
	Stride: A pointer to a CKDWORD that will be filled with the 
amount in bytes between each specular color stored in the array 
returned
Return Value:
	A pointer to the specular color list.
Remarks:
	+ The vertex color depends on the light state (VX_LITMODE) of 
	the mesh. If that state is VX_LITMESH and no call to 
	SetLitMode(VX_PRELITMESH) was done before, then there is no 
	colors to return. 
	+ If you used this method to change some colors you must call ColorChanged to warn the Render engine.
See also: SetVertexColor, SetLitMode,GetSpecularColorsPtr
************************************************/
virtual void*	GetSpecularColorsPtr(CKDWORD* Stride) = 0;

/************************************************
Summary: Returns the array of normals.

Arguments:
	Stride: A pointer to a CKDWORD that will be filled with the 
	amount in bytes between each normals stored in the returned array.
Return Value:
	A pointer to the normals list.
Remarks:
	+ If you used this method to change some colors you must call ColorChanged to warn the Render engine.
	
See Also: GetVertexNormal,SetVertexNormal
************************************************/
virtual void*	GetNormalsPtr(CKDWORD* Stride) = 0;

/************************************************
Summary: Returns the array of vertex positions

Arguments:
	Stride: A pointer to a CKDWORD that will be filled with the 
amount in bytes between each vertex positions stored in the returned array.
Return Value:
	A pointer to the vertex position list.
See Also: SetVertexPosition,GetVertexPosition
************************************************/
virtual void*	GetPositionsPtr(CKDWORD* Stride) = 0;

/************************************************
Summary: Returns the array of texture coordinates.

Arguments:
	Stride: A pointer to a CKDWORD that will be filled with the 
amount in bytes between each texture coordinate stored in the returned array.
	channel: Channel index for which the list is to obtained (See Remarks).
Return Value:
	A pointer to the Texture coordinates list.
Remarks:
	+ If channel = -1, list of texture coordinates of each vertex is returned,
	else channel's texture coordinates list is returned.
	+ Returns NULL for invalid channel.
See Also: SetVertexTextureCoordinates,AddChannel
************************************************/
virtual void*	GetTextureCoordinatesPtr(CKDWORD* Stride,int channel=-1) = 0;

/*************************************************
Summary: Gets the color of a given vertex.

Arguments:
	Index: Index of the vertex in the mesh.
Return Value:
	Color value of vertex (ARGB 32 Bits).
See Also: SetVertexColor,GetColorsPtr
*************************************************/
virtual CKDWORD	GetVertexColor(int Index) = 0;

/*************************************************
Summary: Gets the specular color of a given vertex.

Arguments:
	Index: Index of the vertex in the mesh.
Return Value:
	Specular color value of vertex (ARGB 32 Bits).
Remarks:
See Also: SetVertexSpecularColor,GetSpecularColorsPtr
*************************************************/
virtual CKDWORD	GetVertexSpecularColor(int Index) = 0;

/*************************************************
Summary: Gets the Normal of a given vertex.

Arguments:
	Index: Index of the vertex in the mesh which normal should be returned.
	Vector: A pointer to be filled with normal of the vertex.
See Also: SetVertexNormal,GetNormalsPtr
*************************************************/
virtual void	GetVertexNormal(int Index, VxVector *Vector) = 0;

/*************************************************
Summary: Gets the position of a given vertex.

Arguments:
	Index: Index of the vertex in the mesh which position should be returned.
	Vector: A pointer to be filled with the position of the vertex.
See Also: SetVertexPosition,GetPositionsPtr,GetModifierVertices
*************************************************/
virtual void	GetVertexPosition(int Index, VxVector *Vector) = 0;

/*************************************************
Summary: Gets the texture coordinates of a given vertex.

Arguments:
	Index: Index of the vertex in mesh.
	u: Texture coordinate value.
	v: Texture coordinate value.
	channel: Channel index.
See Also: SetVertexTextureCoordinates,GetTextureCoordinatesPtr,GetModifierUVs
*************************************************/
virtual void	GetVertexTextureCoordinates(int Index, float *u,float *v,int channel=-1) = 0;

/*************************************************
Summary: Translates all vertices of the mesh by given vector

Arguments:
	Vector: A pointer to vector by which mesh vertices should be translated.
See Also: ScaleVertices,RotateVertices
*************************************************/
virtual void	TranslateVertices(VxVector *Vector) = 0;

/*************************************************
Summary: Scales all vertices of the mesh by given vector.

Arguments:
	Vector: A pointer to a vector by which mesh vertices should be scaled.
	Pivot: A pointer to a vector giving the pivot of the scale operation.
	X: Scaling factor for X direction.
	Y: Scaling factor for Y direction.
	Z: Scaling factor for Z direction.
Remarks: 
+ Scaling ratios in x,y,z directions can be controlled by Vector or 
by independent X, Y, Z factors.
+ Scaling of vertices is done with respect to the pivot point provided.
See Also: TranslateVertices,RotateVertices
*************************************************/
virtual void	ScaleVertices(VxVector *Vector,VxVector *Pivot = NULL) = 0;
virtual void	ScaleVertices(float X,float Y,float Z,VxVector *Pivot = NULL) = 0;

/*************************************************
Summary: Rotates all vertices of the mesh by given angle.

Arguments:
	Vector: Axis of rotation.
	Angle: Angle by which the vertices have to be rotated.
See Also: ScaleVertices,TranslateVertices
*************************************************/
virtual void	RotateVertices(VxVector *Vector, float Angle) = 0;

/*************************************************
Summary: Notifies vertex movement.

Remarks:
	+ Notifies at least a vertex position has changed.
	+ This should be called if a position was changed after a call
	to GetPositionsPtr.
See Also: GetPositionsPtr
*************************************************/
virtual	void	VertexMove() = 0;

/*************************************************
Summary: Notifies texture coordinates modifications

Remarks:
	+ Notifies at least a vertex texture coordinates have changed.
	+ This should be called if a vertex texture coordinates were changed after a call
	to GetTextureCoordinatesPtr.
See Also: GetTextureCoordinatesPtr
*************************************************/
virtual	void	UVChanged() = 0;

/*************************************************
Summary: Notifies normal modifications.

Remarks:
	+ Notifies at least a vertex normal has changed.
	+ This should be called if a vertex normal was changed after a call
	to GetNormalsPtr.
See Also: GetNormalsPtr
*************************************************/
virtual	void	NormalChanged() = 0;

/*************************************************
Summary: Notifies vertex color modifications.

Remarks:
	+ Notifies at least a vertex normal has changed.
	+ This should be called if a vertex normal was changed after a call
	to GetNormalsPtr.
See Also: GetColorsPtr,GetSpecularColorsPtr
*************************************************/
virtual	void	ColorChanged() = 0;

//------------------------------------------------------------------------
//  FACES ACCES

/*************************************************
Summary: Returns number of faces in the mesh.

Return Value:
	Number of faces.
See Also: SetFaceCount
*************************************************/
virtual int	GetFaceCount() = 0;	

/*************************************************
Summary: Sets the number of faces for the mesh.

Return Value:
	TRUE if successful.
Arguments: 
	Count: Number of faces (triangles) to be set for the mesh.
See Also: GetFaceCount
*************************************************/
virtual CKBOOL	SetFaceCount(int Count) = 0; 

/*************************************************
Summary: Gets all the face indices of the mesh.

Return Value:
	A pointer to an array of WORD containing the face indices.
Remarks:
+ The size of the returned array is 3 times the number of faces in the mesh. Each face is made up of three vertices which indices are given in the array.
+ The returned array content must not be modify.
See Also:GetFaceCount,GetFaceVertexIndex
*************************************************/
virtual WORD*	GetFacesIndices() = 0;		

/*************************************************
Summary: Gets the vertices making a given face.

Arguments:
	Index: Index of the face which the vertex indices have to be obtained.
	Vertex1: First vertex
	Vertex2: Second vertex
	Vertex3: Third vertex
Remarks:
	+ Fills 3 vertices supplied with the vertices found at
	the specified position in face index list.
See Also: GetFaceCount,GetFacesIndices
*************************************************/
virtual void	GetFaceVertexIndex(int Index,int& Vertex1,int& Vertex2,int& Vertex3) = 0;

/*************************************************
Summary: Gets the material of a face.

Arguments:
	Index: Index of face which material is to be returned.
Return Value:
	Pointer to Material
Remarks:
	+ The material of a face defines the way the face will be drawn, as a general rule 
	it is best to avoid having too different materials on a given mesh.
See Also: GetFaceCount,SetFaceMaterial
*************************************************/
virtual CKMaterial*		GetFaceMaterial(int Index) = 0;

/*************************************************
Summary: Gets the normal of a given face.

Arguments:
	Index: Index of face for which normal has to be obtained.
Return Value:
	Normal of the face.
Remarks:
See Also: SetFaceNormal,BuildNormals,BuildFaceNormals
*************************************************/
virtual const VxVector&	GetFaceNormal(int Index) = 0;

/*************************************************
Summary: Gets the channel mask of a face.

Arguments:
	FaceIndex: Index of face which channel mask has to be obtained.
Return Value:
	Channel Mask.

Remarks:
	+ When adding a channel to a mesh, it is added to all the faces of the
	mesh, but you can exclude some faces from a channel so the additionnal
	material is not applied to them.
	+ The mask is a set of bit , each channel mask is given by the CKCHANNELMASK(channel)
	macro. A Mask of 0xFFFF indicates the face is included in all the channels.

Example:
	//  Search for the first face that does not belongs to the given channel
	int Channel = 2;
	CKWORD ChannelMask = CKCHANNELMASK(channel);
	int FaceCount = mesh->GetFacesCount();
	int TheFace = -1;

	for (int i=0; i < FaceCount; ++i) {
		if (! (GetFaceChannelMask(i) & ChannelMask)) {
			TheFace = i;
			break;
		}
	}

See Also: SetFaceChannelMask, ChangeFaceChannelMask
*************************************************/
virtual CKWORD	GetFaceChannelMask(int FaceIndex) = 0;


/*************************************************
Summary: Gets a vertex from face.

Arguments:
	FaceIndex: Index of face in which vertex has to be obtained.
	VIndex: Index of vertex in the face (0..2)
Return Value:
	Vertex of the given face.
Remarks:	
	+ This method is a shortcut to return directly the vertex position
	of the vertex used by a face. It is equivalent to using GetFaceVertexIndex
	then GetVertexPosition with the result indices.
See Also: SetFaceVertexIndex,GetFaceVertexIndex,GetVertexPosition
*************************************************/
virtual VxVector&		GetFaceVertex(int FaceIndex,int VIndex) = 0;


/*************************************************
Summary: Returns the pointer to the list of face normals.

Arguments:
	Stride : Size in byte between each normals in the returned array .
Return Value:
	BYTE Pointer containing the address of the normal of the first face. 
Remarks:
	+ the returned pointer is the address of an array containing VxVector
	normals for each face, Stride contains the size in bytes between each normal.
	+ see GetModifierVertices for example usage of the Stride value.

See Also: GetFaceNormal, SetFaceNormal
*************************************************/
virtual BYTE*	GetFaceNormalsPtr(CKDWORD* Stride) = 0;

/*************************************************
Summary: Sets the vertex indices for a face.

Arguments:
	FaceIndex: Index of face for which vertex indices have to be set.
	Vertex1: First vertex index in the face.
	Vertex2: Second vertex index in the face.
	Vertex3: Third vertex index in the face.

See Also: GetFaceVertexIndex,GetFaceIndices
*************************************************/
virtual void	SetFaceVertexIndex(int FaceIndex,int Vertex1,int Vertex2,int Vertex3) = 0;

/*************************************************
Summary: Sets the material for one or more faces.

Arguments:
	FaceIndex: Index of the face for which material has to be set.
	FaceIndices: List of face indices for which material has to be set.
	Mat	: A pointer to the material to be set.
	FaceCount: Number of faces submitted.
Remarks:
	+ If a material has to be changed frequently on a mesh, you must use
	ReplaceMaterial instead of this methods as they force the reconstruction
	of runtime specific structures each time a face have its material changed.
See Also: GetFaceMaterial,ReplaceMaterial
*************************************************/
virtual void	SetFaceMaterial(int FaceIndex, CKMaterial *Mat) = 0;
virtual void	SetFaceMaterial(int* FaceIndices,int FaceCount, CKMaterial *Mat) = 0;


virtual void	SetFaceChannelMask(int FaceIndex,CKWORD ChannelMask) = 0;

/*************************************************
Summary: Replaces any reference to a material by a new.

Arguments:
	oldMat	: A pointer to the old material to be replaced.
	newMat	: A pointer to the new material to be set.
Remarks:
	+ Using this method is recommended to set a new material to 
	some faces which previously used oldMat. The replace operation
	does not force the render engine to re-sort its per-material 
	runtime specific structures so it's much faster than using SetFaceMaterial.

See Also: GetFaceMaterial, SetFaceMaterial
*************************************************/
virtual void	ReplaceMaterial(CKMaterial *oldMat, CKMaterial *newMat) = 0;

/*************************************************
Summary: Modifies the channel mask of a face.

Arguments:
	FaceIndex: Index of face which channel mask has to be modified.
	AddChannelMask : Mask to be set.
	RemoveChannelMask: Mask to be removed.
Remarks:
	+ When adding a channel to a mesh, it is added to all the faces of the
	mesh, but you can exclude some faces from a channel so the additionnal
	material is not applied to them.
	+ The mask is a set of bit , each channel mask is given by the CKCHANNELMASK(channel)
	macro. A Mask of 0xFFFF indicates the face is included in all the channels.
	+ If you want all the faces to be exclude from a channel, de-activate the channel
	with ActivateChannel(channel,FALSE) rather than using this method for all the faces.

Example:
	//  Sets additionnal channel 0 to be displayed on all faces
	int Channel = 0;
	CKWORD ChannelMask = CKCHANNELMASK(channel);
	int FaceCount = mesh->GetFacesCount();

	for (int i=0; i < FaceCount; ++i) {
		ChangeFaceChannelMask(i,ChannelMask,0);
	}

See Also: GetFaceChannelMask, AddChannel
*************************************************/
virtual void	ChangeFaceChannelMask(int FaceIndex,CKWORD AddChannelMask,CKWORD RemoveChannelMask) = 0;

/*************************************************
Summary: Sets the same material to all faces.

Arguments:
	Mat	: A pointer to the material to be set.
See Also: SetFaceMaterial,GetFaceMaterial,
*************************************************/
virtual void	ApplyGlobalMaterial(CKMaterial *Mat) = 0;

/*************************************************
Summary: Dissociates all faces.

Remarks:
	+ This method duplicates vertices so that faces do not 
	share any vertices any more. The number of vertices then
	becomes 3 times the number of faces in the mesh.
See Also: GetFaceIndices,
*************************************************/
virtual void	DissociateAllFaces() = 0;

//------------------------------------------------------------------------
//    LINES	  

/*************************************************
Summary: Sets the number of lines in the mesh.

Return Value:
	TRUE if succesful.
Arguments:
	Count: Number of lines.
See Also: GetLineCount
*************************************************/
virtual CKBOOL	SetLineCount(int Count) = 0;

/*************************************************
Summary: Returns the number of lines in the mesh.

Return Value:
	Number of lines in mesh.
See Also: SetLineCount
*************************************************/
virtual	int		GetLineCount() = 0;

/*************************************************
Summary: Returns the list of line indices.

Return Value:
	Pointer to vertex indices of lines in mesh.
Remarks:
	The returned array size is 2*GetLineCount WORD indices.
See Also: SetLine,GetLine
*************************************************/
virtual CKWORD*	GetLineIndices() = 0;

/*************************************************
Summary: Sets the vertex indices to a line.

Arguments:
	LineIndex: Index of the line which vertex indices have to be set.
	VIndex1: First vertex index.
	VIndex2: Second vertex index.
See Also: GetLineIndices,GetLineCount
*************************************************/
virtual void	SetLine(int LineIndex,int VIndex1,int VIndex2) = 0;

/*************************************************
Summary: Gets the vertex indices of a line.

Arguments:
	LineIndex: Index of the line whose vertices are to be obtained.
	VIndex1: First vertex index, to be filled.
	VIndex2: Second vertex index, to be filled.
See Also: SetLine,GetLineIndices,GetLineCount
*************************************************/
virtual void	GetLine(int LineIndex,int *VIndex1,int *VIndex2) = 0;

/*************************************************
Summary: Creates a line strip.

Arguments:
	StartingLine: Starting position of line in line segment array.
	Count: Number of segments to be created.
	StartingVertexIndex: Starting index of the line segment.
Remarks: 
	+ Creates a line strip with specified number of line segments.
	+ CreateLineStrip(0,3,14) will generate 3 lines (0,1,2) whose
	vertex indices will respectively be : {(14,15),(15,16),(16,17)}
See Also: SetLine,GetLineIndices,GetLineCount
*************************************************/
virtual void	CreateLineStrip(int StartingLine,int Count,int StartingVertexIndex) = 0;


//------------------------------------------------------------------------
//    MISC UTILITIES 

/*************************************************
Summary: Removes all primitives(faces,lines,material channels)

Arguments:
	KeepVertices: A flag specifying whether to retain vertices or not.
Remarks: 
	+ This removes all data related to mesh, such as faces, lines and material channels.
	+ The vertices can be kept in order to reconstruct the mesh.
*************************************************/
virtual void  	Clean(CKBOOL KeepVertices = FALSE) = 0;

/*************************************************
Summary: Inverse the faces by changing the winding order of the vertices.

Remarks:
	+ This method also updates the normals of the faces and vertices.
See Also: GetFaceIndices,GetFaceNormal,GetFaceVertexIndex
************************************************/
virtual void	InverseWinding() = 0;

/*************************************************
Summary: Removes the degenerate faces and the isolated vertices of a mesh.

Remarks:
	+ Removes degenerates faces ( lines, points or 0 normal faces) and 
	unused vertices.
************************************************/
virtual void	Consolidate() = 0;

/*************************************************
Summary: Marks the mesh as to be re-optimized. 

Remarks:
	+ Before rendering a mehs is optimized according to 
	its material and face organisation to minimize 
	render state changes. 
	+ This method is automatically called by the framework when
	necessary.
*************************************************/
virtual void	UnOptimize() = 0;

//------------------------------------------------------------------------
// BOUNDING VOLUMES 

/*************************************************
Summary: Returns the radius of the mesh.

Return Value:
	Radius of mesh.
See Also: GetLocalBox, GetBaryCenter
************************************************/
virtual float	GetRadius() = 0;

/*************************************************
Summary: Returns the bounding box of the mesh.

Return Value:
	Returns the bounding box of the mesh in its local frame.
See Also: GetRadius, GetBaryCenter,VxBbox
*************************************************/
virtual const VxBbox&	GetLocalBox() = 0;

/*************************************************
Summary: Returns the barycenter of the mesh.

Return Value:
	Barycenter of the mesh.
Remarks:
	+ The barycenter of the mesh is the average of 
	all the vertices positions.
See Also: GetLocalBox, GetRadius
*************************************************/
virtual void	GetBaryCenter(VxVector *Vector) = 0;

//------------------------------------------------------------------------
// MATERIAL CHANNELS 

/*************************************************
Summary: Returns the number of channels in the mesh.

Return Value:
	Number of additionnal channels in the mesh.
See Also: AddChannel, RemoveChannel
*************************************************/
virtual int		GetChannelCount() = 0;

/*************************************************
Summary: Adds a channel to the mesh.

Arguments: 
	Mat: A pointer to the material used by the channel.
	CopySrcUv: TRUE if default texture coordinates have to be copied to this channel,
				FALSE otherwise.
Return Value:
	Index of the newly created channel.
See Also: RemoveChannel, GetChannelCount
*************************************************/
virtual int		AddChannel(CKMaterial *Mat,CKBOOL CopySrcUv=TRUE) = 0;

/*************************************************
Summary: Removes a channel from the mesh.

Arguments:
	Mat: A pointer to material, channel corresponding to this material will be removed.
	Index: Channel index in the mesh.
See Also: AddChannel, GetChannelCount
*************************************************/
virtual void	RemoveChannel(CKMaterial *Mat) = 0;
virtual void	RemoveChannel(int Index) = 0;

/*************************************************
Summary: Returns the index of a channel given its material.

Arguments:
	Mat: A pointer to material, channel corrensponding to this material will be returned.
Return Value:
	Channel index.
See Also: AddChannel, GetChannelCount
*************************************************/
virtual int		GetChannelByMaterial(CKMaterial *mat) = 0;

/*************************************************
Summary: Sets the active flag of the specified channel.

Arguments:
	Index: Channel index.
	Active: Active flag, TRUE activates, FALSE deactivates it.
Remarks:
	+ This method sets or removes the VXCHANNEL_ACTIVE flags for this channel.
	+ A deactivated channel is not drawn.
See also: ActivateAllChannels, IsChannelActive, AddChannel, RemoveChannel
*************************************************/
virtual void	ActivateChannel(int Index,CKBOOL Active=TRUE) = 0;

/************************************************
Summary: Returns the active flag of a specific channel.

Arguments:
	Index: Channel index.
Return Value: TRUE if the specified channel is active, otherwise FALSE. 
Remarks:
    + If channel Index parameter is invalid then the method returns FALSE.
See also: ActivateChannel, ActivateAllChannels, AddChannel, RemoveChannel,VXCHANNEL_FLAGS
************************************************/
virtual CKBOOL	IsChannelActive(int Index) = 0;

/************************************************
Name: ActivateAllChannels

Summary: Sets the active flag for all channels.
Arguments:
	Active: Active flag, TRUE is active, FALSE deactivates it.
Remarks:
	+ Shows or hides all the channels at once.
See also: ActivateChannel, IsChannelActive, AddChannel, RemoveChannel,VXCHANNEL_FLAGS
************************************************/
virtual void	ActivateAllChannels(CKBOOL Active=TRUE) = 0;

/************************************************
Summary: Sets the lighting flag of the specified channel.

Arguments:
	Index: Channel index.
	Lit: Lighting flag, TRUE for the channel to be lit, FALSE otherwise.
Remarks:
	+ This method sets or removes the VXCHANNEL_NOTLIT flags for this channel.
	+ A additionnal channel can be drawn with as a decal ( only the texture color 
	is used) or be lit (influenced by lights) as for the vertices of the mesh.
See also: IsChannelLit,  AddChannel, RemoveChannel,VXCHANNEL_FLAGS
************************************************/
virtual void	LitChannel(int Index,CKBOOL Lit=TRUE) = 0;

/************************************************
Summary: Returns whether a specific channel is  to be lit.

Arguments:
	Index: Channel index.
Return Value: TRUE if the specified channel is lit, otherwise FALSE. 
Remarks: 
	+ This method checks the VXCHANNEL_NOTLIT flags for this channel.
	+ If channel Index parameter is invalid then the method returns FALSE.
See also: LitChannel, AddChannel, RemoveChannel,VXCHANNEL_FLAGS
************************************************/
virtual CKBOOL	IsChannelLit(int Index) = 0;

/*************************************************
Summary: Returns the Channel flags of a channel.

Arguments: 
	Index : Index of the channel
Return Value: 
	VXCHANNEL_FLAGS for the given channel .
Remarks:
	+ The channel flags contain the activation and lighting flags 
	and alo some hint or information set by the render engine. For example 
	is the channel was render along with the default faces in monopass
	multi-texturing.
See Also: SetChannelFlags,VXCHANNEL_FLAGS
*************************************************/
virtual CKDWORD GetChannelFlags(int Index) = 0;

/*************************************************
Summary: Sets the Channel flags of a channel.

Arguments: 
	Index : Index of the channel
	Flags:  VXCHANNEL_FLAGS to be set for the channel.
Remarks:
	+ The channel flags contain the activation and lighting flags 
	and alo some hint or information set by the render engine. For example 
	is the channel was render along with the default faces in monopass
	multi-texturing.
See Also: GetChannelFlags,VXCHANNEL_FLAGS
*************************************************/
virtual void	SetChannelFlags(int Index,CKDWORD Flags) = 0;

/************************************************
Summary: Returns the material of a channel.

Arguments:
	Index: Index of the channel.
Return Value: 
	A pointer to the CKMaterial which is used by the channel.
See also: SetChannelMaterial
************************************************/
virtual CKMaterial*		GetChannelMaterial(int Index) = 0;

/************************************************
Summary: Returns the source blend mode of a specific channel.

Return Value:
	Source blending factor.
Arguments:
	Index: Channel index.
Remarks:
	+ When a mesh with channels is rendered, the color of what is already drawn and the
	color of this mesh are combined as shown for each pixel :

		(destination blend factor) * (what is already drawn) +
		(source blend factor) * (what is to be drawn).
See also: GetChannelDestBlend, SetChannelSourceBlend
************************************************/
virtual VXBLEND_MODE	GetChannelSourceBlend(int Index) = 0;

/************************************************
Summary: Returns the destination blend mode of a specific channel.

Arguments:
	Index: Channel index.
Return Value:
	Destination blending factor.
Remarks:
	+ When a mesh with channels is rendered, the color of what is already drawn and the
	color of this mesh are combined as shown for each pixel :

		(destination blend factor) * (what is already drawn) +
		(source blend factor) * (what is to be drawn).
See also: GetChannelSourceBlend, SetChannelDestBlend
************************************************/
virtual VXBLEND_MODE	GetChannelDestBlend(int Index) = 0;

/************************************************
Summary: Sets material of a given channel.

Arguments:
	Index: Channel index.
	Mat: New CKMaterial which will be assigned to the channel.
See also: GetChannelByMaterial
************************************************/
virtual void	SetChannelMaterial(int Index,CKMaterial *Mat) = 0;

/************************************************
Summary: Sets the source blend mode of a specific channel.

Arguments:
	Index: Channel index.
	BlendMode: A VXBLEND_MODE type.
Remarks:
	+ When a mesh with channels is rendered, the color of what is already drawn and the
color of this mesh are combined as shown for each pixel :

		(destination blend) * (what is already drawn) +
		(source blend) * (what is to be drawn).

See also: SetChannelDestBlend, GetChannelSourceBlend
************************************************/
virtual void	SetChannelSourceBlend(int Index,VXBLEND_MODE BlendMode) = 0;

/************************************************
Summary: Sets the destination blend mode of a specific channel.

Arguments:
	Index: Channel index.
	BlendMode: A VXBLEND_MODE type.
Return Value:
	Current destination blend mode.
Remarks:
	+ When a mesh with channels is rendered, the color of what is already drawn and the
color of this mesh are combined as shown for each pixel :

		(destination blend) * (what is already drawn) +
		(source blend) * (what is to be drawn).
See also: SetChannelSourceBlend, GetChannelDestBlend
************************************************/
virtual void	SetChannelDestBlend(int Index,VXBLEND_MODE BlendMode) = 0;

//------------------------------------------------------------------------
// NORMALS	

/*************************************************
Summary: Calculates vertex normals of all vertices of the mesh.

See Also: BuildFaceNormals
*************************************************/
virtual 	void	BuildNormals() = 0;

/*************************************************
Summary: Calculates face normals of all faces in the mesh.

See Also: BuildNormals
*************************************************/
virtual 	void	BuildFaceNormals() = 0;

//------------------------------------------------------------------------
//	RENDERING

/************************************************
Summary: Renders the mesh.

Arguments:
	Dev: A CKrenderContext on which the mesh should be rendered.
	Mov: A optionnal 3dEntity for position, scaling, rotation... infomation.

Return Value: CK_OK if successful, an error code otherwise.
Remarks:
	+ This method works as following: first, it calls pre-render callbacks, then it renders the
	mesh or it calls a user defined procedure and last, it calls post-render callbacks.
	+ The current world transformation matrix (See CKRenderContext::SetWorldTransformationMatrix)
	is used to transform the vertices.
See also: CKRenderContext::Render;CK3dEntity::Render
************************************************/
virtual 	CKERROR	Render(CKRenderContext *Dev,CK3dEntity *Mov) = 0;

//------------------------------------------------------------------------
//	CALLBACKS

/*************************************************
Summary: Adds a function to be called before rendering the mesh.

Arguments:
	Function: A function of type CK_RENDEROBJECT_CALLBACK which will be called before drawing the mesh.
	Argument: Argument that will be passed to the function.
	Temporary: A Boolean to indicate if the callback should be called only once and then removed
Return Value: TRUE if successful, FALSE otherwise.
See also: RemovePreRenderCallBack, SetRenderCallBack
*************************************************/
virtual CKBOOL	AddPreRenderCallBack(CK_MESHRENDERCALLBACK Function,void *Argument,BOOL Temporary = FALSE) = 0;

/*************************************************
Summary: Removes a pre-render callback function.

Arguments:
	Function: Function to be removed.
	Argument: Argument of that function (as passed to AddPreRenderCallBack).
Return Value: TRUE if successful, FALSE otherwise.
See also: AddPreRenderCallBack, SetRenderCallBack
*************************************************/
virtual CKBOOL	RemovePreRenderCallBack(CK_MESHRENDERCALLBACK Function,void *Argument) = 0;

/*************************************************
Summary: Adds a function to be called after rendering the mesh.

Arguments:
	Function: A function of type CK_RENDEROBJECT_CALLBACK which will be called after drawing the mesh.
	Argument: Argument that will be passed to the function.
	Temporary: A Boolean to indicate if the callback should be called only once and then removed
Return Value: TRUE if successful, FALSE otherwise.
See also: RemovePostRenderCallBack, SetRenderCallBack
*************************************************/
virtual CKBOOL	AddPostRenderCallBack(CK_MESHRENDERCALLBACK Function,void *Argument,BOOL Temporary = FALSE) = 0;

/*************************************************
Summary: Removes a post-render callback function.

Arguments:
	Function: Function to be removed.
	Argument: Argument of that function (as passed to AddPostRenderCallBack).
Return Value: TRUE if successful, FALSE otherwise.
See also: AddPostRenderCallBack, SetRenderCallBack
*************************************************/
virtual CKBOOL	RemovePostRenderCallBack(CK_MESHRENDERCALLBACK Function,void *Argument) = 0;

/*************************************************
Summary: Sets the mesh rendering function.

Arguments:
	Function: Rendering Function.
	Argument: Argument of that function.
Remarks:
	+ The given function will replace the default rendering method 
	for the mesh. To restore it use SetDefaultRenderCallBack
See also: SetDefaultRenderCallBack
*************************************************/
virtual void	SetRenderCallBack(CK_MESHRENDERCALLBACK Function,void *Argument) = 0;

/*************************************************
Summary: Restores the default rendering function.

See also: SetRenderCallBack
*************************************************/
virtual void	SetDefaultRenderCallBack() = 0;

/*************************************************
Summary: Removes all the callback functions.

Remarks:
	+ All Pre,Postand RenderCallback functions are removed.
See also: SetRenderCallBack, AddPreRenderCallBack, AddPostRenderCallBack
*************************************************/
virtual void	RemoveAllCallbacks() = 0;

//---------------------------------------------------
// Access to Materials list

/****************************************************
Summary: Fills an array with all materials used by the faces of the mesh.

Arguments:
	array: Array of type XObjectPointerArray, to be filled with materials.
Return Value: CK_OK if successful, FALSE otherwise.
Remarks:
	+ The function only returns materials used by the faces of the mesh, not the channels.
	+ It is more convenient to use GetMaterialCount and GetMaterial.
See also: SetFaceMaterial, SetChannelMaterial,GetMaterialCount,GetMaterial
************************************************/
//virtual  const XObjectPointerArray& ComputeMaterialList() = 0;

/*************************************************
Summary: Returns the number of different materials.

Return Value:
	Number of materials used in the mesh.
Remarks:
	+ The function only returns the number of materials used by the faces of the mesh, not the channels.
See Also: GetMaterial
*************************************************/
virtual int	    GetMaterialCount() = 0;

/*************************************************
Summary: Returns the material given its index.

Arguments: 
	index: Index at which material is to be obtained.
Return Value: 
	Pointer to material.
See Also: GetMaterialCount
*************************************************/
virtual CKMaterial* GetMaterial(int index) = 0;

//---------------------------------------------------
// Access to Vertex Weights

/*************************************************
Summary: Returns the number of vertex weights.

Return Value:
	Number of vertex weights available.
Remarks:
	+ Normally the number of vertex weight is to be the 
	same than the number of vertices.
See Also: SetVertexWeightsCount, GetVertexWeightsPtr, SetVertexWeight
*************************************************/
virtual int		GetVertexWeightsCount() = 0;

/*************************************************
Summary: Sets the number of vertex weights.

Arguments:
	count: Number of vertex weights required.
See Also: GetVertexWeightsCount, GetVertexWeightsPtr, GetVertexWeight, SetVertexWeight
*************************************************/
virtual void	SetVertexWeightsCount(int count) = 0;

/*************************************************
Summary: Returns a pointer to the list of vertex weights.

Return Value:
	A pointer to an array of float containing each vertex weight or NULL if there are no weights.
See Also: GetVertexWeightsCount, SetVertexWeightsCount, GetVertexWeight, SetVertexWeight
*************************************************/
virtual float*  GetVertexWeightsPtr() = 0;

/*************************************************
Summary: Returns the vertex weight of a given vertex.

Arguments:
	index: Position from which vertex wieght is to be obtained.
Return Value:
	Vertex weight at given index.
See Also: SetVertexWeightsCount, GetVertexWeightsPtr, SetVertexWeight
*************************************************/
virtual float   GetVertexWeight(int index) = 0;

/*************************************************
Summary: Sets the vertex weight at a given position.

Arguments:
	index: Position from which vertex weight is to be set.
	w: Vertex Weight value.
See Also: SetVertexWeightsCount, GetVertexWeightsPtr, GetVertexWeight
*************************************************/
virtual void    SetVertexWeight(int index,float w) = 0;

/*************************************************
Summary: Restores the vertices from a chunk.

Arguments:
	chunk: a CKStateChunk holding mesh data.
Remarks:
This method restores the vertex data (positions,normals and texture coordinates,...)
See Also: BuildNormals
*************************************************/
virtual void    LoadVertices(CKStateChunk *chunk) = 0;

/*************************************************
Summary: Sets the number of vertices to be rendered.

Arguments:
	count: Number of vertices that will actually be rendered.
Remarks:
This function works only if the mesh has been transformed to a progressive mesh, using CreatePM()

See Also: GetVerticesRendered, CreatePM
*************************************************/
virtual void	SetVerticesRendered(int count) = 0;

/*************************************************
Summary: Gets the number of vertices actually rendered.

Return Value:
	Number of vertices that will actually be rendered.
Remarks:
This function works only if the mesh has been transformed to a progressive mesh, using CreatePM()
See Also: SetVerticesRendered, CreatePM
*************************************************/
virtual int		GetVerticesRendered() = 0;

/*************************************************
Summary: Creates the progressive mesh data for the mesh.

Return Value:
	CK_OK if it successful or an error code otherwise.
Remarks:
Progressive mesh allow you to generates level of detail meshes seamlessly and continuously, by variating the number of vertices to be rendered, rendering less when the mesh is far away.
See Also: SetVerticesRendered, DestroyPM
*************************************************/
virtual CKERROR	CreatePM() = 0;

/*************************************************
Summary: Destroy the progressive mesh data for the mesh.

See Also: CreatePM
*************************************************/
virtual void	DestroyPM() = 0;

/*************************************************
Summary: Returns if the mesh is progressive.
Return Value:
	TRUE if the mesh is a progressive mesh.

See Also: CreatePM, DestroyPM
*************************************************/
virtual CKBOOL	IsPM() = 0;

/*************************************************
Summary: Sets the progressive mesh to proceed geomorphic
transition when changing level.

See Also: IsPMGeoMorphEnabled
*************************************************/
virtual void	EnablePMGeoMorph(CKBOOL enable) = 0;

/*************************************************
Summary: Returns if the progressive mesh enable geomorphic
transition.
Return Value:
	TRUE if the progressive mesh enable geomorphic transition.

See Also: EnablePMGeoMorph
*************************************************/
virtual CKBOOL	IsPMGeoMorphEnabled() = 0;

/*************************************************
Summary: Sets the geomorph steps.


Arguments:
	gs: steps for the geomorphic progressive decimation. 

Remarks:
	0 means the mesh will morph between level by dichotomy : 
	nv <-> nv/2, then nv/2 <-> nv/4, etc.
	Values greater than 0 means a linear degradation of the mesh
	step by step : nv <-> nv - gs, then nv - gs <-> nv - 2*gs, etc.

See Also: EnablePMGeoMorph
*************************************************/
virtual void	SetPMGeoMorphStep(int gs) = 0;

/*************************************************
Summary: Gets the geomorph steps.


Return Value:
	steps for the geomorphic progressive decimation. 

See Also: SetPMGeoMorphStep
*************************************************/
virtual int		GetPMGeoMorphStep() = 0;


/*************************************************
Summary: Adds a function to be called before rendering of a sub-part of the mesh.

Arguments:
	Function: A function of type CK_SUBMESHRENDERCALLBACK which will be called before drawing the mesh sub-part.
	Argument: Argument that will be passed to the function.
	Temporary: A Boolean to indicate if the callback should be called only once and then removed
Return Value: TRUE if successful, FALSE otherwise.
Remarks:
	+ A Mesh is divided in sub-parts when it uses several materials,each sub-parts only use a given material.
See also: RemoveSubMeshPreRenderCallBack,AddSubMeshPostRenderCallBack
*************************************************/
virtual CKBOOL	AddSubMeshPreRenderCallBack(CK_SUBMESHRENDERCALLBACK Function,void *Argument,BOOL Temporary = FALSE) = 0;

/*************************************************
Summary: Removes a pre-render callback function.

Arguments:
	Function: Function to be removed.
	Argument: Argument of that function (as passed to AddSubMeshPreRenderCallBack).
Return Value: TRUE if successful, FALSE otherwise.
See also: AddPreRenderCallBack, SetRenderCallBack
*************************************************/
virtual CKBOOL	RemoveSubMeshPreRenderCallBack(CK_SUBMESHRENDERCALLBACK Function,void *Argument) = 0;

/*************************************************
Summary: Adds a function to be called after rendering a mesh sub-part.

Arguments:
	Function: A function of type CK_SUBMESHRENDERCALLBACK which will be called after drawing the mesh sub-part.
	Argument: Argument that will be passed to the function.
	Temporary: A Boolean to indicate if the callback should be called only once and then removed
Return Value: TRUE if successful, FALSE otherwise.
See also: RemoveSubMeshPostRenderCallBack, AddSubMeshPreRenderCallBack
*************************************************/
virtual CKBOOL	AddSubMeshPostRenderCallBack(CK_SUBMESHRENDERCALLBACK Function,void *Argument,BOOL Temporary = FALSE) = 0;

/*************************************************
Summary: Removes a post-render callback function.

Arguments:
	Function: Function to be removed.
	Argument: Argument of that function (as passed to AddSubMeshPostRenderCallBack).
Return Value: TRUE if successful, FALSE otherwise.
See also: AddPostRenderCallBack, RemoveSubMeshPreRenderCallBack
*************************************************/
virtual CKBOOL	RemoveSubMeshPostRenderCallBack(CK_SUBMESHRENDERCALLBACK Function,void *Argument) = 0;

/*************************************************
Summary: Dynamic cast operator. 
Arguments:
	iO: A pointer to a CKObject to cast.
Return Value:
	iO casted to the appropriate class or NULL if iO is not from the required class .
Example:
	  CKObject* Object;
	  CKAnimation* anim = CKAnimation::Cast(Object);
Remarks:

*************************************************/
static CKMesh* Cast(CKObject* iO) 
{
	return CKIsChildClassOf(iO,CKCID_MESH)?(CKMesh*)iO:NULL;
}


CKMesh() {}
CKMesh(CKContext *Context,CKSTRING name=NULL)  : CKBeObject(Context,name) {} 
};

#endif

