/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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

#line 146 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
