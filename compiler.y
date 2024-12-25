/* Please feel free to modify any content */

/* Definition section */
%{
    #include "compiler_common.h" //Extern variables that communicate with lex
    #define YYDEBUG 1
    int yydebug = 1;

    extern int yylineno;
    extern int yylex();
    extern FILE *yyin;

    int yylex_destroy ();
    void yyerror (char const *s)
    {
        printf("error:%d: %s\n", yylineno, s);
    }

    extern int yylineno;
    extern int yylex();
    extern FILE *yyin;



    /* Used to generate code */
    /* As printf; the usage: CODEGEN("%d - %s\n", 100, "Hello world"); */
    /* We do not enforce the use of this macro */
    #define CODEGEN(...) \
        do { \
            for (int i = 0; i < g_indent_cnt; i++) { \
                fprintf(fout, "\t"); \
            } \
            fprintf(fout, __VA_ARGS__); \
        } while (0)


    #define SETUPGEN(...) \
        do { \
            fprintf(fout, __VA_ARGS__); \
        } while (0)


    #define LABELGEN(...) \
        do { \
            fprintf(fout, __VA_ARGS__); \
        } while (0)


// from hw2
    typedef struct symbol_entry {
        int ret;
        int mut;
        int address;
        int lineno;
        char *name;
        char *type;
        char *func_sig;
        char *array_type;
        struct symbol_entry *next;
    } symbol_entry;

    typedef struct symbol_node {
        symbol_entry *head;
        struct symbol_node *prev;
        struct symbol_node *next;
    } symbol_node;

    /* Global variables */
    bool g_has_error = false;
    FILE *fout = NULL;
    int g_indent_cnt = 1;
    bool HAS_ERROR = false;
    int g_address = 0;
    symbol_entry *func = NULL;
    symbol_node *table = NULL;
    int g_break = 0;
    char *array_type = "";
    int g_array_size = 0;
    int g_lvl = -1;
    int g_lbl_idx = 0;


    static void create_symbol() {
        // plus g_lvl first
        g_lvl++;      // created

        // llo
        symbol_node *node =  malloc(sizeof(symbol_node));
        node->head = NULL;
        node->prev = node;
        node->next = node;
        if (table) {
            node->prev = table->prev;
            table->prev->next = node;
            node->next = table;
            table->prev = node;
        }
        else {
            table = node;
        }

        printf("> Create symbol table (scope level %d)\n", g_lvl);
    }

    static void insert_symbol(char *name, int mut, char *type, char *func_sig) {
        symbol_node *node = table;

        int n = 0;
        if(mut != -1){
            n = g_lvl;
        }
        
        // llo
        for (int i = 0; i < n; i++){
            node = node->next;
        }

        symbol_entry *ptr = node->head;
        symbol_entry *prev = NULL;

        while (ptr) {
            prev = ptr;
            ptr = ptr->next;
        }

        symbol_entry *entry = malloc(sizeof(symbol_entry));
        entry->ret = 0;
        entry->mut = mut;
        entry->name = name;
        entry->type = type;
        entry->lineno = yylineno;
        entry->func_sig = func_sig;
        entry->array_type = array_type;
        entry->next = NULL;

        if(prev){
            prev->next = entry;
        }
        else{
            node->head = entry;
        }


        if(mut != -1){
            entry->address = g_address++;
        }
        else{
            entry->address = -1;
        }


        if(mut == -1){
            func = entry;
        }

        printf("> Insert `%s` (addr: %d) to scope level %d\n", entry->name, entry->address, n);
    }

    static symbol_entry *lookup_symbol(char *name){
        symbol_node *node = table->prev;

        int i = 0;
        while(i <= g_lvl) {
            for (symbol_entry *entry = node->head; entry; entry = entry->next) {
                if (strcmp(entry->name, name) == 0) 
                    return entry;
            }
            node = node->prev;
            i++;
        }

        return NULL;
    }

    static void dump_symbol() {
        printf("\n> Dump symbol table (scope level: %d)\n", g_lvl);
        printf("%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n", "Index", "Name", "Mut", "Type", "Addr", "Lineno", "Func_sig");

        symbol_node *node = table;
        int i = 0;
        while(i++ < g_lvl) {
            node = node->next;
        }


        node->prev->next = node->next;
        node->next->prev = node->prev;


        int j = 0;
        for (symbol_entry *entry = node->head, *safe; entry; entry = safe) {
            safe = entry->next;
            printf("%-10d%-10s%-10d%-10s%-10d%-10d%-10s\n", j, entry->name, entry->mut, entry->type, entry->address, entry->lineno, entry->func_sig);
            j++;
        }
        node = NULL;
        g_lvl--;  // dumped
    }

// new
    static void cmp_gen(char *code, char *type) {
        if (strcmp(type, "i32") == 0){
            CODEGEN("isub\n");
        }
        else if (strcmp(type, "f32") == 0){
            CODEGEN("fcmpl\n");
        }

        CODEGEN("%s label_%d\n", code, g_lbl_idx);
        CODEGEN("iconst_0\n");
        CODEGEN("goto label_%d\n", g_lbl_idx + 1);
        LABELGEN("label_%d:\n", g_lbl_idx);
        CODEGEN("iconst_1\n");
        LABELGEN("label_%d:\n", g_lbl_idx + 1);

        g_lbl_idx += 2; 

    }


    static void type_gen(char *code, char *type) {
        if(strcmp(type, "i32") == 0){
            CODEGEN("i%s\n", code);
        }
        else if(strcmp(type, "f32") == 0){
            CODEGEN("f%s\n", code);
        }
        else if(strcmp(type, "bool") == 0){
            CODEGEN("i%s\n", code);
        }
        else if(strcmp(type, "string") == 0){
            if(strcmp(code, "const_0") == 0){
                CODEGEN("ldc \"\"\n");
            }
            else{
                CODEGEN("a%s\n", code);
            }
        }
    }


    static void addr_gen(char *code, char *type, int address) {
        if(strcmp(type, "i32") == 0){
            CODEGEN("i%s %d\n", code, address);
        }
        else if(strcmp(type, "f32") == 0){
            CODEGEN("f%s %d\n", code, address);
        }
        else if(strcmp(type, "bool") == 0){
            CODEGEN("i%s %d\n", code, address);
        }
        else if(strcmp(type, "str") == 0){
            CODEGEN("a%s %d\n", code, address);
        }
        else if(strcmp(type, "array") == 0){       // *
            CODEGEN("a%s %d\n", code, address);
        }
    }


    static void print_gen(char *code, char *type) {
        char *str;

        if(strcmp(type, "i32") == 0)
            str = "I";
        else if(strcmp(type, "f32") == 0)
            str = "F";
        else if(strcmp(type, "bool") == 0) {
            str = "Ljava/lang/String;";

            CODEGEN("ifne label_%d\n", g_lbl_idx);
            CODEGEN("ldc \"false\"\n");
            CODEGEN("goto label_%d\n", g_lbl_idx + 1);
            LABELGEN("label_%d:\n", g_lbl_idx);
            CODEGEN("ldc \"true\"\n");
            LABELGEN("label_%d:\n", g_lbl_idx + 1);

            g_lbl_idx += 2;
        }
        else if(strcmp(type, "str") == 0){
            str = "Ljava/lang/String;";
        }

        CODEGEN("getstatic java/lang/System/out Ljava/io/PrintStream;\n");
        CODEGEN("swap\n");
        CODEGEN("invokevirtual java/io/PrintStream/%s(%s)V\n", code, str);

    }


    int string_length(char *str){
        int i = 0;
        while(str[i] != '\0'){
            i++;
        }
        return i;
    }


    // put it to back
    char *func_get(char *list, char *return_type) {
        /* if(length == 0){
            list = malloc(1);
            length = 1;
            list[0] = 'V';
        } */
        int length = string_length(list) + string_length(return_type) + 3;
        char *str = malloc(length);

        str[0] = '\0';
        strcat(str, "(");       // use it for concatenation
        strcat(str, list);
        strcat(str, ")");
        strcat(str, return_type);
        str[length - 1] = '\0';

        return str;
    }


    char *concatenate(char *str, char *type_string) {
        char *str2 = malloc(strlen(str) + 2);
        int i = 0;
        while (i++ < strlen(str)) {
            str2[i] = str[i];
        }

        str2[i] = type_string[0];
        i++;
        str2[i] = '\0';

        return str2;
    }

    static char *type_get(char *type){
        char *str = NULL;

        if(strcmp(type, "i32") == 0){
            str = "I";
        }
        else if(strcmp(type, "f32") == 0){
            str = "F";
        }
        else if(strcmp(type, "bool") == 0){
            str = "I";
        }
        else if(strcmp(type, "string") == 0){
            str = "Ljava/lang/String;";
        }

        return str;
    }

    static char *list_get(char *par, char *type){
        char *str = malloc(strlen(par) + strlen(type) + 1);

        str[0] = '\0';
        strcat(str, par);     // use for concatenation
        strcat(str, type);

        str[strlen(par) + strlen(type)] = '\0';

        return str;
    }
%}

