/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 4 "compiler.y"

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

#line 433 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LET = 258,
    MUT = 259,
    ARROW = 260,
    AS = 261,
    IN = 262,
    DOTDOT = 263,
    TRUE = 264,
    FALSE = 265,
    RSHIFT = 266,
    LSHIFT = 267,
    FUNC = 268,
    RETURN = 269,
    STR = 270,
    GEQ = 271,
    LEQ = 272,
    EQL = 273,
    NEQ = 274,
    LOR = 275,
    LAND = 276,
    IF = 277,
    ELSE = 278,
    FOR = 279,
    WHILE = 280,
    LOOP = 281,
    BREAK = 282,
    PRINT = 283,
    PRINTLN = 284,
    INT_LIT = 285,
    FLOAT_LIT = 286,
    STRING_LIT = 287,
    ID = 288,
    INT = 289,
    FLOAT = 290,
    BOOL = 291,
    ADD_ASSIGN = 292,
    SUB_ASSIGN = 293,
    MUL_ASSIGN = 294,
    DIV_ASSIGN = 295,
    REM_ASSIGN = 296
  };
#endif
/* Tokens.  */
#define LET 258
#define MUT 259
#define ARROW 260
#define AS 261
#define IN 262
#define DOTDOT 263
#define TRUE 264
#define FALSE 265
#define RSHIFT 266
#define LSHIFT 267
#define FUNC 268
#define RETURN 269
#define STR 270
#define GEQ 271
#define LEQ 272
#define EQL 273
#define NEQ 274
#define LOR 275
#define LAND 276
#define IF 277
#define ELSE 278
#define FOR 279
#define WHILE 280
#define LOOP 281
#define BREAK 282
#define PRINT 283
#define PRINTLN 284
#define INT_LIT 285
#define FLOAT_LIT 286
#define STRING_LIT 287
#define ID 288
#define INT 289
#define FLOAT 290
#define BOOL 291
#define ADD_ASSIGN 292
#define SUB_ASSIGN 293
#define MUL_ASSIGN 294
#define DIV_ASSIGN 295
#define REM_ASSIGN 296

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 374 "compiler.y"

    int i_val;
    float f_val;
    char *s_val;
    bool b_val;     //

#line 574 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   742

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  232

#define YYUNDEFTOK  2
#define YYMAXUTOK   296


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,    61,     2,     2,    36,    55,     2,
      53,    54,    34,    32,    60,    33,     2,    35,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    57,
      31,    49,    30,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    56,     2,    58,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    51,     2,    52,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   401,   401,   405,   406,   410,   414,   430,   452,   453,
     457,   458,   459,   460,   464,   473,   474,   475,   476,   477,
     478,   496,   500,   501,   502,   506,   507,   511,   512,   513,
     514,   515,   516,   520,   542,   546,   553,   554,   558,   559,
     563,   564,   565,   566,   567,   568,   569,   570,   571,   575,
     576,   580,   584,   588,   589,   590,   594,   598,   605,   609,
     613,   635,   640,   645,   650,   655,   661,   670,   674,   682,
     686,   693,   705,   709,   710,   714,   720,   729,   730,   736,
     743,   779,   780,   784,   785,   789,   797,   803,   810,   817,
     824,   825,   829,   830,   831,   836,   841,   848,   855,   862,
     869,   874,   882,   890,   898,   906,   914,   922,   929,   939,
     940,   944,   945,   949,   956,   963,   964,   965,   966,   974,
     978,   984,  1003,  1008,  1013,  1018,  1022,  1030,  1034,  1041,
    1048,  1055,  1062
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LET", "MUT", "ARROW", "AS", "IN",
  "DOTDOT", "TRUE", "FALSE", "RSHIFT", "LSHIFT", "FUNC", "RETURN", "STR",
  "GEQ", "LEQ", "EQL", "NEQ", "LOR", "LAND", "IF", "ELSE", "FOR", "WHILE",
  "LOOP", "BREAK", "PRINT", "PRINTLN", "'>'", "'<'", "'+'", "'-'", "'*'",
  "'/'", "'%'", "INT_LIT", "FLOAT_LIT", "STRING_LIT", "ID", "INT", "FLOAT",
  "BOOL", "ADD_ASSIGN", "SUB_ASSIGN", "MUL_ASSIGN", "DIV_ASSIGN",
  "REM_ASSIGN", "'='", "'!'", "'{'", "'}'", "'('", "')'", "'&'", "'['",
  "';'", "']'", "':'", "','", "'\"'", "$accept", "Program",
  "GlobalStatementList", "GlobalStatement", "Function_DeclarationStmt",
  "Function_Declaration", "FuncBlock", "Function_Sig", "Function_Open",
  "Type", "Dotdot", "Operand", "Unary_Op", "Assign_Op", "Ident",
  "EnterBlock", "ExitBlock", "Block", "StatementList", "Statement",
  "SimpleStmt", "ExpressionStmt", "SimpleIfStmt", "IfStmt", "If",
  "If_Jump", "Else", "Condition", "AssignmentStmt", "DeclarationStmt",
  "ReturnStmt", "PrintStmt", "Unary_Expression", "BreakExpression",
  "ArrayElements", "ArrayExpression", "BeginArr", "For_Label", "For_Loop",
  "LoopExpression", "Loop", "WhileStmt", "While", "While_Jump",
  "Expression", "ArgumentList", "Call_Expression", "Cast_Expression",
  "Index_Expression", "Primary_Expression", "Literal", "Boolean_Literal",
  "Parameter_List", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
      62,    60,    43,    45,    42,    47,    37,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,    61,
      33,   123,   125,    40,    41,    38,    91,    59,    93,    58,
      44,    34
};
# endif

