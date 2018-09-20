extern char *yytext;		/* declared by lex */
extern int linenum;		/* declared in lex.l */
extern int Opt_D;		/* declared in lex.l */
extern int loop_cnt;
extern char* fn;

typedef struct Array_signal Array_signal;
typedef struct Type Type;
typedef struct TypeList TypeList;
typedef struct TableEntry TableEntry;
typedef struct SymbolTable SymbolTable;
typedef struct IdList IdList;
typedef struct Value Value;
typedef struct Attribute Attribute;
typedef struct Expr Expr;
typedef struct ExprList ExprList;

extern SymbolTable* symbol_table;
struct SymbolTable {
	int current_level;
	int position;
	int size_;
	TableEntry** Entries;
} ;

struct TableEntry {
	char name[32];
	char kind[20];
	int level;
	Type* type;
	Attribute* attri;

};

struct Array_signal{
	int size_;
	Array_signal* next_dimension;
};

struct Attribute{
	Value* val;
	TypeList* type_list;
};

struct Type{
	char name[32];
	Array_signal* array_signature;
};

struct TypeList{
	int current_size;
	int size_;
	Type** types;
};

struct Expr{
	char kind[16]; 
	char name[32];
	Type* type;
	TableEntry* entry;
	int current_dimension;
	TypeList* para;
};

struct ExprList{
	int current_size;
	int size_;
	Expr** exprs;
};

struct IdList{
	int position;
	int size_;
	char** Ids;
};

struct Value{
	Type* type;
	int int_value;
	double double_value;
	char* string_value;
};
////////////////////////////////////////////////////////
void insert_entry(SymbolTable*,TableEntry*);
void insert_entry_list(SymbolTable*,IdList*,const char*,Type*,Attribute*);
TableEntry* build_entry(char*,const char*,int,Type*,Attribute*);
TypeList* add_type_list(TypeList*,Type*,int);
TableEntry* find_in_scope(SymbolTable*,char*);
TableEntry* find_in_global(SymbolTable*,char*);
TableEntry* find_in_loop(SymbolTable*,char*);
Expr* find_varref(SymbolTable*,char*);
Expr* call_func(char*,ExprList*);
////////////////////////////////////////////////////
void pop_entry(SymbolTable*);
void pop_entry_name(SymbolTable*,char*);
////////////////////////////////////////////////////
void PrintSymbolTable(SymbolTable*);
char* print_type(const Type*,int);
/////////////////////////////////////////////////////
SymbolTable* BuildSymbolTable();
IdList* BuildIdList();
Type* BuildType(const char*);
ExprList* BuildExprList(ExprList*,Expr*);
Value* buildscientific(const char* );
Value* buildoctal(const char* );
Value* buildboolean(const char* );
Value* buildint(const char* );
Value* buildreal(const char* );
Value* buildstring(const char* );
//////////////////////////////////////////////////////////
