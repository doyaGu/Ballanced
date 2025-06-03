/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Calculator
//
//_____________________________________________
// Rem : look for NEWOP to add a new operation
// and don't forget the documentation
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "Calculator.h"



CKObjectDeclaration	*FillBehaviorCalculatorDecl();
CKERROR CreateCalculatorProto(CKBehaviorPrototype **);
int Calculator(const CKBehaviorContext& behcontext);

CKERROR CalculatorCallBack(const CKBehaviorContext& behcontext); // CallBack Function


CKObjectDeclaration	*FillBehaviorCalculatorDecl()
{
  CKObjectDeclaration *od = CreateCKObjectDeclaration("Calculator");	
  od->SetDescription("Calculates the value of an arithmetic expression; 'a' stand for the first parameter, 'b' for the second, ...etc.");
  /* rem:
  <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
  <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
  <BR>                
  <SPAN CLASS=pin>Expression: </SPAN>Arithmetic expression .<BR>
  <SPAN CLASS=pin>A: </SPAN>first value.<BR>
  <SPAN CLASS=pin>B: </SPAN>second value.<BR>
  <SPAN CLASS=pin>C: </SPAN>third value.<BR>
  <SPAN CLASS=pin>D: </SPAN>...etc.<BR>
  <BR>
  <SPAN CLASS=pout>X: </SPAN>result.<BR>
  <BR>
  You can add as many 'Input Parameters' as you want.<BR>
  The first  argument after <FONT COLOR=#000077>'Expression'</FONT> will stand for 'a' in the arithmetic expression.<BR>
  The second argument after <FONT COLOR=#000077>'Expression'</FONT> will stand for 'b' in the arithmetic expression.<BR>
  ...etc.<BR>
  You should note that whatever name the input param has, its associated letter will only be determined by its position (as described above).<BR>
  <P>
  <FONT SIZE=2>Available FLOAT Operations :</FONT><BR>
  <TABLE WIDTH="450" BORDER="0" CELLSPACING="0" CELLPADDING="0">
  <TR>
    <TD WIDTH="30%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Operation</B></FONT><BR>
    <FONT SIZE=-2>
    Addition<BR>
    Subtraction<BR>
    Multiplication<BR>
    Division<BR>
    Sine<BR>
    Tangent<BR>
    Cosine<BR>
    Absolute Value<BR>
    Square Root<BR>
    Arctangent<BR>
    ArcSine<BR>
    ArcCosine<BR>
    Power<BR>
    Natural Logarithme<BR>
    Exponential<BR>
    Modulus<BR>
    </FONT>
    </TD>
    <TD WIDTH="20%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Syntax</B></FONT><BR>
    <FONT SIZE=-2>
    +<BR>
    -<BR>
    *<BR>
    /<BR>
    Sin<BR>
    Tan<BR>
    Cos<BR>
    Abs<BR>
    Sqrt<BR>
    Atan<BR>
    Asin<BR>
    Acos<BR>
    ^<BR>
    Ln<BR>
    Exp<BR>
    \<BR>
    </FONT>
    </TD>
    <TD WIDTH="50%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Example</B></FONT><BR>
    <FONT SIZE=-2>
    a+5<BR>
    b-c<BR>
    -3.2+(7*b)<BR>
    5/11.5<BR>
    Sin(a*pi)<BR>
    5*Cos(0.2)<BR>
    abs(-3.8) ----> 3.8<BR>
    sin(pi/3)-Sqrt(3)/2<BR>
    tan(pi/4) ----> 1.0<BR>
    atan(1) ----> pi/4<BR>
    Asin(1) ----> 1.570...<BR>
    Acos(0.5) ----> 1.04...<BR>
    3^2       ----> 9<BR>
    Ln(2.72)<BR>
    Exp(1)    ----> 2.72...<BR>
    32\6      ----> 2<BR>
    </FONT>
    </TD>
  </TR>
  </TABLE>
  <P>
  <FONT SIZE=2>Available VECTOR Operations :</FONT><BR>
  <TABLE WIDTH="500" BORDER="0" CELLSPACING="0" CELLPADDING="0">
  <TR>
    <TD WIDTH="30%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Operation</B></FONT><BR>
    <FONT SIZE=-2>
    Addition<BR>
    Subtraction<BR>
    Dot Product<BR>
    Division<BR>
    Absolute Value<BR>
    Cross Product<BR>
    </FONT>
    </TD>
    <TD WIDTH="20%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Syntax</B></FONT><BR>
    <FONT SIZE=-2>
    +<BR>
    -<BR>
    *<BR>
    /<BR>
    Abs<BR>
    ^<BR>
    </FONT>
    </TD>
    <TD WIDTH="50%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Example</B></FONT><BR>
    <FONT SIZE=-2>
    a+b ----> if a=[1,-2,3] & b=[-4,5,6] then x=[-3,3,9]<BR>
    b-c ----> if b=[1,-2,3] & c=[-4,5,6] then x=[5,-7,-3]<BR>
    a*b ----> if a=[3,1,0] & b=[-2,5,1] then x=-1<BR>
    a/b ----> if a=[1,0,3] & b=[-4,2,6] then x=[-0.25,0,2]<BR>
    abs(a) ----> if a=[-1,5,-3] then x=[1,5,3]<BR>
    a^b ----> if a=[1,0,0] & b=[0,1,0] then x=[0,0,1]<BR>
    </FONT>
    </TD>
  </TR>
  </TABLE>
  <P>
  <FONT SIZE=2>Available Constants :</FONT><BR>
  <TABLE WIDTH="500" BORDER="0" CELLSPACING="0" CELLPADDING="0">
    <TD WIDTH="20%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Syntax</B></FONT><BR>
    <FONT SIZE=-2>
    PI<BR>
    </FONT>
    </TD>
    <TD WIDTH="40%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Associated Value</B></FONT><BR>
    <FONT SIZE=-2>
    3.141592654...<BR>
    </FONT>
    </TD>
    <TD WIDTH="40%" VALIGN=TOP><FONT SIZE=2 COLOR=#EEEEEE><B>Example</B></FONT><BR>
    <FONT SIZE=-2>
    5*Sin(pi)<BR>
    </FONT>
    </TD>
  </TABLE>
  <P>
  <FONT SIZE=2>Additionnal Technical Information :</FONT><BR>
  - All Arithmetic Expressions are CASE-INSENSITIVE ; which means, "sin(pi/b)" = "SIN(PI/B)" = "Sin(Pi/b)" = "sIN(pI/B)"<BR>
  - Priority between operation are managed ; which means <FONT SIZE=2>/</FONT> has a higher priority than <FONT SIZE=2>+</FONT>.<BR>
  - The tree describing the combination of operations is created each time the Arithmetic Expression String change, but this tree isn't recreated if a parametric input change its value. 
  It means that the 'Calculator' buiding block is in fact a very efficient way to compute mathematic operations (Where 'Parametric Operations' need time to create and compute mathematic combinations).<BR>
  - For the moment the syntax [x,y,z] is not allowed. You can write V.x (the x component of V), but you can write in it (like [0,V.y,V.z]). 
  Use 'Set Component' to rebuild your vector (or color...).<BR>
	- Scientific number writing is allowed. It means 100000 can be written 1e5 (or 10e4), and 0.1 can be written 1e-1 (or 10e-2).<BR>
  */
  /* warning:
  - All the 'input parameters' must be either FLOATS, VECTOR ANGLES, PERCENTAGE (except the very first one, which stand for the arithmetic STRING expression).<BR>
  - If you create a non-FLOAT or non-VECTOR or non-ANGLE or non-PERCENTAGE 'input param',
  it will be transformed into a FLOAT one. Beware ... you'll have to set your value again !!!<BR>
  */
  od->SetCategory("Logics/Calculator");
  od->SetType( CKDLL_BEHAVIORPROTOTYPE);
  od->SetGuid(CKGUID(0x4bc209b8,0x5b3679b0));
  od->SetAuthorGuid(VIRTOOLS_GUID);
  od->SetAuthorName("Virtools");
  od->SetVersion(0x00020000);
  od->SetCreationFunction(CreateCalculatorProto);
  od->SetCompatibleClassId(CKCID_BEOBJECT);
  return od;
}