%error-verbose

/* Use variable or self-defined structure to represent
 * nonterminal and token type
 *  - you can add new fields if needed.
 */

%union {
    int i_val;
    float f_val;
    char *s_val;
    bool b_val;     //
}

/* Token without return  */
%token LET MUT ARROW AS IN DOTDOT TRUE FALSE RSHIFT LSHIFT FUNC RETURN STR GEQ LEQ EQL NEQ LOR LAND IF ELSE FOR WHILE LOOP BREAK PRINT PRINTLN
%left LOR
%left LAND GEQ LEQ EQL NEQ 
%left '>' '<' '+' '-'
%left '*' '/' '%' 

/* Token with return, which need to sepcify type  */
%token <i_val> INT_LIT, <f_val> FLOAT_LIT, <s_val> STRING_LIT ID INT FLOAT BOOL ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN REM_ASSIGN '+' '-' '=' '!'

/* Nonterminal with return, which need to sepcify type  */
%type <i_val> Loop If_Jump For_Loop For_Label While While_Jump, <s_val> Type Function_Open Function_Declaration Function_Sig Operand Assign_Op Unary_Op Ident Literal Expression Parameter_List Index_Expression Unary_Expression Primary_Expression Cast_Expression Call_Expression, <b_val> Boolean_Literal

