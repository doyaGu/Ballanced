#ifndef _MODIFYCOMPONENT_H_
#define _MODIFYCOMPONENT_H_

//--------------------------------------------------
//types
//--------------------------------------------------

struct ComponentData;
//modify function
typedef void (*ModifyParameter)(const CKBehaviorContext& BehContext,CKDWORD flags);

//--------------------------------------------------
//Modify Functions
//--------------------------------------------------
void Modify2DVector		(const CKBehaviorContext& BehContext,CKDWORD flags);
void ModifyBox			(const CKBehaviorContext& BehContext,CKDWORD flags);
void ModifyColor		(const CKBehaviorContext& BehContext,CKDWORD flags);
void ModifyEulerAngles	(const CKBehaviorContext& BehContext,CKDWORD flags);
void ModifyQuaternion	(const CKBehaviorContext& BehContext,CKDWORD flags);
void ModifyRect			(const CKBehaviorContext& BehContext,CKDWORD flags);
void ModifyVector		(const CKBehaviorContext& BehContext,CKDWORD flags);

//--------------------------------------------------
//Databases
//--------------------------------------------------

typedef struct ComponentData{
    XSArray<CKDWORD>			m_flags;	//parameter flag	(from settings)
                                        //store something from FLAG_xxx + FLAG_POUT if output parameter
    XClassArray<XString>	m_names;	//parameter name
    XSArray<int>			m_types;	//parameter types

    int						m_count;	//parameter count
    CKGUID					m_componentGuid;	//guid of parameter to modify (the component)
    int						m_componentType;	//type of that parameter

    ModifyParameter			m_modifyFunction;	//behavior function to modify the parameter

    ComponentData();
    ~ComponentData();
}ComponentData;

extern XClassArray<ComponentData> g_Components;

void InitializeModifyComponent(CKContext* context);
void DestroyModifyComponent(CKContext* context);

//--------------------------------------------------
//Utils functions (in Modify_Base.cpp)
//--------------------------------------------------

//removes all pin & pout from behavior except the main input & output
//sets the type of the main pout with the main pin's type
void ClearParameters(const CKBehaviorContext& BehContext);

//return pin index according to its name.
//return value : 
//	-1		:	parameter isn't	here
//	0-n		:	parameter exists, and the return value is its index
int GetPinIndex		(CKBehavior * beh,char * name);

//return pout index according to its name.
//return value : 
//	-1		:	parameter isn't	here
//	0-n		:	parameter exists, and the return value is its index
int GetPoutIndex	(CKBehavior * beh,char * name);

int		ComponentIndexToType(int index);
int		ComponentTypeToIndex(int type);
CKGUID	ComponentIndexToGuid(int index);
int		ComponentGuidToIndex(CKGUID guid);
//--------------------------------------------------
//defines & enums
//--------------------------------------------------

//count
#define	COMPONENT_COUNT			7

//behavior ios
#define IN_IN			0
#define OUT_OUT			0
#define PIN_VIN			0
#define POUT_VOUT		0
#define	SETTINGS_FLAGS	0

//flags guid
#define CKPGUID_MODIFYCOMPONENT_2DVECTOR			CKDEFINEGUID(0x741f6152,0x574d1dc0)
#define CKPGUID_MODIFYCOMPONENT_BOX					CKDEFINEGUID(0x741f6152,0x574d1dc1)
#define CKPGUID_MODIFYCOMPONENT_COLOR				CKDEFINEGUID(0x741f6152,0x574d1dc2)
#define CKPGUID_MODIFYCOMPONENT_EULERANGLES			CKDEFINEGUID(0x741f6152,0x574d1dc3)
#define CKPGUID_MODIFYCOMPONENT_QUATERNION			CKDEFINEGUID(0x741f6152,0x574d1dc4)
#define CKPGUID_MODIFYCOMPONENT_RECT				CKDEFINEGUID(0x741f6152,0x574d1dc5)
#define CKPGUID_MODIFYCOMPONENT_VECTOR				CKDEFINEGUID(0x741f6152,0x574d1dc6)

//type allowed indexes
#define	INDEX_2DVECTOR		0
#define	INDEX_BOX			1
#define	INDEX_COLOR			2
#define	INDEX_EULERANGLES	3
#define	INDEX_QUATERNION	4
#define	INDEX_RECT			5
#define	INDEX_VECTOR		6

