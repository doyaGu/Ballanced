/**************************************************************************/
/*	File : CKBaseManager.h												 */
/*	Author :  Romain Sididris											 */	
/*																		 */	
/*	Base Class for Virtools Managers									 */	
/*	Virtools SDK 														 */	 
/*	Copyright (c) Virtools 2000, All Rights Reserved.					 */	
/*************************************************************************/
#ifndef CKBASEMANAGER_H
#define CKBASEMANAGER_H "$Id:$"

#include "CKDefines.h"

class CKFile;
class CKStateChunk;

#define MAX_MANAGERFUNC_PRIORITY		30000

#define DEFAULT_MANAGERFUNC_PRIORITY	0

/*************************************************
Summary: Mask for Base Manager overridable functions.

Remarks:
	+ When implementing a manager the CKBaseManager::GetValidFunctionsMask 
	must be overriden to return a combination of these flag to indicate
	which methods are implemented.
See also: CKBaseManager,CKBaseManager::GetValidFunctionsMask
*************************************************/
typedef enum CKMANAGER_FUNCTIONS {
				CKMANAGER_FUNC_OnSequenceToBeDeleted		= 0x00000001,	// CKBaseManager::OnSequenceToBeDeleted
				CKMANAGER_FUNC_OnSequenceDeleted			= 0x00000002,	// CKBaseManager::OnSequenceDeleted
				CKMANAGER_FUNC_PreProcess					= 0x00000004,	// CKBaseManager::PreProcess
				CKMANAGER_FUNC_PostProcess					= 0x00000008,	// CKBaseManager::PostProcess
				CKMANAGER_FUNC_PreClearAll					= 0x00000010,	// CKBaseManager::PreClearAll
				CKMANAGER_FUNC_PostClearAll					= 0x00000020,	// CKBaseManager::PostClearAll
				CKMANAGER_FUNC_OnCKInit						= 0x00000040,	// CKBaseManager::OnCKInit
				CKMANAGER_FUNC_OnCKEnd						= 0x00000080,	// CKBaseManager::OnCKEnd
				CKMANAGER_FUNC_OnCKPlay						= 0x00000100,	// CKBaseManager::OnCKPlay
				CKMANAGER_FUNC_OnCKPause					= 0x00000200,	// CKBaseManager::OnCKPause
				CKMANAGER_FUNC_PreLoad						= 0x00000400,	// CKBaseManager::PreLoad
				CKMANAGER_FUNC_PreSave						= 0x00000800,	// CKBaseManager::PreSave
				CKMANAGER_FUNC_PreLaunchScene				= 0x00001000,	// CKBaseManager::PreLaunchScene
				CKMANAGER_FUNC_PostLaunchScene				= 0x00002000,	// CKBaseManager::PostLaunchScene
				CKMANAGER_FUNC_OnCKReset					= 0x00004000,	// CKBaseManager::OnCKReset
				CKMANAGER_FUNC_PostLoad						= 0x00008000,	// CKBaseManager::PostLoad
				CKMANAGER_FUNC_PostSave						= 0x00010000,	// CKBaseManager::PostSave
				CKMANAGER_FUNC_OnCKPostReset				= 0x00020000,	// CKBaseManager::OnCKPostReset
				CKMANAGER_FUNC_OnSequenceAddedToScene		= 0x00040000,	// CKBaseManager::OnSequenceAddedToScene
				CKMANAGER_FUNC_OnSequenceRemovedFromScene	= 0x00080000,	// CKBaseManager::OnSequenceRemovedFromScene
				CKMANAGER_FUNC_OnPreCopy					= 0x00100000,	// CKBaseManager::OnPreCopy
				CKMANAGER_FUNC_OnPostCopy					= 0x00200000,	// CKBaseManager::OnPostCopy
				CKMANAGER_FUNC_OnPreRender					= 0x00400000,	// CKBaseManager::OnPreRender
				CKMANAGER_FUNC_OnPostRender					= 0x00800000,	// CKBaseManager::OnPostRender
				CKMANAGER_FUNC_OnPostSpriteRender			= 0x01000000,	// CKBaseManager::OnPostSpriteRender
				CKMANAGER_FUNC_OnPreBackToFront				= 0x02000000,	// CKBaseManager::OnPreBackToFront
				CKMANAGER_FUNC_OnPostBackToFront			= 0x04000000,	// CKBaseManager::OnPostBackToFront
				CKMANAGER_FUNC_OnPreFullScreen				= 0x08000000,	// CKBaseManager::OnPreFullScreen
				CKMANAGER_FUNC_OnPostFullScreen				= 0x10000000	// CKBaseManager::OnPostFullScreen
} CKMANAGER_FUNCTIONS;