/* Yacc will start at this nonterminal */
%start Program

/* Grammar section */
%%

Program
    : GlobalStatementList
;

GlobalStatementList 
    : GlobalStatementList GlobalStatement
    | GlobalStatement
;

GlobalStatement
    : Function_DeclarationStmt
;

Function_DeclarationStmt
    : Function_Declaration FuncBlock{   
                                        if(func->ret == 0 && func->func_sig[3] != 'V') {
                                            int i = 0;
                                            while(func->func_sig[i] != 0)
                                                i++;
                                            printf("%creturn\n", func->func_sig[i-1] | 32);   
                                        }
                                        g_address = 0;
                                        dump_symbol(); 

                                        CODEGEN("return\n");
                                        SETUPGEN(".end method\n");
                                    }
;

Function_Declaration
    : Function_Open Function_Sig {
                                    func->ret = 0;
                                    func->func_sig = $2;      
                                    $$ = $2;  
                                    
                                    g_address = 0;      // *
                                    char *signature;
                                    if(strcmp($1, "main") == 0){
                                        signature = "([Ljava/lang/String;)V";
                                    }
                                    else{
                                        signature = $2;
                                    }

                                    SETUPGEN(".method public static %s%s\n", $1, signature);
                                    SETUPGEN(".limit stack 100\n");
                                    SETUPGEN(".limit locals 100\n");

                                 }
;

FuncBlock
    : '{' StatementList '}'
    | '{' '}' 
;

Function_Sig
    : '(' ')'                                   { $$ = func_get(strdup(""), "V"); }
    | '(' ')' ARROW Type                        { $$ = func_get(strdup(""), type_get($4)); }
    | '(' Parameter_List ')'                    { $$ = func_get($2, "V"); }
    | '(' Parameter_List ')' ARROW Type         { $$ = func_get($2, type_get($5)); }
;

Function_Open
    : FUNC ID   {
                    $$ = $2;
                    printf("func: %s\n", $2);
                    insert_symbol($2, -1, "func", "");                                               
                    create_symbol();
                }
;

Type
    : INT   { $$ = "i32"; }
    | FLOAT { $$ = "f32"; }
    | BOOL  { $$ = "bool"; }
    | STR   { $$ = "str"; }
    | '&' Type { $$ = $2; }
    | '[' Type ';' Expression ']'   { 
                                        $$ = "array"; 
                                        array_type = $2;
                                        CODEGEN("dup\n");

                                        char *type;
                                        if(strcmp(array_type, "i32") == 0){
                                            type = "int";
                                        }
                                        else if (strcmp(array_type, "f32") == 0){
                                            type = "float";
                                        }

                                        CODEGEN("newarray %s\n", type);
                                    }