#define YYPACT_NINF (-136)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-94)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      12,   -10,    21,    12,  -136,  -136,   -11,   -12,  -136,  -136,
    -136,   133,  -136,     4,  -136,     2,  -136,  -136,   385,  -136,
       3,  -136,  -136,   385,     1,     9,  -136,  -136,  -136,    42,
    -136,  -136,  -136,   385,    15,    10,  -136,   -28,  -136,    84,
       5,   188,  -136,   243,  -136,    13,  -136,    37,  -136,   385,
    -136,    14,    18,    22,  -136,  -136,  -136,   385,   188,  -136,
     680,    27,  -136,   385,   654,  -136,  -136,  -136,    63,  -136,
    -136,    40,    23,    78,   -38,    44,   -36,    39,  -136,   654,
      89,   654,   385,   385,  -136,  -136,  -136,  -136,  -136,  -136,
     314,   385,   547,  -136,    41,  -136,    38,  -136,  -136,   385,
    -136,  -136,   188,  -136,  -136,  -136,  -136,   -19,  -136,   654,
    -136,  -136,  -136,   -32,   654,  -136,   188,  -136,  -136,   385,
     385,   385,   385,   385,   385,   385,   385,   385,   385,   385,
     385,   385,   385,   385,    -5,    60,    -5,    -5,    93,     8,
     -35,   385,    -5,    85,   573,   599,  -136,   654,   -25,   654,
    -136,   281,  -136,   431,  -136,  -136,  -136,    27,  -136,   385,
    -136,    27,   654,   654,   118,   118,   118,   118,   706,   118,
     195,   195,   195,   195,     7,     7,     7,  -136,  -136,  -136,
    -136,    -5,    -5,  -136,    -5,  -136,  -136,    -5,    86,    68,
     385,    -5,   654,    51,    77,  -136,  -136,  -136,   385,  -136,
     385,   628,  -136,  -136,   654,  -136,  -136,    74,  -136,  -136,
      76,    -5,   654,    91,   385,  -136,   654,   460,   347,   385,
      -5,  -136,   385,   654,  -136,  -136,   489,   518,  -136,   654,
    -136,  -136
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     4,     5,     0,     0,    14,     1,
       3,     0,     6,     0,     7,     0,   127,   128,    67,    56,
       0,    88,    86,    73,     0,     0,    25,   122,   123,    33,
      26,    34,     9,     0,     0,    79,    44,     0,   115,     0,
      22,     0,    48,     0,    39,    45,    50,    55,    46,     0,
      49,     0,     0,     0,    90,    91,    92,     0,     0,    85,
      47,     0,    83,     0,    51,   118,   116,   117,    72,    23,
     125,     0,     0,    10,     0,     0,     0,    33,    93,    68,
       0,    74,     0,     0,    28,    29,    30,    31,    32,    27,
       0,     0,     0,    33,     0,    77,     0,   126,    71,     0,
      35,    37,     0,     8,    38,    41,    58,     0,    57,    59,
      40,    42,    43,     0,    75,    81,     0,    84,    89,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,     0,     0,     0,   111,   109,     0,    60,
      24,     0,   124,     0,    36,    53,    54,     0,    78,     0,
      82,     0,   108,   107,   103,   104,   105,   106,    94,    95,
     101,   102,    96,    97,    98,    99,   100,    18,    15,    16,
      17,     0,     0,   113,     0,   129,    11,     0,     0,     0,
       0,     0,    63,    65,     0,    69,    70,   112,     0,    21,
       0,     0,   114,    52,    76,    87,    19,     0,   130,    13,
       0,     0,    64,    66,     0,    80,   110,     0,     0,     0,
       0,   131,     0,    61,   120,   119,     0,     0,   132,    62,
     121,    20
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -136,  -136,  -136,   135,  -136,  -136,  -136,  -136,  -136,  -135,
     -57,  -136,  -136,  -136,   112,  -136,   -49,   -41,   -24,   -39,
    -136,  -136,  -136,    49,  -136,  -136,  -136,   100,  -136,  -136,
    -136,  -136,   125,  -136,  -136,  -136,  -136,  -136,  -136,    16,
    -136,  -136,  -136,  -136,   -18,  -136,  -136,  -136,  -136,  -136,
    -136,  -136,  -136
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     5,     6,    12,    14,     7,   183,
     200,    38,    39,    91,    40,    41,   101,    42,    43,    44,
      45,    46,    47,    48,    49,   157,   107,   108,    50,    51,
      52,    53,    54,    55,   113,    56,    57,    58,    59,    78,
      61,    62,    63,   161,    64,   148,    65,    66,    67,    68,
      69,    70,    74
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      79,   185,   186,    19,   104,    81,    75,   193,    71,   115,
     177,    96,   188,   141,   190,    92,   138,   102,   119,   120,
     117,     9,   139,   142,   191,     1,   158,    60,   159,   197,
       8,   109,    31,    97,   116,   198,   178,   179,   180,   114,
      11,    13,    76,    80,    72,   109,   206,   207,   189,   208,
     181,   182,   209,   154,    82,    93,   213,    60,    73,    60,
     106,    99,    83,   104,   144,   145,   155,   160,    95,   134,
     105,   110,   147,   149,    60,   111,   221,   104,    31,   112,
     135,   153,   136,   137,   140,   228,    84,    85,    86,    87,
      88,    89,    90,    16,    17,    90,   143,   151,   187,   152,
     214,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   203,    26,    60,   184,
     205,    27,    28,   192,    77,   194,   210,   211,   215,   119,
     120,   219,    60,   201,    30,   220,    15,    33,    10,    34,
     222,   204,    16,    17,   218,    37,    94,    18,   127,   128,
     129,   130,   131,   132,   133,    19,   156,    20,    21,    22,
      23,    24,    25,   118,    98,     0,    26,     0,     0,     0,
      27,    28,   212,    29,     0,     0,     0,     0,     0,     0,
     216,     0,   217,    30,    31,    32,    33,     0,    34,    35,
      36,    15,     0,     0,    37,     0,   223,    16,    17,     0,
     226,   227,    18,     0,   229,     0,   119,   120,     0,     0,
      19,     0,    20,    21,    22,    23,    24,    25,     0,     0,
       0,    26,     0,     0,     0,    27,    28,     0,    29,   131,
     132,   133,     0,     0,     0,     0,     0,     0,    30,    31,
     100,    33,     0,    34,    35,    36,    15,     0,     0,    37,
       0,     0,    16,    17,     0,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,    19,     0,    20,    21,    22,
      23,    24,    25,     0,     0,     0,    26,     0,     0,     0,
      27,    28,     0,    29,     0,     0,     0,     0,     0,   199,
      16,    17,     0,    30,    31,   103,    33,     0,    34,    35,
      36,     0,     0,     0,    37,    20,    21,    22,    23,     0,
       0,     0,     0,     0,    26,     0,     0,     0,    27,    28,
       0,    77,     0,    16,    17,     0,     0,     0,     0,     0,
       0,    30,     0,     0,    33,     0,    34,    35,    20,    21,
      22,    23,    37,     0,     0,     0,     0,    26,     0,     0,
       0,    27,    28,     0,    77,     0,    16,    17,     0,     0,
       0,     0,     0,     0,    30,     0,     0,    33,   146,    34,
      35,    20,    21,    22,    23,    37,     0,     0,     0,     0,
      26,     0,     0,     0,    27,    28,     0,    77,     0,     0,
       0,     0,     0,     0,    16,    17,     0,    30,     0,     0,
      33,     0,    34,    35,     0,   225,     0,     0,    37,    20,
      21,    22,    23,     0,     0,     0,     0,     0,    26,     0,
       0,     0,    27,    28,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,     0,    33,     0,
      34,    35,   119,   120,     0,     0,    37,   121,   122,   123,
     124,   125,   126,     0,     0,     0,     0,     0,     0,     0,
       0,   127,   128,   129,   130,   131,   132,   133,     0,     0,
       0,   119,   120,     0,     0,     0,   121,   122,   123,   124,
     125,   126,     0,     0,     0,     0,     0,     0,     0,   202,
     127,   128,   129,   130,   131,   132,   133,     0,     0,     0,
     119,   120,     0,     0,     0,   121,   122,   123,   124,   125,
     126,     0,     0,     0,     0,     0,     0,     0,   224,   127,
     128,   129,   130,   131,   132,   133,     0,     0,     0,   119,
     120,     0,     0,     0,   121,   122,   123,   124,   125,   126,
       0,     0,     0,     0,     0,     0,     0,   230,   127,   128,
     129,   130,   131,   132,   133,     0,     0,     0,   119,   120,
       0,     0,     0,   121,   122,   123,   124,   125,   126,     0,
       0,     0,     0,     0,     0,     0,   231,   127,   128,   129,
     130,   131,   132,   133,   119,   120,     0,     0,     0,   121,
     122,   123,   124,   125,   126,     0,     0,     0,     0,     0,
       0,   150,     0,   127,   128,   129,   130,   131,   132,   133,
     119,   120,     0,     0,     0,   121,   122,   123,   124,   125,
     126,     0,     0,     0,     0,     0,     0,   195,     0,   127,
     128,   129,   130,   131,   132,   133,   199,     0,     0,   119,
     120,     0,     0,     0,   121,   122,   123,   124,   125,   126,
       0,     0,     0,   196,     0,     0,     0,     0,   127,   128,
     129,   130,   131,   132,   133,   119,   120,     0,     0,     0,
     121,   122,   123,   124,   125,   126,     0,     0,     0,     0,
       0,     0,     0,     0,   127,   128,   129,   130,   131,   132,
     133,   -93,   -93,     0,     0,     0,   -93,   -93,   -93,   -93,
     -93,   -93,     0,     0,     0,     0,     0,     0,     0,     0,
     -93,   -93,   -93,     0,   -93,   -93,   -93,   119,   120,     0,
       0,     0,   121,   122,   123,   124,     0,   126,     0,     0,
       0,     0,     0,     0,     0,     0,   127,   128,   129,   130,
     131,   132,   133
};

