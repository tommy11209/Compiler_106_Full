extern char *yytext;		/* declared by lex */
extern int linenum;		/* declared in lex.l */
extern int Opt_D;		/* declared in lex.l */
extern int loop_cnt;
extern char* fn;
extern char* temp_fn;

typedef struct Array_Signl Array_Signl;
typedef struct Type Type;
typedef struct Type_List Type_List;
typedef struct TableEntry TableEntry;
typedef struct SymbolTable SymbolTable;
typedef struct Id_List Id_List;
typedef struct Value Value;
typedef struct Attribute Attribute;
typedef struct Expr Expr;
typedef struct Expr_List Expr_List;

extern SymbolTable* symbol_table;
struct SymbolTable {
	int current_level;
	int position;
	int capacity;
	TableEntry** Entries;
} ;

struct TableEntry {
	char name[32];
	char kind[20];
	int level;
	Type* type;
	Attribute* attri;

};

struct Array_Signl{
	int capacity;
	Array_Signl* next_dimension;
};

struct Attribute{
	Value* val;
	Type_List* type_list;
};

struct Type{
	char name[32];
	Array_Signl* Array_Signlature;
};

struct Type_List{
	int current_size;
	int capacity;
	Type** types;
};

struct Expr{
	char kind[16]; //var,func,const
	char name[32];
	Type* type;
	TableEntry* entry;
	int current_size;
	Type_List* para;
};

struct Expr_List{
	int current_size;
	int capacity;
	Expr** exprs;
};

struct Id_List{
	int position;
	int capacity;
	char** Ids;
};

struct Value{
	Type* type;
	int int_value;
	double double_value;
	char* string_value;
};

SymbolTable* buildtable();
TableEntry* buildentry(char*,const char*,int,Type*,Attribute*);
Type* buildtype(const char*);
Id_List* buildidlist();
Expr_List* buildexprlist(Expr_List*,Expr*);
Value* buildscientific(const char* );
Value* buildoctal(const char* );
Value* buildboolean(const char* );
Value* buildint(const char* );
Value* buildreal(const char* );
Value* buildstring(const char* );

void insert_entry(SymbolTable*,TableEntry*);
void insert_entry_list(SymbolTable*,Id_List*,const char*,Type*,Attribute*);
void pop_entry(SymbolTable*);
void pop_entry_name(SymbolTable*,char*);

void print_table(SymbolTable*);
char* return_type_buffer(const Type*,int);
Type_List* add_typelist(Type_List*,Type*,int);

Expr* find_var_ref(SymbolTable*,char*);
Expr* call_function(char*,Expr_List*);

Expr* relation_operation(Expr*,char*,Expr*);
Expr* multi_operation(Expr*,char*,Expr*);
Expr* add_operation(Expr*,char*,Expr*);
Expr* bool_operation(Expr*,char*,Expr*);
Value* sub_operation(Value*);

TableEntry* find_in_scope(SymbolTable*,char*);
TableEntry* find_in_global(SymbolTable*,char*);
TableEntry* find_in_loop(SymbolTable*,char*);

int const_assign(Expr*);
int function_parameter(Expr*);