;

Dotdot
    : DOTDOT { printf("DOTDOT\n"); }
;

Operand
    : Ident
    | Literal
    | '(' Expression ')' { $$ = $2; }
;

Unary_Op
    : '-'   { $$ = "NEG"; } 
    | '!'   { $$ = "NOT"; }
;

Assign_Op
    : '='           { $$ = "ASSIGN"; }
    | ADD_ASSIGN    { $$ = "add"; }
    | SUB_ASSIGN    { $$ = "sub"; }
    | MUL_ASSIGN    { $$ = "mul"; }
    | DIV_ASSIGN    { $$ = "div"; }
    | REM_ASSIGN    { $$ = "rem"; }
;

Ident
    : ID    { 
                symbol_entry *entry = lookup_symbol($1);

                if (entry) {
                    $$ = entry->type;

                    if(strcmp(entry->type, "array") == 0){
                        $$ = entry->array_type;
                    }

                    printf("IDENT (name=%s, address=%d)\n", entry->name, entry->address);
                }
                else {
                    $$ = "undefined";
                    printf("error:%d: undefined: %s\n", yylineno, $1);
                }

                addr_gen("load", entry->type, entry->address);
            }    
;

EnterBlock
    : '{' { create_symbol(); }
;

ExitBlock
    : '}'   { 
                func->ret = 0; 
                dump_symbol(); 
            }
;

Block
    : EnterBlock StatementList ExitBlock
    | EnterBlock ExitBlock
;

StatementList
    : StatementList Statement
    | Statement
;

Statement
    : DeclarationStmt ';'
    | SimpleStmt ';'
    | ReturnStmt ';'
    | PrintStmt ';'
    | ';'
    | SimpleStmt
    | IfStmt
    | LoopExpression
    | Block
;

SimpleStmt
    : AssignmentStmt
    | ExpressionStmt
;

ExpressionStmt
    : Expression
;

SimpleIfStmt
    : If Condition If_Jump Block { LABELGEN("label_%d:\n", $3); }
;

IfStmt
    : SimpleIfStmt Else Block
    | SimpleIfStmt Else IfStmt
    | SimpleIfStmt
;

If
    : IF
;

If_Jump
    :   {
            CODEGEN("ifeq label_%d\n", g_lbl_idx);
            $$ = g_lbl_idx++;
        }
;

Else
    : ELSE
;

Condition
    : Expression 
;

AssignmentStmt
    : ID Assign_Op Expression   {
                                    symbol_entry *entry = lookup_symbol($1);

                                    if (!entry) {
                                        printf("error:%d: undefined: %s\n", yylineno, $1);
                                    }
                                    else{
                                        printf("%s\n", $2); 
                                    }

                                    if(strcmp($2, "ASSIGN")){
                                        addr_gen("load", entry->type, entry->address);
                                        CODEGEN("swap\n");
                                        type_gen($2, entry->type);
                                    }

                                    addr_gen("store", entry->type, entry->address);
                                }

;

DeclarationStmt
    : LET ID ':' Type '=' Expression{   
                                        insert_symbol($2, 0, $4, "-");
                                        symbol_entry *entry = lookup_symbol($2);
                                        addr_gen("store", entry->type, entry->address);
                                    }
    | LET MUT ID ':' Type '=' Expression {   
                                        insert_symbol($3, 1, $5, "-");
                                        symbol_entry *entry = lookup_symbol($3);
                                        addr_gen("store", entry->type, entry->address);
                                    }     
    | LET ID  '=' Expression        {
                                        insert_symbol($2, 0, $4, "-");
                                        symbol_entry *entry = lookup_symbol($2);
                                        addr_gen("store", entry->type, entry->address);
                                    }
    | LET MUT ID  '=' Expression    {   
                                        insert_symbol($3, 1, $5, "-");
                                        symbol_entry *entry = lookup_symbol($3);
                                        addr_gen("store", entry->type, entry->address);
                                    }                                                                   
    | LET ID ':' Type               { 
                                        insert_symbol($2, 0, $4, "-");
                                        symbol_entry *entry = lookup_symbol($2);
                                        type_gen("const_0", entry->type);
                                        addr_gen("store", entry->type, entry->address);
                                    }
    | LET MUT ID ':' Type           { 
                                        insert_symbol($3, 1, $5, "-");
                                        symbol_entry *entry = lookup_symbol($3);
                                        type_gen("const_0", entry->type);
                                        addr_gen("store", entry->type, entry->address);
                                    }