static const yytype_int16 yycheck[] =
{
      18,   136,   137,    22,    43,    23,     4,   142,     4,    58,
      15,    39,     4,    49,    49,    33,    54,    41,    11,    12,
      61,     0,    60,    59,    59,    13,    58,    11,    60,    54,
      40,    49,    51,    61,    58,    60,    41,    42,    43,    57,
      51,    53,    40,    40,    40,    63,   181,   182,    40,   184,
      55,    56,   187,   102,    53,    40,   191,    41,    54,    43,
      23,    56,    53,   102,    82,    83,   107,   116,    58,     6,
      57,    57,    90,    91,    58,    57,   211,   116,    51,    57,
      40,    99,    59,     5,    40,   220,    44,    45,    46,    47,
      48,    49,    53,     9,    10,    53,     7,    56,     5,    61,
      49,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   157,    33,   102,    59,
     161,    37,    38,   141,    40,    40,    40,    59,    51,    11,
      12,    57,   116,   151,    50,    59,     3,    53,     3,    55,
      49,   159,     9,    10,   201,    61,    34,    14,    30,    31,
      32,    33,    34,    35,    36,    22,   107,    24,    25,    26,
      27,    28,    29,    63,    39,    -1,    33,    -1,    -1,    -1,
      37,    38,   190,    40,    -1,    -1,    -1,    -1,    -1,    -1,
     198,    -1,   200,    50,    51,    52,    53,    -1,    55,    56,
      57,     3,    -1,    -1,    61,    -1,   214,     9,    10,    -1,
     218,   219,    14,    -1,   222,    -1,    11,    12,    -1,    -1,
      22,    -1,    24,    25,    26,    27,    28,    29,    -1,    -1,
      -1,    33,    -1,    -1,    -1,    37,    38,    -1,    40,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    -1,    55,    56,    57,     3,    -1,    -1,    61,
      -1,    -1,     9,    10,    -1,    -1,    -1,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    -1,    24,    25,    26,
      27,    28,    29,    -1,    -1,    -1,    33,    -1,    -1,    -1,
      37,    38,    -1,    40,    -1,    -1,    -1,    -1,    -1,     8,
       9,    10,    -1,    50,    51,    52,    53,    -1,    55,    56,
      57,    -1,    -1,    -1,    61,    24,    25,    26,    27,    -1,
      -1,    -1,    -1,    -1,    33,    -1,    -1,    -1,    37,    38,
      -1,    40,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    -1,    -1,    53,    -1,    55,    56,    24,    25,
      26,    27,    61,    -1,    -1,    -1,    -1,    33,    -1,    -1,
      -1,    37,    38,    -1,    40,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,    55,
      56,    24,    25,    26,    27,    61,    -1,    -1,    -1,    -1,
      33,    -1,    -1,    -1,    37,    38,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,     9,    10,    -1,    50,    -1,    -1,
      53,    -1,    55,    56,    -1,    58,    -1,    -1,    61,    24,
      25,    26,    27,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      -1,    -1,    37,    38,    -1,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    -1,    -1,    53,    -1,
      55,    56,    11,    12,    -1,    -1,    61,    16,    17,    18,
      19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      -1,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    30,    31,    32,
      33,    34,    35,    36,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    30,    31,    32,    33,    34,    35,    36,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    30,
      31,    32,    33,    34,    35,    36,     8,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    54,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    -1,    34,    35,    36,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    -1,    21,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    13,    63,    64,    65,    66,    67,    70,    40,     0,
      65,    51,    68,    53,    69,     3,     9,    10,    14,    22,
      24,    25,    26,    27,    28,    29,    33,    37,    38,    40,
      50,    51,    52,    53,    55,    56,    57,    61,    73,    74,
      76,    77,    79,    80,    81,    82,    83,    84,    85,    86,
      90,    91,    92,    93,    94,    95,    97,    98,    99,   100,
     101,   102,   103,   104,   106,   108,   109,   110,   111,   112,
     113,     4,    40,    54,   114,     4,    40,    40,   101,   106,
      40,   106,    53,    53,    44,    45,    46,    47,    48,    49,
      53,    75,   106,    40,    76,    58,    39,    61,    94,    56,
      52,    78,    80,    52,    81,    57,    23,    88,    89,   106,
      57,    57,    57,    96,   106,    78,    80,    79,    89,    11,
      12,    16,    17,    18,    19,    20,    21,    30,    31,    32,
      33,    34,    35,    36,     6,    40,    59,     5,    54,    60,
      40,    49,    59,     7,   106,   106,    54,   106,   107,   106,
      54,    56,    61,   106,    78,    79,    85,    87,    58,    60,
      78,   105,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,    15,    41,    42,
      43,    55,    56,    71,    59,    71,    71,     5,     4,    40,
      49,    59,   106,    71,    40,    54,    54,    54,    60,     8,
      72,   106,    58,    79,   106,    79,    71,    71,    71,    71,
      40,    59,   106,    71,    49,    51,   106,   106,    72,    57,
      59,    71,    49,   106,    58,    58,   106,   106,    71,   106,
      58,    58
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    62,    63,    64,    64,    65,    66,    67,    68,    68,
      69,    69,    69,    69,    70,    71,    71,    71,    71,    71,
      71,    72,    73,    73,    73,    74,    74,    75,    75,    75,
      75,    75,    75,    76,    77,    78,    79,    79,    80,    80,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    82,
      82,    83,    84,    85,    85,    85,    86,    87,    88,    89,
      90,    91,    91,    91,    91,    91,    91,    92,    92,    93,
      93,    94,    94,    95,    95,    96,    96,    97,    97,    98,
      99,   100,   100,   101,   101,   101,   102,   103,   104,   105,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   107,
     107,   108,   108,   109,   110,   111,   111,   111,   111,   111,
     111,   111,   112,   112,   112,   112,   112,   113,   113,   114,
     114,   114,   114
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     2,     3,     2,
       2,     4,     3,     5,     2,     1,     1,     1,     1,     2,
       5,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     2,     1,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     3,     3,     1,     1,     0,     1,     1,
       3,     6,     7,     4,     5,     4,     5,     1,     2,     4,
       4,     2,     1,     1,     2,     1,     3,     2,     3,     1,
       5,     2,     3,     1,     2,     1,     1,     4,     1,     0,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     4,     3,     4,     1,     1,     1,     1,     6,
       6,     7,     1,     1,     3,     1,     2,     1,     1,     3,
       4,     5,     6
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 6:
#line 414 "compiler.y"
                                    {   
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
#line 2042 "y.tab.c"
    break;

  case 7:
#line 430 "compiler.y"
                                 {
                                    func->ret = 0;
                                    func->func_sig = (yyvsp[0].s_val);      
                                    (yyval.s_val) = (yyvsp[0].s_val);  
                                    
                                    g_address = 0;      // *
                                    char *signature;
                                    if(strcmp((yyvsp[-1].s_val), "main") == 0){
                                        signature = "([Ljava/lang/String;)V";
                                    }
                                    else{
                                        signature = (yyvsp[0].s_val);
                                    }

                                    SETUPGEN(".method public static %s%s\n", (yyvsp[-1].s_val), signature);
                                    SETUPGEN(".limit stack 100\n");
                                    SETUPGEN(".limit locals 100\n");

                                 }
#line 2066 "y.tab.c"
    break;

  case 10:
#line 457 "compiler.y"
                                                { (yyval.s_val) = func_get(strdup(""), "V"); }
#line 2072 "y.tab.c"
    break;

  case 11:
#line 458 "compiler.y"
                                                { (yyval.s_val) = func_get(strdup(""), type_get((yyvsp[0].s_val))); }
#line 2078 "y.tab.c"
    break;

  case 12:
#line 459 "compiler.y"
                                                { (yyval.s_val) = func_get((yyvsp[-1].s_val), "V"); }
#line 2084 "y.tab.c"
    break;

  case 13:
#line 460 "compiler.y"
                                                { (yyval.s_val) = func_get((yyvsp[-3].s_val), type_get((yyvsp[0].s_val))); }
#line 2090 "y.tab.c"
    break;

  case 14:
#line 464 "compiler.y"
                {
                    (yyval.s_val) = (yyvsp[0].s_val);
                    printf("func: %s\n", (yyvsp[0].s_val));
                    insert_symbol((yyvsp[0].s_val), -1, "func", "");                                               
                    create_symbol();
                }
#line 2101 "y.tab.c"
    break;

  case 15:
#line 473 "compiler.y"
            { (yyval.s_val) = "i32"; }
#line 2107 "y.tab.c"
    break;

  case 16:
#line 474 "compiler.y"
            { (yyval.s_val) = "f32"; }
#line 2113 "y.tab.c"
    break;

  case 17:
#line 475 "compiler.y"
            { (yyval.s_val) = "bool"; }
#line 2119 "y.tab.c"
    break;

  case 18:
#line 476 "compiler.y"
            { (yyval.s_val) = "str"; }
#line 2125 "y.tab.c"
    break;

  case 19:
#line 477 "compiler.y"
               { (yyval.s_val) = (yyvsp[0].s_val); }
#line 2131 "y.tab.c"
    break;

  case 20:
#line 478 "compiler.y"
                                    { 
                                        (yyval.s_val) = "array"; 
                                        array_type = (yyvsp[-3].s_val);
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
#line 2151 "y.tab.c"
    break;

  case 21:
#line 496 "compiler.y"
             { printf("DOTDOT\n"); }
#line 2157 "y.tab.c"
    break;

  case 24:
#line 502 "compiler.y"
                         { (yyval.s_val) = (yyvsp[-1].s_val); }
#line 2163 "y.tab.c"
    break;

  case 25:
#line 506 "compiler.y"
            { (yyval.s_val) = "NEG"; }
#line 2169 "y.tab.c"
    break;

  case 26:
#line 507 "compiler.y"
            { (yyval.s_val) = "NOT"; }
#line 2175 "y.tab.c"
    break;

  case 27:
#line 511 "compiler.y"
                    { (yyval.s_val) = "ASSIGN"; }
#line 2181 "y.tab.c"
    break;

  case 28:
#line 512 "compiler.y"
                    { (yyval.s_val) = "add"; }
#line 2187 "y.tab.c"
    break;

  case 29:
#line 513 "compiler.y"
                    { (yyval.s_val) = "sub"; }
#line 2193 "y.tab.c"
    break;

  case 30:
#line 514 "compiler.y"
                    { (yyval.s_val) = "mul"; }
#line 2199 "y.tab.c"
    break;

  case 31:
#line 515 "compiler.y"
                    { (yyval.s_val) = "div"; }
#line 2205 "y.tab.c"
    break;

  case 32:
#line 516 "compiler.y"
                    { (yyval.s_val) = "rem"; }
#line 2211 "y.tab.c"
    break;

  case 33:
#line 520 "compiler.y"
            { 
                symbol_entry *entry = lookup_symbol((yyvsp[0].s_val));

                if (entry) {
                    (yyval.s_val) = entry->type;

                    if(strcmp(entry->type, "array") == 0){
                        (yyval.s_val) = entry->array_type;
                    }

                    printf("IDENT (name=%s, address=%d)\n", entry->name, entry->address);
                }
                else {
                    (yyval.s_val) = "undefined";
                    printf("error:%d: undefined: %s\n", yylineno, (yyvsp[0].s_val));
                }

                addr_gen("load", entry->type, entry->address);
            }
#line 2235 "y.tab.c"
    break;

  case 34:
#line 542 "compiler.y"
          { create_symbol(); }
#line 2241 "y.tab.c"
    break;

  case 35:
#line 546 "compiler.y"
            { 
                func->ret = 0; 
                dump_symbol(); 
            }
#line 2250 "y.tab.c"
    break;

  case 52:
#line 584 "compiler.y"
                                 { LABELGEN("label_%d:\n", (yyvsp[-1].i_val)); }
#line 2256 "y.tab.c"
    break;

  case 57:
#line 598 "compiler.y"
        {
            CODEGEN("ifeq label_%d\n", g_lbl_idx);
            (yyval.i_val) = g_lbl_idx++;
        }
#line 2265 "y.tab.c"
    break;

  case 60:
#line 613 "compiler.y"
                                {
                                    symbol_entry *entry = lookup_symbol((yyvsp[-2].s_val));

                                    if (!entry) {
                                        printf("error:%d: undefined: %s\n", yylineno, (yyvsp[-2].s_val));
                                    }
                                    else{
                                        printf("%s\n", (yyvsp[-1].s_val)); 
                                    }

                                    if(strcmp((yyvsp[-1].s_val), "ASSIGN")){
                                        addr_gen("load", entry->type, entry->address);
                                        CODEGEN("swap\n");
                                        type_gen((yyvsp[-1].s_val), entry->type);
                                    }

                                    addr_gen("store", entry->type, entry->address);
                                }
#line 2288 "y.tab.c"
    break;

  case 61:
#line 635 "compiler.y"
                                    {   
                                        insert_symbol((yyvsp[-4].s_val), 0, (yyvsp[-2].s_val), "-");
                                        symbol_entry *entry = lookup_symbol((yyvsp[-4].s_val));
                                        addr_gen("store", entry->type, entry->address);
                                    }
#line 2298 "y.tab.c"
    break;

  case 62:
#line 640 "compiler.y"
                                         {   
                                        insert_symbol((yyvsp[-4].s_val), 1, (yyvsp[-2].s_val), "-");
                                        symbol_entry *entry = lookup_symbol((yyvsp[-4].s_val));
                                        addr_gen("store", entry->type, entry->address);
                                    }
#line 2308 "y.tab.c"
    break;

  case 63:
#line 645 "compiler.y"
                                    {
                                        insert_symbol((yyvsp[-2].s_val), 0, (yyvsp[0].s_val), "-");
                                        symbol_entry *entry = lookup_symbol((yyvsp[-2].s_val));
                                        addr_gen("store", entry->type, entry->address);
                                    }
#line 2318 "y.tab.c"
    break;

  case 64:
#line 650 "compiler.y"
                                    {   
                                        insert_symbol((yyvsp[-2].s_val), 1, (yyvsp[0].s_val), "-");
                                        symbol_entry *entry = lookup_symbol((yyvsp[-2].s_val));
                                        addr_gen("store", entry->type, entry->address);
                                    }
#line 2328 "y.tab.c"
    break;

  case 65:
#line 655 "compiler.y"
                                    { 
                                        insert_symbol((yyvsp[-2].s_val), 0, (yyvsp[0].s_val), "-");
                                        symbol_entry *entry = lookup_symbol((yyvsp[-2].s_val));
                                        type_gen("const_0", entry->type);
                                        addr_gen("store", entry->type, entry->address);
                                    }
#line 2339 "y.tab.c"
    break;

  case 66:
#line 661 "compiler.y"
                                    { 
                                        insert_symbol((yyvsp[-2].s_val), 1, (yyvsp[0].s_val), "-");
                                        symbol_entry *entry = lookup_symbol((yyvsp[-2].s_val));
                                        type_gen("const_0", entry->type);
                                        addr_gen("store", entry->type, entry->address);
                                    }
#line 2350 "y.tab.c"
    break;

  case 67:
#line 670 "compiler.y"
                { 
                    printf("return\n"); 
                    CODEGEN("return\n");    
                }
#line 2359 "y.tab.c"
    break;

  case 68:
#line 674 "compiler.y"
                        { 
                            printf("%creturn\n", (yyvsp[0].s_val)[0]); 
                            func->ret = 1;
                            type_gen("return", (yyvsp[0].s_val));
                        }
#line 2369 "y.tab.c"
    break;

  case 69:
#line 682 "compiler.y"
                                { 
                                    printf("PRINT %s\n", (yyvsp[-1].s_val)); 
                                    print_gen("print", (yyvsp[-1].s_val));
                                }
#line 2378 "y.tab.c"
    break;

  case 70:
#line 686 "compiler.y"
                                { 
                                    printf("PRINTLN %s\n", (yyvsp[-1].s_val));
                                    print_gen("println", (yyvsp[-1].s_val));
                                }
#line 2387 "y.tab.c"
    break;

  case 71:
#line 693 "compiler.y"
                                { 
                                    (yyval.s_val) = (yyvsp[0].s_val);
                                    printf("%s\n", (yyvsp[-1].s_val));

                                    if(strcmp((yyvsp[-1].s_val), "NOT") == 0){
                                        CODEGEN("iconst_1\n");
                                        CODEGEN("ixor\n");
                                    }
                                    else if(strcmp((yyvsp[-1].s_val), "NEG") == 0){
                                        CODEGEN("%cneg\n", (yyvsp[0].s_val)[0]);
                                    }
                                }
#line 2404 "y.tab.c"
    break;

  case 75:
#line 714 "compiler.y"
                    {
                        CODEGEN("iastore\n");
                        CODEGEN("iconst_1\n");
                        CODEGEN("iadd\n");
                        CODEGEN("dup2\n");
                    }
#line 2415 "y.tab.c"
    break;

  case 76:
#line 720 "compiler.y"
                                   {
                                        CODEGEN("iastore\n");
                                        CODEGEN("iconst_1\n");
                                        CODEGEN("iadd\n");
                                        CODEGEN("dup2\n");
                                    }
#line 2426 "y.tab.c"
    break;

  case 78:
#line 730 "compiler.y"
                                 {
                                    CODEGEN("pop\n");
                                }
#line 2434 "y.tab.c"
    break;

  case 79:
#line 736 "compiler.y"
            { 
                CODEGEN("iconst_0\n");
                CODEGEN("dup2\n");
            }
#line 2443 "y.tab.c"
    break;

  case 80:
#line 743 "compiler.y"
                       {
                            symbol_entry *entry = lookup_symbol((yyvsp[-1].s_val));

                            if(entry){
                                printf("IDENT (name=%s, address=%d)\n", entry->name, entry->address);
                                create_symbol();                                            
                                insert_symbol((yyvsp[-3].s_val), 0, entry->array_type, "-");
                                
                                (yyval.i_val) = g_lbl_idx;
                                symbol_entry *for_entry = lookup_symbol((yyvsp[-3].s_val));

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
                                printf("error:%d: undefined: %s\n", yylineno, (yyvsp[-1].s_val));
                                create_symbol();
                                insert_symbol((yyvsp[-3].s_val), 0, "undefined", "-");
                            }
                        }
#line 2481 "y.tab.c"
    break;

  case 84:
#line 785 "compiler.y"
                {
                    CODEGEN("goto label_%d\n", (yyvsp[-1].i_val));
                    LABELGEN("label_%d:\n", g_lbl_idx++);
                }
#line 2490 "y.tab.c"
    break;

  case 85:
#line 789 "compiler.y"
                {
                    CODEGEN("dup2\n");
                    CODEGEN("if_icmpne label_%d\n", (yyvsp[0].i_val));
                    CODEGEN("pop2\n");
                }
#line 2500 "y.tab.c"
    break;

  case 86:
#line 797 "compiler.y"
            {
                LABELGEN("label_%d:\n", g_lbl_idx);
                (yyval.i_val) = g_lbl_idx++;       
            }
#line 2509 "y.tab.c"
    break;

  case 87:
#line 803 "compiler.y"
                                       {
                                            CODEGEN("goto label_%d\n", (yyvsp[-3].i_val));
                                            LABELGEN("label_%d:\n", (yyvsp[-1].i_val));
                                        }
#line 2518 "y.tab.c"
    break;

  case 88:
#line 810 "compiler.y"
            { 
                LABELGEN("label_%d:\n", g_lbl_idx);
                (yyval.i_val) = g_lbl_idx++; 
            }
#line 2527 "y.tab.c"
    break;

  case 89:
#line 817 "compiler.y"
        { 
            CODEGEN("ifeq label_%d\n", g_lbl_idx);
            (yyval.i_val) = g_lbl_idx++; 
        }
#line 2536 "y.tab.c"
    break;

  case 91:
#line 825 "compiler.y"
                        {
                            CODEGEN("goto label_%d\n", g_lbl_idx);

                        }
#line 2545 "y.tab.c"
    break;

  case 94:
#line 831 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    printf("LOR\n");
                                    CODEGEN("ior\n");
                                }
#line 2555 "y.tab.c"
    break;

  case 95:
#line 836 "compiler.y"
                                { 
                                    (yyval.s_val) = "bool";
                                    printf("LAND\n");
                                    CODEGEN("iand\n");
                                }
#line 2565 "y.tab.c"
    break;

  case 96:
#line 841 "compiler.y"
                                {
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "ADD", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("ADD\n");
                                    CODEGEN("%cadd\n", (yyvsp[-2].s_val)[0]);
                                }
#line 2577 "y.tab.c"
    break;

  case 97:
#line 848 "compiler.y"
                                {
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "SUB", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("SUB\n");
                                    CODEGEN("%csub\n", (yyvsp[-2].s_val)[0]);
                                }
#line 2589 "y.tab.c"
    break;

  case 98:
#line 855 "compiler.y"
                                {
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "MUL", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("MUL\n");
                                    CODEGEN("%cmul\n", (yyvsp[-2].s_val)[0]);
                                }
#line 2601 "y.tab.c"
    break;

  case 99:
#line 862 "compiler.y"
                                {
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "DIV", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("DIV\n");
                                    CODEGEN("%cdiv\n", (yyvsp[-2].s_val)[0]);
                                }
#line 2613 "y.tab.c"
    break;

  case 100:
#line 869 "compiler.y"
                                {
                                    (yyval.s_val) = "i32";
                                    printf("REM\n");   
                                    CODEGEN("irem\n");
                                }
#line 2623 "y.tab.c"
    break;

  case 101:
#line 874 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "GTR", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("GTR\n");
                                    cmp_gen("ifgt", (yyvsp[-2].s_val));
                                }
#line 2636 "y.tab.c"
    break;

  case 102:
#line 882 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "LSS", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("LSS\n");
                                    cmp_gen("iflt", (yyvsp[-2].s_val));
                                }