typedef enum CKMANAGER_FUNCTIONS_INDEX {
				CKMANAGER_INDEX_OnSequenceToBeDeleted			= 0,
				CKMANAGER_INDEX_OnSequenceDeleted				= 1,
				CKMANAGER_INDEX_PreProcess						= 2,
				CKMANAGER_INDEX_PostProcess						= 3,
				CKMANAGER_INDEX_PreClearAll						= 4,
				CKMANAGER_INDEX_PostClearAll					= 5,
				CKMANAGER_INDEX_OnCKInit						= 6,
				CKMANAGER_INDEX_OnCKEnd							= 7,
				CKMANAGER_INDEX_OnCKPlay						= 8,
				CKMANAGER_INDEX_OnCKPause						= 9,
				CKMANAGER_INDEX_PreLoad							= 10,
				CKMANAGER_INDEX_PreSave							= 11,
				CKMANAGER_INDEX_PreLaunchScene					= 12,
				CKMANAGER_INDEX_PostLaunchScene					= 13,
				CKMANAGER_INDEX_OnCKReset						= 14,
				CKMANAGER_INDEX_PostLoad						= 15,
				CKMANAGER_INDEX_PostSave						= 16,
				CKMANAGER_INDEX_OnCKPostReset					= 17,
				CKMANAGER_INDEX_OnSequenceAddedToScene			= 18,
				CKMANAGER_INDEX_OnSequenceRemovedFromScene		= 19,
				CKMANAGER_INDEX_OnPreCopy						= 20,
				CKMANAGER_INDEX_OnPostCopy						= 21,
				CKMANAGER_INDEX_OnPreRender						= 22,
				CKMANAGER_INDEX_OnPostRender					= 23,
				CKMANAGER_INDEX_OnPostSpriteRender				= 24,
				CKMANAGER_INDEX_OnPreBackToFront				= 25,
				CKMANAGER_INDEX_OnPostBackToFront				= 26,
				CKMANAGER_INDEX_OnPreFullScreen					= 27,
				CKMANAGER_INDEX_OnPostFullScreen				= 28,
} CKMANAGER_FUNCTIONS_INDEX;


/*************************************************************************
Summary: Base Class for managers.

Remarks: 
+ This class provides virtual methods that can be override by any managers. Any manager that inherits from CKBaseManager can override function to do some processing.

+ The instances of managers may be retrieved through the global function CKContext::GetManagerByGuid()

+ Some default managers implemented in Virtools  can be accessed directly : See Managers Access



See also: CKContext::RegisterNewManager,Implementing a external Manager
*************************************************************************/
class CKBaseManager {
public:
	
	CKBaseManager(CKContext *Context,CKGUID guid,CKSTRING Name);
	
	virtual ~CKBaseManager();
	virtual CKStateChunk* SaveData(CKFile* SavedFile);
	virtual CKERROR LoadData(CKStateChunk *chunk,CKFile* LoadedFile);
	virtual CKERROR PreClearAll();
	virtual CKERROR PostClearAll();
	virtual CKERROR PreProcess();
	virtual CKERROR PostProcess();
	virtual CKERROR SequenceAddedToScene(CKScene *scn,CK_ID *objids,int count);
	virtual CKERROR SequenceRemovedFromScene(CKScene *scn,CK_ID *objids,int count);
	virtual CKERROR PreLaunchScene(CKScene* OldScene,CKScene* NewScene);
	virtual CKERROR PostLaunchScene(CKScene* OldScene,CKScene* NewScene);
	virtual CKERROR OnCKInit();
	virtual CKERROR OnCKEnd();
	virtual CKERROR OnCKReset();
	virtual CKERROR OnCKPostReset();
	virtual CKERROR OnCKPause();
	virtual CKERROR OnCKPlay();
	virtual CKERROR SequenceToBeDeleted(CK_ID *objids,int count);
	virtual CKERROR SequenceDeleted(CK_ID *objids,int count);
	virtual CKERROR PreLoad();
	virtual CKERROR PostLoad();
	virtual CKERROR PreSave();
	virtual CKERROR PostSave();
	virtual CKERROR OnPreCopy(CKDependenciesContext& context);
	virtual CKERROR OnPostCopy(CKDependenciesContext& context);
	virtual CKERROR OnPreRender(CKRenderContext* dev);
	virtual CKERROR OnPostRender(CKRenderContext* dev);
	virtual CKERROR OnPostSpriteRender(CKRenderContext* dev);
	virtual int		GetFunctionPriority(CKMANAGER_FUNCTIONS Function);
	virtual CKDWORD	GetValidFunctionsMask();

	CKGUID			m_ManagerGuid;		// Manager GUID
	CKSTRING		m_ManagerName;		// Manager Name
	CKContext*		m_Context;			// A pointer to the CKContext on which this manager is valid.
	float			m_ProcessingTime;	// Time elapsed during profiling. (Reset each frame before behavioral processing starts)
	VxTimeProfiler  m_Timer;			

};

#endif