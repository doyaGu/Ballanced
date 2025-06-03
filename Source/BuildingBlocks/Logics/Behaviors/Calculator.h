#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define SUCCESS          0
#define CREATION_ERROR  -1
#define RUNTIME_ERROR   -2

//---------- Operation Struct
typedef struct{
  int         token;
  char        *tag;
  size_t      taglen;
  int         precedence;
} Operation;

//---------- Node Struct
typedef struct{
  int type;              // Node type
  union{
    int opid;            // Operation ID (if node type is OP )
    void *ptr;           // ptr to param (if node type is PARAM_PTR)
    float v[3];          // Value of the node contained floats  (if node type is FLOAT)
  };
} Node;

typedef struct{
  int nb_node;
  Node *node_stack;
} Node_Total_Struct;


//---------- Node Types
enum{
  OP=1, FLOAT_VALUE, VECTOR_VALUE, PARAM_FLOAT_PTR, PARAM_VECTOR_PTR
};

//---------- Operation ID
//NEWOP : add function macro name
enum{
  ADD=1,    SUB,    MULT,     DIV,    MOD,
  POW,      OPEN_P, CLOSED_P, SIN,    COS,
  ABS,      LN,     EXP,      ATAN,   SQRT,
  ACOS,     ASIN,   XCOMP,    YCOMP,  ZCOMP,
  TAN
};

//NEWOP increment
#define A_CALULATOR_NB_OP 22
//---------- Operation Available List
//NEWOP : add operation description (class by frequency order)
Operation op_signification[] = {
  {ADD,       "+",       1, 2  },
  {SUB,       "-",       1, 3  },
  {MULT,      "*",       1, 4  },
  {DIV,       "/",       1, 5  },
  {OPEN_P,    "(",       1, 0  },
  {CLOSED_P,  ")",       1, 256},
  {SIN,       "SIN(",    4, 0  },
  {COS,       "COS(",    4, 0  },
  {TAN,       "TAN(",    4, 0  },
  {XCOMP,     ".X",      2, 7  },
  {YCOMP,     ".Y",      2, 7  },
  {ZCOMP,     ".Z",      2, 7  },
  {ABS,       "ABS(",    4, 0  },
  {SQRT,      "SQRT(",   5, 0  },
  {ASIN,      "ASIN(",   5, 0  },
  {ACOS,      "ACOS(",   5, 0  },
  {ATAN,      "ATAN(",   5, 0  },
  {POW,       "^",       1, 6  },
  {LN,        "LN(",     3, 0  },
  {EXP,       "EXP(",    4, 0  },
  {MOD,       "\\",      1, 5  },
  {0,         NULL,      0, 0  }
};

int    op_stack[256];                    /* Operation stack           */
Node   node_stack[256];                  /* Node stack                */
Node  *current_node_stack;               /* Current Node (+efficient) */
char   token[256];                       /* Token buffer              */
int    op_sptr,                          /* op_stack pointer          */
node_sptr,                               /* node_stack pointer        */
parens,                                  /* Nesting level             */
state;                                   /* 0 = Expecting Expression
                                            1 = Expecting Operation
*/
int              PushOpInNodeStack(void);
int              DoParenthesis(void);
void             PushOp(int);
void             PushNode(Node);
int              PopOp(int*);
char            *GetExpression(char*);
Operation       *GetOp(char*);
int              Precedence(int);
int              PrecedenceOfLastPut(void);
int              EvalNode(float *value, int *type);
int              CreateNodeStack(char*, Node_Total_Struct*, CKBehavior*);
char            *remove_space(char*);

