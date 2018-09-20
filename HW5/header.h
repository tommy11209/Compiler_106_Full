#ifndef _HEADER_H_
#define _HEADER_H_

#define HASHBUNCH 23

typedef enum { __FALSE, __TRUE } __BOOLEAN;
typedef enum { VOID_t, INTEGER_t, BOOLEAN_t, STRING_t, REAL_t, ARRAY_t, PROGRAM_t, FUNCTION_t, \
VARIABLE_t, PARAMETER_t, CONSTANT_t, ERROR_t, ID_LIST, LOOPVAR_t } SEMTYPE;
typedef enum { ADD_t, SUB_t, MUL_t, DIV_t, MOD_t, LT_t, LE_t, EQ_t, GE_t, GT_t, NE_t, AND_t, OR_t, NOT_t, NONE_t } OPERATOR;




struct ArrayDimNode {
	int low;
	int high;
	int size;
	struct ArrayDimNode *next;
};

struct PType {
	__BOOLEAN isError;
	__BOOLEAN isArray;		/* array or scalar*/
	SEMTYPE type;
	int dimNum;
	struct ArrayDimNode *dim;	/* # of dim, and size of each dimension */
};

struct ConstAttr {
	SEMTYPE category;
	union {
		int integerVal;
		float realVal;
		__BOOLEAN booleanVal;
		char *stringVal;
	} value;

	__BOOLEAN hasMinus;
};

struct PTypeList {
	struct PType *value;
	struct PTypeList* next;
};

// FuncAttr: formal parameter lists for function
struct FuncAttr {
	int paramNum;
	struct PTypeList *params;
};

struct idNode_sem {
	char *value;
	struct idNode_sem *next;
};

struct param_sem {
	struct idNode_sem *idlist;
	struct PType *pType;

	struct param_sem *next;
};

struct typeNode {
	SEMTYPE value;			// INTEGER_t, BOOLEAN_t, REAL_t, STRING_t, ERROR_t
	struct typeNode *next;
};

struct var_ref_sem {
	char *id;
	int dimNum;
	struct typeNode *dim;
};

struct expr_sem {
	OPERATOR beginningOp;
	//__BOOLEAN beginWithUnaryOp;		// __TRUE: ......
	__BOOLEAN isDeref;		// __TRUE: has been deferenced, examine `type` field
	struct var_ref_sem *varRef;
	struct PType *pType;

	struct expr_sem *next;
};

union SymAttr {
	struct ConstAttr *constVal;
	struct FuncAttr *formalParam;
};
// structure for symbol table
struct SymNode {
	char *name;
	int scope;
    int symLocalNum;
	SEMTYPE category;
	struct PType *type;
	union SymAttr *attribute;

	struct SymNode *next;
	struct SymNode *prev;
};

struct SymTable {
	struct SymNode *entry[HASHBUNCH];

	int loopVarDepth;
	struct SymNode *loopVar;
};

void GenProgramStart(char* pname);
void GenProgramEnd(int check);
void GenMethod(char* pname , int check);
void GenGlobalVar(char* pname,struct PType* type);
void printlcd_int(int t);
void printlcd_f(float t);
void printlcd_str(char *s);
void printlcd_true();
void printlcd_false();
void print_load(char *id);
void Control_read_stmt(char *id);
void GenFunctionStart(struct PType* ret);
void GenFunctionEnd(struct PType* ret);
void GenFuncRutern(struct expr_sem* ret);
void GenFunctionStart_param(struct param_sem *parPtr);
void GenSaveExpr(struct expr_sem* expr,struct expr_sem* RHS);
void GenPrint(struct expr_sem* expr);
void GenFunctionCall(char* id);
void GenForLoop(char* id,int start,int end);
void GenForLoopEnd(char* id);
void GenToList();
void GenRelational( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2);
void GenArithmetic( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2);
void GenNegative(char *id);


#endif