;

ReturnStmt
    : RETURN    { 
                    printf("return\n"); 
                    CODEGEN("return\n");    
                }
    | RETURN Expression { 
                            printf("%creturn\n", $2[0]); 
                            func->ret = 1;
                            type_gen("return", $2);
                        }
;

PrintStmt
    : PRINT '(' Expression ')'  { 
                                    printf("PRINT %s\n", $3); 
                                    print_gen("print", $3);
                                }
    | PRINTLN '(' Expression ')'{ 
                                    printf("PRINTLN %s\n", $3);
                                    print_gen("println", $3);
                                }
;

Unary_Expression
    : Unary_Op Unary_Expression { 
                                    $$ = $2;
                                    printf("%s\n", $1);

                                    if(strcmp($1, "NOT") == 0){
                                        CODEGEN("iconst_1\n");
                                        CODEGEN("ixor\n");
                                    }
                                    else if(strcmp($1, "NEG") == 0){
                                        CODEGEN("%cneg\n", $2[0]);
                                    }
                                }
    | Primary_Expression
;

BreakExpression
    : BREAK
    | BREAK Expression
;

ArrayElements 
    : Expression    {
                        CODEGEN("iastore\n");
                        CODEGEN("iconst_1\n");
                        CODEGEN("iadd\n");
                        CODEGEN("dup2\n");
                    }
    | ArrayElements ',' Expression {
                                        CODEGEN("iastore\n");
                                        CODEGEN("iconst_1\n");
                                        CODEGEN("iadd\n");
                                        CODEGEN("dup2\n");
                                    }
;

ArrayExpression 
    : '[' ']'
    | BeginArr ArrayElements ']' {
                                    CODEGEN("pop\n");
                                }
;

BeginArr            
    : '['   { 
                CODEGEN("iconst_0\n");
                CODEGEN("dup2\n");
            }
;

For_Label
    : FOR ID IN ID '{' {
                            symbol_entry *entry = lookup_symbol($4);

                            if(entry){
                                printf("IDENT (name=%s, address=%d)\n", entry->name, entry->address);
                                create_symbol();                                            
                                insert_symbol($2, 0, entry->array_type, "-");
                                
                                $$ = g_lbl_idx;
                                symbol_entry *for_entry = lookup_symbol($2);

                                addr_gen("load", entry->type, entry->address);
                                CODEGEN("arraylength\n");
                                CODEGEN("iconst_1\n");
                                CODEGEN("isub\n");
                                CODEGEN("iconst_0\n");
                                CODEGEN("iconst_1\n");
                                CODEGEN("isub\n");
                                LABELGEN("label_%d:\n", g_lbl_idx++);
                                CODEGEN("iconst_1\n");
                                CODEGEN("iadd\n");
                                CODEGEN("dup\n");
                                addr_gen("load", entry->type, entry->address);
                                CODEGEN("swap\n");
                                CODEGEN("iaload\n");
                                addr_gen("store", for_entry->type, for_entry->address);
                            }
                            else {
                                printf("error:%d: undefined: %s\n", yylineno, $4);
                                create_symbol();
                                insert_symbol($2, 0, "undefined", "-");
                            }
                        }
;

For_Loop
    : For_Label ExitBlock
    | For_Label StatementList ExitBlock
;

LoopExpression
   : WhileStmt
   | Loop Block {
                    CODEGEN("goto label_%d\n", $1);
                    LABELGEN("label_%d:\n", g_lbl_idx++);
                }
   | For_Loop   {
                    CODEGEN("dup2\n");
                    CODEGEN("if_icmpne label_%d\n", $1);
                    CODEGEN("pop2\n");
                }
;

Loop
    :LOOP   {
                LABELGEN("label_%d:\n", g_lbl_idx);
                $$ = g_lbl_idx++;       
            }