#line 2649 "y.tab.c"
    break;

  case 103:
#line 890 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "GEQ", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("GEQ\n");
                                    cmp_gen("ifge", (yyvsp[-2].s_val));
                                }
#line 2662 "y.tab.c"
    break;

  case 104:
#line 898 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "LEQ", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("LEQ\n");
                                    cmp_gen("ifle", (yyvsp[-2].s_val));
                                }
#line 2675 "y.tab.c"
    break;

  case 105:
#line 906 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "EQL", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("EQL\n");
                                    cmp_gen("ifeq", (yyvsp[-2].s_val));
                                }
#line 2688 "y.tab.c"
    break;

  case 106:
#line 914 "compiler.y"
                                {
                                    (yyval.s_val) = "bool";
                                    if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                        printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "NEQ", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                    }
                                    printf("NEQ\n");
                                    cmp_gen("ifne", (yyvsp[-2].s_val));
                                }
#line 2701 "y.tab.c"
    break;

  case 107:
#line 922 "compiler.y"
                                    {
                                        if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                            printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "LSHIFT", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                        }
                                        printf("LSHIFT\n");
                                        CODEGEN("ishl\n");
                                    }
#line 2713 "y.tab.c"
    break;

  case 108:
#line 929 "compiler.y"
                                    {
                                        if (strcmp((yyvsp[-2].s_val), (yyvsp[0].s_val)) != 0){
                                            printf("error:%d: invalid operation: %s (mismatched types %s and %s)\n", yylineno, "RSHIFT", (yyvsp[-2].s_val), (yyvsp[0].s_val));
                                        }
                                        printf("RSHIFT\n");
                                        CODEGEN("iushr\n");
                                    }