//RegisterNewFlags for ck2, settings
#define SETTINGS_2DVECTOR_NAME		"Modify 2DVector Settings"
#define SETTINGS_BOX_NAME			"Modify Box Settings"
#define SETTINGS_COLOR_NAME			"Modify Color Settings"
#define SETTINGS_EULERANGLES_NAME	"Modify EulerAngles Settings"
#define SETTINGS_QUATERNION_NAME	"Modify Quaternion Settings"
#define SETTINGS_RECT_NAME			"Modify Rect Settings"
#define SETTINGS_VECTOR_NAME		"Modify Vector Settings"

#define SETTINGS_2DVECTOR		"Modify X=1,Modify Y=4"
#define SETTINGS_BOX			"Modify Min=1,Modify Max=4"
#define SETTINGS_COLOR			"Modify R=1,Modify G=4,Modify B=16,Modify Alpha=64"
#define SETTINGS_EULERANGLES	"Modify Angle1=1,Modify Angle2=4,Modify Angle3=16"
#define SETTINGS_QUATERNION		"Modify X=1,Modify Y=4,Modify Z=16,Modify W=64"
#define SETTINGS_RECT			"Modify TopLeft=1,Modify BottomRight=4"
#define SETTINGS_VECTOR			"Modify X=1,Modify Y=4,Modify Z=16"

//old type (stored in settings): 0-6 value, in last 4 bits
typedef enum {
    TYPE_2DVECTOR		=	0x10000000,
    TYPE_BOX			=	0x20000000,
    TYPE_COLOR			=	0x30000000,
    TYPE_EULERANGLES	=	0x40000000,
    TYPE_QUATERNION		=	0x50000000,
    TYPE_RECT			=	0x60000000,
    TYPE_VECTOR			=	0x70000000,
    TYPE_ALL			=	0xf0000000,
}SETTINGS_TYPE;

//add that in component parameter flag so that u know it's a parameter output
//so ==> DON'T PUT MORE THAN 31 PARAMETER IN A COMPONENT
#define FLAG_POUT	0x80000000

//2DVector component
typedef enum {
    V2_X		=	0x00000001,
    V2_Y		=	0x00000004,
}FLAG_2DVECTOR;

#define STR_V2_X		"X"
#define STR_V2_Y		"Y"

//Box component
typedef enum {
    B_V1		=	0x00000001,
    B_V2		=	0x00000004,
}FLAG_BOX;

#define STR_B_V1		"Min"
#define STR_B_V2		"Max"

//Color component
typedef enum {
    C_R			=	0x00000001,
    C_G			=	0x00000004,
    C_B			=	0x00000010,
    C_A			=	0x00000040,
}FLAG_COLOR;

#define STR_C_R			"R"
#define STR_C_G			"G"
#define STR_C_B			"B"
#define STR_C_A			"Alpha"

//EulerAngles component
typedef enum {
    E_A1		=	0x00000001,
    E_A2		=	0x00000004,
    E_A3		=	0x00000010,
}FLAG_EULERANGLES;

#define STR_E_A1		"Angle 1"
#define STR_E_A2		"Angle 2"
#define STR_E_A3		"Angle 3"

//Quaternion component
typedef enum {
    Q_X			=	0x00000001,
    Q_Y			=	0x00000004,
    Q_Z			=	0x00000010,
    Q_W			=	0x00000040,
}FLAG_QUATERNION;

#define STR_Q_X				"X"
#define STR_Q_Y				"Y"
#define STR_Q_Z				"Z"
#define STR_Q_W				"W"

//Rect component
typedef enum {
    R_V1		=	0x00000001,
    R_V2		=	0x00000004,
}FLAG_RECT;

#define STR_R_V1		"TopLeft"
#define STR_R_V2		"BottomRight"

//Vector component
typedef enum {
    V_X			=	0x00000001,
    V_Y			=	0x00000004,
    V_Z			=	0x00000010,
}FLAG_VECTOR;

#define STR_V_X			"X"
#define STR_V_Y			"Y"
#define STR_V_Z			"Z"

#endif