WhileStmt
    : While Condition While_Jump Block {
                                            CODEGEN("goto label_%d\n", $1);
                                            LABELGEN("label_%d:\n", $3);
                                        }
;

While
    : WHILE { 
                LABELGEN("label_%d:\n", g_lbl_idx);
                $$ = g_lbl_idx++; 
            }
;

While_Jump
    :   { 
            CODEGEN("ifeq label_%d\n", g_lbl_idx);
            $$ = g_lbl_idx++; 
        }
;

Expression 
    : Unary_Expression
    | BreakExpression   {
                            CODEGEN("goto label_%d\n", g_lbl_idx);

                        }
    | ArrayExpression
    | LoopExpression
    | Expression LOR Expression {
                                    $$ = "bool";
                                    printf("LOR\n");
                                    CODEGEN("ior\n");
                                }
    | Expression LAND Expression{ 
                                    $$ = "bool";
                                    printf("LAND\n");
                                    CODEGEN("iand\n");
                                }    
    | Expression '+' Expression {
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "ADD", $1, $3);
                                    }
                                    printf("ADD\n");
                                    CODEGEN("%cadd\n", $1[0]);
                                }
    | Expression '-' Expression {
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "SUB", $1, $3);
                                    }
                                    printf("SUB\n");
                                    CODEGEN("%csub\n", $1[0]);
                                }
    | Expression '*' Expression {
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "MUL", $1, $3);
                                    }
                                    printf("MUL\n");
                                    CODEGEN("%cmul\n", $1[0]);
                                }
    | Expression '/' Expression {
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "DIV", $1, $3);
                                    }
                                    printf("DIV\n");
                                    CODEGEN("%cdiv\n", $1[0]);
                                }
    | Expression '%' Expression {
                                    $$ = "i32";
                                    printf("REM\n");   
                                    CODEGEN("irem\n");
                                }
    | Expression '>' Expression {
                                    $$ = "bool";
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "GTR", $1, $3);
                                    }
                                    printf("GTR\n");
                                    cmp_gen("ifgt", $1);
                                }
    | Expression '<' Expression {
                                    $$ = "bool";
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "LSS", $1, $3);
                                    }
                                    printf("LSS\n");
                                    cmp_gen("iflt", $1);
                                }
    | Expression GEQ Expression {
                                    $$ = "bool";
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "GEQ", $1, $3);
                                    }
                                    printf("GEQ\n");
                                    cmp_gen("ifge", $1);
                                }
    | Expression LEQ Expression {
                                    $$ = "bool";
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "LEQ", $1, $3);
                                    }
                                    printf("LEQ\n");
                                    cmp_gen("ifle", $1);
                                }
    | Expression EQL Expression {
                                    $$ = "bool";
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "EQL", $1, $3);
                                    }
                                    printf("EQL\n");
                                    cmp_gen("ifeq", $1);
                                }
    | Expression NEQ Expression {
                                    $$ = "bool";
                                    if (strcmp($1, $3) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "NEQ", $1, $3);
                                    }
                                    printf("NEQ\n");
                                    cmp_gen("ifne", $1);
                                }
    | Expression LSHIFT Expression  {
                                        if (strcmp($1, $3) != 0){
                                            printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "LSHIFT", $1, $3);
                                        }
                                        printf("LSHIFT\n");
                                        CODEGEN("ishl\n");
                                    }
    | Expression RSHIFT Expression  {
                                        if (strcmp($1, $3) != 0){
                                            printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "RSHIFT", $1, $3);
                                        }
                                        printf("RSHIFT\n");
                                        CODEGEN("iushr\n");
                                    }
;

ArgumentList
    : Expression
    | ArgumentList ',' Expression
;

Call_Expression
    : ID '(' ')'
    | ID '(' ArgumentList ')'
;

Cast_Expression
    : Primary_Expression AS Type    { 
                                        $$ = $3; printf("%c2%c\n", $1[0], $3[0]); 
                                        CODEGEN("%c2%c\n", $1[0], $3[0]);
                                    }
;

Index_Expression
    : Ident '[' Expression ']'  { 
                                    CODEGEN("iaload\n");
    
                                }
;