#line 2725 "y.tab.c"
    break;

  case 113:
#line 949 "compiler.y"
                                    { 
                                        (yyval.s_val) = (yyvsp[0].s_val); printf("%c2%c\n", (yyvsp[-2].s_val)[0], (yyvsp[0].s_val)[0]); 
                                        CODEGEN("%c2%c\n", (yyvsp[-2].s_val)[0], (yyvsp[0].s_val)[0]);
                                    }
#line 2734 "y.tab.c"
    break;

  case 114:
#line 956 "compiler.y"
                                { 
                                    CODEGEN("iaload\n");
    
                                }
#line 2743 "y.tab.c"
    break;

  case 118:
#line 966 "compiler.y"
                        {   
                            symbol_entry *entry = lookup_symbol((yyvsp[0].s_val));

                            (yyval.s_val) = entry->type;
                            printf("call: %s%s\n", entry->name, entry->func_sig);
                        
                            CODEGEN("invokestatic Main/%s%s\n", entry->name, entry->func_sig);
                        }
#line 2756 "y.tab.c"
    break;

  case 119:
#line 974 "compiler.y"
                                            { 
                                                (yyval.s_val) = (yyvsp[-4].s_val); 
                                                CODEGEN("invokevirtual java/lang/String/substring(I)Ljava/lang/String;\n");
                                            }