/************************************************/
/*        Op Functions of Calculator            */
/************************************************/
typedef int (*CalculatorOp)( float *val, int *type );
CalculatorOp CalculatorOpFct[A_CALULATOR_NB_OP];

#define DO_EVALNODE( value, type ) float value[3]; int type; if( SUCCESS != EvalNode( value, &type ) ) return RUNTIME_ERROR; 

//NEWOP : add function here

//___________________________________________
//____________________________________/ XCOMP
int CalcOp_Xcomp( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( type1==VECTOR_VALUE ){ // if float
    *val = val1[0];
    *type = FLOAT_VALUE;
    return SUCCESS;
  }
  return RUNTIME_ERROR;
}
//___________________________________________
//____________________________________/ YCOMP
int CalcOp_Ycomp( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( type1==VECTOR_VALUE ){ // if float
    *val = val1[1];
    *type = FLOAT_VALUE;
    return SUCCESS;
  }
  return RUNTIME_ERROR;
}
//___________________________________________
//____________________________________/ ZCOMP
int CalcOp_Zcomp( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( type1==VECTOR_VALUE ){ // if float
    *val = val1[2];
    *type = FLOAT_VALUE;
    return SUCCESS;
  }
  return RUNTIME_ERROR;
}
//___________________________________________
//____________________________________/ ADD
int CalcOp_Add( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
  val[0] = val2[0] + val1[0];
  if( type1==type2 ){
    if( type1==FLOAT_VALUE ){ // if float
      *type = FLOAT_VALUE;
      return SUCCESS;
    }
    val[1] = val2[1] + val1[1]; // else vector
    val[2] = val2[2] + val1[2];
    *type = VECTOR_VALUE;
    return SUCCESS;
  }
  return RUNTIME_ERROR;
}
//___________________________________________
//____________________________________/ SUB
int CalcOp_Sub( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
  val[0] = val2[0] - val1[0];
  if( type1==type2 ){
    if( type1==FLOAT_VALUE ){ // if float
      *type = FLOAT_VALUE;
      return SUCCESS;
    }
    val[1] = val2[1] - val1[1]; // else vector
    val[2] = val2[2] - val1[2];
    *type = VECTOR_VALUE;
    return SUCCESS;
  }
  return RUNTIME_ERROR;
}
//___________________________________________
//____________________________________/ MULT 
int CalcOp_Mult( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
  if( type1==type2 ){
    if( type1==FLOAT_VALUE ){ // if float.float
      val[0] = val2[0] * val1[0];
      *type = FLOAT_VALUE;
      return SUCCESS;
    } // else vector.vector (DotProduct)
    val[0] = val2[0] * val1[0] + val2[1] * val1[1] + val2[2] * val1[2];
    *type = FLOAT_VALUE;
    return SUCCESS;
  } // if float.vector or vector.float
  *type = VECTOR_VALUE;
  if( type1==FLOAT_VALUE ){
    val[0] = val1[0] * val2[0];
    val[1] = val1[0] * val2[1];
    val[2] = val1[0] * val2[2];
    return SUCCESS;
  }
  val[0] = val2[0] * val1[0];
  val[1] = val2[0] * val1[1];
  val[2] = val2[0] * val1[2];
  return SUCCESS;
}
//___________________________________________
//____________________________________/ DIV
int CalcOp_Div( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
  if( type1==type2 ){
    if( type1==FLOAT_VALUE ){ // if float.float
      if(0.0f == val1[0]) return RUNTIME_ERROR;
      val[0] = val2[0] / val1[0];
      *type = FLOAT_VALUE;
      return SUCCESS;
    } // else vector.vector (DotProduct)
    if(0.0f == val1[0]) return RUNTIME_ERROR;
    if(0.0f == val1[1]) return RUNTIME_ERROR;
    if(0.0f == val1[2]) return RUNTIME_ERROR;
    val[0] = val2[0] / val1[0] + val2[1] / val1[1] + val2[2] / val1[2];
    *type = FLOAT_VALUE;
    return SUCCESS;
  } // if float.vector or vector.float
  *type = VECTOR_VALUE;
  if( type1==FLOAT_VALUE ){
    val[0] = val2[0] / val1[0];
    val[1] = val2[1] / val1[0];
    val[2] = val2[2] / val1[0];
    return SUCCESS;
  }
  val[0] = val2[0] / val1[1];
  val[1] = val2[0] / val1[2];
  val[2] = val2[0] / val1[3];
  return SUCCESS;
}
//___________________________________________
//____________________________________/ SIN
int CalcOp_Sin( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  *val = sinf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ COS
int CalcOp_Cos( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  *val = cosf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ TAN
int CalcOp_Tan( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  *val = tanf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ SQRT
int CalcOp_Sqrt( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( 0.0f > val1[0] )
    return RUNTIME_ERROR;
  *val = sqrtf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ ACOS
int CalcOp_Acos( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( 1.0f < val1[0] || -1.0f > val1[0] )
    return RUNTIME_ERROR;
  *val = acosf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ ASIN
int CalcOp_Asin( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( 1.0f < val1[0] || -1.0f > val1[0] )
    return RUNTIME_ERROR;
  *val = asinf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ ABS
int CalcOp_Abs( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  val[0] = fabsf( val1[0] );
  if( type1==FLOAT_VALUE ){ // if float
    *type = FLOAT_VALUE;
    return SUCCESS;
  }
  val[1] = fabsf( val1[1] ); // else vector
  val[2] = fabsf( val1[2] );
  *type = VECTOR_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ POW
int CalcOp_Pow( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
	if( type1==VECTOR_VALUE ){
		if( type2==VECTOR_VALUE ){
			val[0] = val1[1]*val2[2]-val2[1]*val1[2];
			val[1] = val2[0]*val1[2]-val1[0]*val2[2];
			val[2] = val1[0]*val2[1]-val2[0]*val1[1];
		  *type = VECTOR_VALUE;
			return SUCCESS;
		}
	}
  *val = powf( val2[0], val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ POW (new corrected)
int CalcOp_Pow_Correct( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
	if( type1==VECTOR_VALUE ){
		if( type2==VECTOR_VALUE ){
			val[0] = val2[1]*val1[2]-val1[1]*val2[2];
			val[1] = val1[0]*val2[2]-val2[0]*val1[2];
			val[2] = val2[0]*val1[1]-val1[0]*val2[1];
		  *type = VECTOR_VALUE;
			return SUCCESS;
		}
	}
  *val = powf( val2[0], val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ ATAN
int CalcOp_Atan( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  *val = atanf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ LN
int CalcOp_Ln( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  if( 0.0f >= val1[0] )
    return RUNTIME_ERROR;
  *val = logf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ EXP
int CalcOp_Exp( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  *val = expf( val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ MOD
int CalcOp_Mod( float *val, int *type ){
  DO_EVALNODE( val1, type1 );
  DO_EVALNODE( val2, type2 );
  
  if( 0.0f == val1[0] )
    return RUNTIME_ERROR;
  *val = fmodf( val2[0], val1[0] );
  *type = FLOAT_VALUE;
  return SUCCESS;
}
//___________________________________________
//____________________________________/ ERROR
int CalcOp_ERROR( float *val, int *type ){
  *type = FLOAT_VALUE;
  return RUNTIME_ERROR;
}

/************************************************/
/*                Proto                         */
/************************************************/
CKERROR CreateCalculatorProto(CKBehaviorPrototype **pproto)
{
  //---
  //NEWOP
  CalculatorOpFct[ADD] = CalcOp_Add;
  CalculatorOpFct[SUB] = CalcOp_Sub;
  CalculatorOpFct[MULT] = CalcOp_Mult;
  CalculatorOpFct[DIV] = CalcOp_Div;
  CalculatorOpFct[OPEN_P] = CalcOp_ERROR;
  CalculatorOpFct[CLOSED_P] = CalcOp_ERROR;
  CalculatorOpFct[SIN] = CalcOp_Sin;
  CalculatorOpFct[COS] = CalcOp_Cos;
  CalculatorOpFct[TAN] = CalcOp_Tan;
  CalculatorOpFct[ABS] = CalcOp_Abs;
  CalculatorOpFct[XCOMP] = CalcOp_Xcomp;
  CalculatorOpFct[YCOMP] = CalcOp_Ycomp;
  CalculatorOpFct[ZCOMP] = CalcOp_Zcomp;
  CalculatorOpFct[SQRT] = CalcOp_Sqrt;
  CalculatorOpFct[ASIN] = CalcOp_Asin;
  CalculatorOpFct[ACOS] = CalcOp_Acos;
  CalculatorOpFct[ATAN] = CalcOp_Atan;
  CalculatorOpFct[POW] = CalcOp_Pow_Correct;
  CalculatorOpFct[LN] = CalcOp_Ln;
  CalculatorOpFct[EXP] = CalcOp_Exp;
  CalculatorOpFct[MOD] = CalcOp_Mod;
  CalculatorOpFct[0] = CalcOp_ERROR;

  //---

  CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Calculator");
  if(!proto) return CKERR_OUTOFMEMORY;
  
  proto->DeclareInput("In");
  proto->DeclareOutput("Out");
  
  proto->DeclareInParameter("expression", CKPGUID_STRING);
  proto->DeclareInParameter("a", CKPGUID_FLOAT);
  
  proto->DeclareLocalParameter(NULL, CKPGUID_STRING); //"local expression"
  proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // "node_total_struct"
  
  proto->DeclareOutParameter("x", CKPGUID_FLOAT);
  
  proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
  proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS|CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS|CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS|CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
  proto->SetFunction(Calculator);
  
  proto->SetBehaviorCallbackFct( CalculatorCallBack );
  
  *pproto = proto;
  return CK_OK;
  
}


/************************************************/
/*            Behavior Function                 */
/************************************************/
int Calculator(const CKBehaviorContext& behcontext)
{
	CKBehavior* beh = behcontext.Behavior;
	CKContext* ctx	= behcontext.Context;

  beh->ActivateInput(0,FALSE);
  beh->ActivateOutput(0);
	
	int retval;  
  char *expression = (char*) beh->GetInputParameterReadDataPtr(0);
  char *local_expr = (char*) beh->GetLocalParameterReadDataPtr(0);
  
  Node_Total_Struct *nts = (Node_Total_Struct*) beh->GetLocalParameterReadDataPtr(1);
  
  if( strcmp( local_expr, expression ) ){ // If String has changed
    
    delete[] nts->node_stack;
    nts->node_stack = NULL;
    
    char *new_expr = CKStrdup( expression );
    retval = CreateNodeStack( new_expr, nts, beh );
    CKDeletePointer(new_expr);

    if( retval ){
      ctx->OutputToConsole("Syntax Error in CALCULATOR");

      char *local_expr = "#####";
      beh->SetLocalParameterValue(0, local_expr,strlen(local_expr)+1);
      
      return CKBR_OK;
    }

    beh->SetLocalParameterValue(0, expression,strlen(expression)+1);
  }
  
  //----------------------------------------    
  node_sptr = nts->nb_node;

  if( !node_sptr ) return CKBR_OK;

	current_node_stack = nts->node_stack;
  
  float x[3];
  int type;
  retval = EvalNode( x, &type );
  
  if( retval ) 
	  ctx->OutputToConsole("Illegal Operation In CALCULATOR");
  
  beh->SetOutputParameterValue(0, x);
  
  return CKBR_OK;
}



/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR CalculatorCallBack(const CKBehaviorContext& behcontext)
{
	CKBehavior* beh = behcontext.Behavior;
	
  switch( behcontext.CallbackMessage ){
    
  case CKM_BEHAVIORCREATE:
  case CKM_BEHAVIORLOAD:
    {
      char *local_expr = "#####";
      beh->SetLocalParameterValue(0, local_expr,strlen(local_expr)+1);
      
      Node_Total_Struct nts;
      nts.nb_node = 0;
      nts.node_stack = NULL;
      beh->SetLocalParameterValue(1, &nts, sizeof(Node_Total_Struct));
      
			if( beh->GetVersion()<0x00020000 ){ // back-compatibilty purpose
				CalculatorOpFct[POW] = CalcOp_Pow;
			}else{
				CalculatorOpFct[POW] = CalcOp_Pow_Correct;
			}

      break;
    }
    
  case CKM_BEHAVIORDELETE:
    {
      Node_Total_Struct *nts = (Node_Total_Struct*) beh->GetLocalParameterReadDataPtr(1);
      if( nts )	  {
		  delete[] nts->node_stack;
		  nts->node_stack = NULL;
	  }
      
      break;
    }  
    
  case CKM_BEHAVIOREDITED:
    {
      char *local_expr = "#####";
      beh->SetLocalParameterValue(0, local_expr,strlen(local_expr)+1);

      int c_pin = beh->GetInputParameterCount();
      
      CKParameterIn* pin;
      CKParameter* pout;
      
      if( pout = beh->GetOutputParameter(0) ){
        CKGUID tmp_guid;
        
        tmp_guid = pout->GetGUID(); // we check the type of the 'Output Parameter'
        if( (tmp_guid!=CKPGUID_FLOAT) && (tmp_guid!=CKPGUID_VECTOR) ){
          pout->SetGUID( CKPGUID_FLOAT );
        }

        while( c_pin > 1 ){ // we check the type of each 'Input Parameter'
          c_pin--;
          pin = beh->GetInputParameter( c_pin );
          tmp_guid = pin->GetGUID();
          
          if( (tmp_guid!=CKPGUID_FLOAT) && (tmp_guid!=CKPGUID_VECTOR) ){
          
            pin->SetGUID( CKPGUID_FLOAT );
            if( pout = pin->GetRealSource() ){
              pout->SetGUID( CKPGUID_FLOAT );
            }
          }
        }
        
      }
    }
    break;
    
  }
  
  return CKBR_OK; 
}




/********************************************************************/
/****************                              **********************/
/****************  CALCULATOR INNER FUNCTIONS  **********************/
/****************                              **********************/
/********************************************************************/

//_____________________________________
//  Remove White Space from a String
//
char *remove_space(char *str){
  char *old_str, *new_str;
  
  for (old_str=str, new_str=str ; *old_str ; ++old_str){
    if(!isspace(*old_str))
      *new_str++ = *old_str;
  }
  *new_str = 0;
  return str;
}

//______________________________________
//  Create the Node Stack
//
//  Arguments: 1 - String to Evaluate
//             2 - Storage to receive float result
//
//  Returns: SUCCESS if successful
//           CREATION_ERROR if syntax error
//           RUNTIME_ERROR  if runtime error
//
int CreateNodeStack(char *line, Node_Total_Struct *nts, CKBehavior *beh){
  
  Node arg;
  char *ptr = line, *str, *endptr;
  int ercode;
  Operation *op;
  
  CKStrupr( line );
  remove_space( line );
  state = op_sptr = node_sptr = parens = 0;
  
  while( *ptr ){
    
    switch( state ){
      
    case 0: // Expecting Expression
      if( NULL != (str = GetExpression( ptr )) ){
        
        if( NULL != (op = GetOp(str)) && strlen(str) == op->taglen ){
          
          PushOp(op->token);
          ptr += op->taglen;
          break;
        }
        
        if( SUCCESS == strcmp(str, "-") ){ ///////////////////// MAYBE TO GETOFF
          
          PushOp( SUB );
          ++ptr;
          break;
        }
        
        //----------------------------- Start of Parametric Interpretation Test
        if( (str[1] == 0) && (*str >= 'A') && (*str <= 'Z') ){

          arg.ptr = beh->GetInputParameter( *str-'A'+1 );

          CKParameterIn *pin;
          if( pin = (CKParameterIn*) arg.ptr ){
            
            CKGUID type = pin->GetGUID();
            
            if( (type==CKPGUID_INT) || (type==CKPGUID_FLOAT) || (type==CKPGUID_PERCENTAGE) ){
              arg.type = PARAM_FLOAT_PTR;
            
            } else if( type == CKPGUID_VECTOR){
              arg.type = PARAM_VECTOR_PTR;
            
            } else {
              return CREATION_ERROR;
            }
          }
          
          PushNode(arg);
          ++ptr;
          state = 1;
          break;
        }
        //---------------------------------------------------------------- End
        
        if( SUCCESS == strcmp(str, "PI") ){
          
          arg.type = FLOAT_VALUE;
          arg.v[0] = PI;
          PushNode(arg);
          
        } else {
          
          arg.type = FLOAT_VALUE;
          arg.v[0] = (float)strtod(str, &endptr);
          
          if( 0.0f==arg.v[0] && NULL==strchr(str, '0')) return CREATION_ERROR;
          
          PushNode(arg);
        }
        ptr += strlen(str);
        
      } else return CREATION_ERROR;
      
      state = 1;
      break;
      
    case 1: // Expecting Operation
      if( NULL != (op = GetOp(ptr)) ){
        if( ')' == *ptr ){
          
          if(SUCCESS > (ercode = DoParenthesis()))
            return ercode;
        } else {
          
          while( op->precedence <= PrecedenceOfLastPut() ){
						ercode = PushOpInNodeStack();
						if(ercode < SUCCESS) return ercode;
          }
          
          PushOp(op->token);

          if( (op->token!=XCOMP) && (op->token!=YCOMP) && (op->token!=ZCOMP) ) state = 0;
        }
        
        ptr += op->taglen;
        
      } else return CREATION_ERROR;
      
      break;
    }
  }
  
  while( op_sptr ){
    ercode = PushOpInNodeStack();
    if(ercode < SUCCESS) return ercode;
  }
  
  nts->nb_node = node_sptr;
  nts->node_stack = new Node[node_sptr];
  memcpy( nts->node_stack, node_stack, node_sptr*sizeof(Node) );
  
  return SUCCESS;
}


//______________________________________
//  Stack Operation in the Node Stack
//
int PushOpInNodeStack(void){
  
  Node node;
  
  if(CREATION_ERROR == PopOp( &(node.opid) ))
    return CREATION_ERROR;
  
  if( node.opid == OPEN_P ) // don't need to put operation '(' in node stack
    return node.opid;
  
  node.type = OP;
  PushNode( node );
  
  return node.opid;
  
}

//______________________________________
//  Evaluate one level
//
int DoParenthesis(void){
  int op;
  
  if(1 > parens--) return CREATION_ERROR;
  
  do{
    if( SUCCESS > (op = PushOpInNodeStack()) )break;
  }while( Precedence(op) );
  
  return op;
}

//______________________________________
//  Stack Operations
//
void PushOp(int op){
  if( !Precedence(op) ) parens++;
  op_stack[op_sptr++] = op;
}

void PushNode(Node arg){
  node_stack[node_sptr++] = arg;
}

int PopOp(int *op){
  if( !op_sptr )return CREATION_ERROR;
  *op = op_stack[--op_sptr];
  return SUCCESS;
}

//______________________________________
//  Get an expression
//
char *GetExpression(char *str){
  
  char *ptr = str, *tptr = token;
  Operation *op;
  
  if( SUCCESS == strncmp(str, "PI", 2))
    return strcpy(token, "PI");
  
  while( *ptr ){
    
    if( NULL != (op = GetOp( ptr )) ){
      
      if( '-' == *ptr ){
        
        if( str != ptr && ( ('E' != ptr[-1]) || (str==(ptr-1)) ) )
          break;

        if( str == ptr && !isdigit(ptr[1] ) && '.' != ptr[1] ){
          Node arg;
          arg.type = FLOAT_VALUE;
          arg.v[0] = 0.0f;
          PushNode(arg);
          strcpy(token, op->tag);
          return token;
        }
        
      } else if(str == ptr){
        
        strcpy(token, op->tag);
        return token;
        
      } else break;
    }
    
    *tptr++ = *ptr++;
  }
  *tptr = 0;
  
  return token;
}

//______________________________________
//  Get an Operation
//
Operation *GetOp(char *str){
  
  Operation *op;
  
  for (op = op_signification; op->token; ++op)
  {
    if(SUCCESS == strncmp(str, op->tag, op->taglen))
      return op;
  }
  return NULL;
}

//______________________________________
//  Get precedence of a token
//
int Precedence(int token){
  
  Operation *op;
  
  for (op = op_signification; op->token; ++op)
  {
    if(token == op->token)                                                     
      break;
  }
  if(op->token)
    return op->precedence;
  else  return 0;
}

//______________________________________
//  Get precedence of Last put token
//
int PrecedenceOfLastPut(void){
  
  if(!op_sptr)
    return 0;
  return Precedence( op_stack[op_sptr - 1] );
}



//______________________________________
//  Evaluate a Node
//
int EvalNode( float *val, int *type ){
  
  Node *cnode = &current_node_stack[ --node_sptr ]; // current node
  if(!cnode) return RUNTIME_ERROR;
  
  switch( cnode->type ){
    
  case FLOAT_VALUE:
    {
      *val = cnode->v[0];
      *type = FLOAT_VALUE;
      break;
    }
  case PARAM_FLOAT_PTR:
    {
      CKParameterIn *pin;
      if( pin = (CKParameterIn*) cnode->ptr ){
        pin->GetValue( val );
        *type = FLOAT_VALUE;
      }
      break;
    }
  case PARAM_VECTOR_PTR:
    {
      CKParameterIn *pin;
      if( pin = (CKParameterIn*) cnode->ptr ){
        pin->GetValue( val );
        *type = VECTOR_VALUE;
      }
      break;
    }
  case OP:
    {
      int op;
      if( (op = cnode->opid) > 0 && (op < A_CALULATOR_NB_OP) ){
        if( SUCCESS != CalculatorOpFct[op]( val, type ) ) return RUNTIME_ERROR;
        break;
      }
      return RUNTIME_ERROR;
    }
    
  default:
    return RUNTIME_ERROR;
  }
  
  return SUCCESS;
}