Primary_Expression
    : Operand
    | Cast_Expression
    | Index_Expression 
    | Call_Expression   {   
                            symbol_entry *entry = lookup_symbol($1);

                            $$ = entry->type;
                            printf("call: %s%s\n", entry->name, entry->func_sig);
                        
                            CODEGEN("invokestatic Main/%s%s\n", entry->name, entry->func_sig);
                        }
    | '&' Ident '[' Expression Dotdot  ']'  { 
                                                $$ = $2; 
                                                CODEGEN("invokevirtual java/lang/String/substring(I)Ljava/lang/String;\n");
                                            }
    | '&' Ident '[' Dotdot Expression ']'   { 
                                                $$ = $2;
                                                CODEGEN("iconst_0\n");
                                                CODEGEN("swap\n");
                                                CODEGEN("invokevirtual java/lang/String/substring(II)Ljava/lang/String;\n"); 
                                            }                     
    | '&' Ident '[' Expression Dotdot Expression ']'    { 
                                                            $$ = $2;
                                                            CODEGEN("invokevirtual java/lang/String/substring(II)Ljava/lang/String;\n");
                                                        }
;

/* Slice
    : '['   {
                CODEGEN("getstatic java/lang/String/substring(II)Ljava/lang/String\n");
            }
;

SliceSlice
    : '['   {
                CODEGEN("getstatic java/lang/String/substring(I)Ljava/lang/String\n");
            }
; */

Literal
    : INT_LIT   { 
                    $$ = "i32";
                    printf("INT_LIT %d\n", $1); 
                    CODEGEN("ldc %d\n", yylval.i_val);
                }
    | FLOAT_LIT {
                    $$ = "f32";
                    printf("FLOAT_LIT %f\n", $1); 
                    CODEGEN("ldc %f\n", yylval.f_val);
                }
    | '"' STRING_LIT '"'    { 
                                $$ = "str";
                                printf("STRING_LIT \"%s\"\n", $2);    
                                CODEGEN("ldc \"%s\"\n", yylval.s_val);
                            }                    
    | Boolean_Literal   {
                            $$ = "bool";                            
                            CODEGEN("ldc %d\n", $1);
                        }
    | '"' '"'   {
                    $$ = "str";
                    printf("STRING_LIT \"\"\n"); 
                    CODEGEN("ldc \"\"\n");
                }
;

Boolean_Literal
    : TRUE  {   
                $$ = 1; 
                printf("bool TRUE\n"); 
            }
    | FALSE { 
                $$ = 0; 
                printf("bool FALSE\n"); 
            }
;

Parameter_List
    : ID ':' Type   { 
                        char *type = type_get($3);
                        $$ = strdup(type);

                        printf("param %s, type: %s\n", $1, type);
                        insert_symbol($1, 0, $3, "-");
                    }
    | MUT ID ':' Type   { 
                            char *type = type_get($4);
                            $$ = strdup(type);

                            printf("param %s, type: %s\n", $2, type);
                            insert_symbol($2, 1, $4, "-");
                        }                    
    | Parameter_List ',' ID ':' Type    { 
                                            char *type = type_get($5);
                                            $$ = list_get($1, type);

                                            printf("param %s, type: %s\n", $3, type);
                                            insert_symbol($3, 0, $5, "-");
                                        }
    | Parameter_List ',' MUT ID ':' Type    { 
                                                char *type = type_get($6);
                                                $$ = list_get($1, type);

                                                printf("param %s, type: %s\n", $4, type);
                                                insert_symbol($4, 1, $6, "-");
                                            }
;

%%

/* C code section */
int main(int argc, char *argv[])
{
    if (argc == 2) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }
    if (!yyin) {
        printf("file `%s` doesn't exists or cannot be opened\n", argv[1]);
        exit(1);
    }

    /* Codegen output init */
    char *bytecode_filename = "hw3.j";
    fout = fopen(bytecode_filename, "w");
    SETUPGEN(".source hw3.j\n");
    SETUPGEN(".class public Main\n");
    SETUPGEN(".super java/lang/Object\n");

    /* Symbol table init */
    // Add your code

    create_symbol();    // step1

    yylineno = 1;       // changed
    yyparse();          // step2

    dump_symbol();      // step3

    yylineno--;         // step4

    /* Symbol table dump */
    // Add your code

	printf("Total lines: %d\n", yylineno);
    fclose(fout);
    fclose(yyin);

    if (g_has_error) {
        remove(bytecode_filename);
    }
    yylex_destroy();
    return 0;
}