#line 2765 "y.tab.c"
    break;

  case 120:
#line 978 "compiler.y"
                                            { 
                                                (yyval.s_val) = (yyvsp[-4].s_val);
                                                CODEGEN("iconst_0\n");
                                                CODEGEN("swap\n");
                                                CODEGEN("invokevirtual java/lang/String/substring(II)Ljava/lang/String;\n"); 
                                            }
#line 2776 "y.tab.c"
    break;

  case 121:
#line 984 "compiler.y"
                                                        { 
                                                            (yyval.s_val) = (yyvsp[-5].s_val);
                                                            CODEGEN("invokevirtual java/lang/String/substring(II)Ljava/lang/String;\n");
                                                        }
#line 2785 "y.tab.c"
    break;

  case 122:
#line 1003 "compiler.y"
                { 
                    (yyval.s_val) = "i32";
                    printf("INT_LIT %d\n", (yyvsp[0].i_val)); 
                    CODEGEN("ldc %d\n", yylval.i_val);
                }
#line 2795 "y.tab.c"
    break;

  case 123:
#line 1008 "compiler.y"
                {
                    (yyval.s_val) = "f32";
                    printf("FLOAT_LIT %f\n", (yyvsp[0].f_val)); 
                    CODEGEN("ldc %f\n", yylval.f_val);
                }
#line 2805 "y.tab.c"
    break;

  case 124:
#line 1013 "compiler.y"
                            { 
                                (yyval.s_val) = "str";
                                printf("STRING_LIT \"%s\"\n", (yyvsp[-1].s_val));    
                                CODEGEN("ldc \"%s\"\n", yylval.s_val);
                            }
#line 2815 "y.tab.c"
    break;

  case 125:
#line 1018 "compiler.y"
                        {
                            (yyval.s_val) = "bool";                            
                            CODEGEN("ldc %d\n", (yyvsp[0].b_val));
                        }
#line 2824 "y.tab.c"
    break;

  case 126:
#line 1022 "compiler.y"
                {
                    (yyval.s_val) = "str";
                    printf("STRING_LIT \"\"\n"); 
                    CODEGEN("ldc \"\"\n");
                }
#line 2834 "y.tab.c"
    break;

  case 127:
#line 1030 "compiler.y"
            {   
                (yyval.b_val) = 1; 
                printf("bool TRUE\n"); 
            }
#line 2843 "y.tab.c"
    break;

  case 128:
#line 1034 "compiler.y"
            { 
                (yyval.b_val) = 0; 
                printf("bool FALSE\n"); 
            }
#line 2852 "y.tab.c"
    break;

  case 129:
#line 1041 "compiler.y"
                    { 
                        char *type = type_get((yyvsp[0].s_val));
                        (yyval.s_val) = strdup(type);

                        printf("param %s, type: %s\n", (yyvsp[-2].s_val), type);
                        insert_symbol((yyvsp[-2].s_val), 0, (yyvsp[0].s_val), "-");
                    }
#line 2864 "y.tab.c"
    break;

  case 130:
#line 1048 "compiler.y"
                        { 
                            char *type = type_get((yyvsp[0].s_val));
                            (yyval.s_val) = strdup(type);

                            printf("param %s, type: %s\n", (yyvsp[-2].s_val), type);
                            insert_symbol((yyvsp[-2].s_val), 1, (yyvsp[0].s_val), "-");
                        }
#line 2876 "y.tab.c"
    break;

  case 131:
#line 1055 "compiler.y"
                                        { 
                                            char *type = type_get((yyvsp[0].s_val));
                                            (yyval.s_val) = list_get((yyvsp[-4].s_val), type);

                                            printf("param %s, type: %s\n", (yyvsp[-2].s_val), type);
                                            insert_symbol((yyvsp[-2].s_val), 0, (yyvsp[0].s_val), "-");
                                        }
#line 2888 "y.tab.c"
    break;

  case 132:
#line 1062 "compiler.y"
                                            { 
                                                char *type = type_get((yyvsp[0].s_val));
                                                (yyval.s_val) = list_get((yyvsp[-5].s_val), type);

                                                printf("param %s, type: %s\n", (yyvsp[-2].s_val), type);
                                                insert_symbol((yyvsp[-2].s_val), 1, (yyvsp[0].s_val), "-");
                                            }
#line 2900 "y.tab.c"
    break;


#line 2904 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1071 "compiler.y"


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


