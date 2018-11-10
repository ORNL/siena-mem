/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 26 "parser/AspenParser.y"

    #include "common/AST.h"
    #include "parser/AspenParseNode.h"
    ASTAppModel *globalapp = NULL;
    ASTMachModel *globalmach = NULL;
    ASTMapModel *globalmap = NULL;

    extern int yylex();
    extern int yylineno;
    extern char *yytext;
    void yyerror(const char *);

/* Line 371 of yacc.c  */
#line 81 "parser/AspenParser.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "AspenParser.hpp".  */
#ifndef YY_YY_PARSER_ASPENPARSER_HPP_INCLUDED
# define YY_YY_PARSER_ASPENPARSER_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
/* Line 387 of yacc.c  */
#line 41 "parser/AspenParser.y"

#include "parser/AspenParseNode.h"

typedef struct YYLTYPE {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  int first_filepos;
  int last_filepos;
  string filename;
} YYLTYPE;

# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */

# define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                 \
      if (N)                                                           \
        {                                                              \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;       \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;     \
          (Current).first_filepos= YYRHSLOC (Rhs, 1).first_filepos;    \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;        \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;      \
          (Current).last_filepos = YYRHSLOC (Rhs, N).last_filepos;     \
          (Current).filename     = YYRHSLOC (Rhs, 1).filename;         \
        }                                                              \
      else                                                             \
        { /* empty RHS */                                              \
          (Current).first_line   = (Current).last_line   =             \
            YYRHSLOC (Rhs, 0).last_line;                               \
          (Current).first_column = (Current).last_column =             \
            YYRHSLOC (Rhs, 0).last_column;                             \
          (Current).first_filepos = (Current).last_filepos =           \
            YYRHSLOC (Rhs, 0).last_filepos;                            \
          (Current).filename  = "";                                    \
        }                                                              \
    while (0)



/* Line 387 of yacc.c  */
#line 155 "parser/AspenParser.cpp"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TKW_PARAM = 258,
     TKW_ENUM = 259,
     TKW_IN = 260,
     TKW_WITH = 261,
     TKW_OF = 262,
     TKW_SIZE = 263,
     TKW_MODEL = 264,
     TKW_KERNEL = 265,
     TKW_DATA = 266,
     TKW_SAMPLE = 267,
     TKW_IMPORT = 268,
     TKW_AS = 269,
     TKW_TO = 270,
     TKW_FROM = 271,
     TKW_CALL = 272,
     TKW_ITERATE = 273,
     TKW_MAP = 274,
     TKW_PAR = 275,
     TKW_SEQ = 276,
     TKW_EXECUTE = 277,
     TKW_IF = 278,
     TKW_ELSE = 279,
     TKW_PROBABILITY = 280,
     TKW_SWITCH = 281,
     TKW_RESOURCE = 282,
     TKW_CONFLICT = 283,
     TKW_POWER = 284,
     TKW_STATIC = 285,
     TKW_DYNAMIC = 286,
     TKW_PROPERTY = 287,
     TKW_CONNECT = 288,
     TIDENT = 289,
     TSTRING = 290,
     TINT = 291,
     TREAL = 292,
     TKW_10POWER = 293,
     TDOTDOT = 294,
     TEQUAL = 295,
     TLPAREN = 296,
     TRPAREN = 297,
     TLBRACE = 298,
     TRBRACE = 299,
     TLBRACKET = 300,
     TRBRACKET = 301,
     TCOMMA = 302,
     TDOT = 303,
     TPLUS = 304,
     TMINUS = 305,
     TMUL = 306,
     TDIV = 307,
     TEXP = 308,
     TCOMPEQ = 309,
     TCOMPNE = 310,
     TCOMPLT = 311,
     TCOMPGT = 312,
     TCOMPLE = 313,
     TCOMPGE = 314,
     TAND = 315,
     TOR = 316,
     NEG = 317
   };
#endif
/* Tokens.  */
#define TKW_PARAM 258
#define TKW_ENUM 259
#define TKW_IN 260
#define TKW_WITH 261
#define TKW_OF 262
#define TKW_SIZE 263
#define TKW_MODEL 264
#define TKW_KERNEL 265
#define TKW_DATA 266
#define TKW_SAMPLE 267
#define TKW_IMPORT 268
#define TKW_AS 269
#define TKW_TO 270
#define TKW_FROM 271
#define TKW_CALL 272
#define TKW_ITERATE 273
#define TKW_MAP 274
#define TKW_PAR 275
#define TKW_SEQ 276
#define TKW_EXECUTE 277
#define TKW_IF 278
#define TKW_ELSE 279
#define TKW_PROBABILITY 280
#define TKW_SWITCH 281
#define TKW_RESOURCE 282
#define TKW_CONFLICT 283
#define TKW_POWER 284
#define TKW_STATIC 285
#define TKW_DYNAMIC 286
#define TKW_PROPERTY 287
#define TKW_CONNECT 288
#define TIDENT 289
#define TSTRING 290
#define TINT 291
#define TREAL 292
#define TKW_10POWER 293
#define TDOTDOT 294
#define TEQUAL 295
#define TLPAREN 296
#define TRPAREN 297
#define TLBRACE 298
#define TRBRACE 299
#define TLBRACKET 300
#define TRBRACKET 301
#define TCOMMA 302
#define TDOT 303
#define TPLUS 304
#define TMINUS 305
#define TMUL 306
#define TDIV 307
#define TEXP 308
#define TCOMPEQ 309
#define TCOMPNE 310
#define TCOMPLT 311
#define TCOMPGT 312
#define TCOMPLE 313
#define TCOMPGE 314
#define TAND 315
#define TOR 316
#define NEG 317



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;
extern YYLTYPE yylloc;
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_PARSER_ASPENPARSER_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 329 "parser/AspenParser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   598

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  157
/* YYNRULES -- Number of states.  */
#define YYNSTATES  371

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   317

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    18,
      21,    27,    29,    32,    34,    36,    38,    40,    42,    44,
      47,    51,    55,    58,    63,    68,    77,    84,    87,    93,
      98,   106,   110,   114,   122,   129,   136,   141,   146,   148,
     151,   156,   163,   165,   168,   174,   181,   190,   200,   202,
     206,   208,   210,   211,   213,   215,   217,   220,   222,   225,
     227,   230,   232,   235,   237,   239,   241,   243,   245,   247,
     253,   260,   262,   267,   275,   280,   289,   296,   305,   315,
     326,   332,   336,   342,   344,   346,   350,   354,   356,   360,
     365,   370,   374,   379,   386,   389,   395,   399,   403,   407,
     411,   415,   419,   423,   427,   435,   447,   457,   462,   468,
     473,   482,   490,   502,   503,   505,   507,   509,   515,   521,
     529,   537,   539,   541,   543,   544,   547,   550,   551,   554,
     556,   560,   562,   567,   572,   574,   578,   581,   584,   588,
     590,   592,   594,   599,   607,   611,   615,   619,   623,   627,
     631,   634,   636,   641,   642,   644,   648,   652
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      64,     0,    -1,    65,    -1,    82,    -1,    77,    -1,    66,
      -1,    93,    -1,    67,    -1,    66,    93,    -1,    66,    67,
      -1,    34,    34,    43,    68,    44,    -1,    69,    -1,    68,
      69,    -1,    71,    -1,    72,    -1,    74,    -1,    76,    -1,
      73,    -1,    70,    -1,    34,    34,    -1,    34,   118,    34,
      -1,    32,    34,   118,    -1,    32,    34,    -1,    33,    43,
     100,    44,    -1,    28,    34,    47,    34,    -1,    27,    34,
      41,    34,    42,   118,     6,    75,    -1,    27,    34,    41,
      34,    42,   118,    -1,    34,   118,    -1,    34,    41,    34,
      42,   118,    -1,    75,    47,    34,   118,    -1,    75,    47,
      34,    41,    34,    42,   118,    -1,    30,    29,   118,    -1,
      31,    29,   118,    -1,     9,    34,    43,    89,    79,    78,
      44,    -1,     9,    34,    43,    89,    79,    44,    -1,     9,
      34,    43,    89,    78,    44,    -1,    10,    43,    80,    44,
      -1,    11,    43,    80,    44,    -1,    81,    -1,    80,    81,
      -1,    19,    85,    15,    85,    -1,     9,    34,    43,    89,
      83,    44,    -1,    84,    -1,    83,    84,    -1,    10,    85,
      43,    91,    44,    -1,    10,    85,    43,    90,    91,    44,
      -1,    10,    85,    41,   116,    42,    43,    91,    44,    -1,
      10,    85,    41,   116,    42,    43,    90,    91,    44,    -1,
      34,    -1,    34,    48,    85,    -1,    36,    -1,    37,    -1,
      -1,    85,    -1,    38,    -1,    93,    -1,    89,    93,    -1,
      94,    -1,    90,    94,    -1,    95,    -1,    91,    95,    -1,
      96,    -1,    92,    96,    -1,   103,    -1,    97,    -1,   102,
      -1,   103,    -1,    97,    -1,   102,    -1,    22,   110,    43,
      92,    44,    -1,    22,   118,   110,    43,    92,    44,    -1,
     111,    -1,    34,   118,   112,   113,    -1,    34,   118,     7,
       8,   118,   112,   113,    -1,     3,    85,    40,   119,    -1,
       3,    85,    40,   119,     5,   119,    39,   119,    -1,     3,
      85,    40,   119,     5,    34,    -1,     3,    85,    45,    46,
      40,    43,    99,    44,    -1,     3,    85,    45,    46,    40,
      34,    41,   121,    42,    -1,     3,    85,    45,    46,    45,
      46,    40,    43,    98,    44,    -1,     4,    34,    43,   100,
      44,    -1,    43,    99,    44,    -1,    98,    47,    43,    99,
      44,    -1,    36,    -1,    37,    -1,    99,    47,    36,    -1,
      99,    47,    37,    -1,    34,    -1,   100,    47,    34,    -1,
      12,    85,    16,   120,    -1,    11,    34,    14,   120,    -1,
      11,    34,   118,    -1,    13,    34,    16,    35,    -1,    13,
      34,    16,    35,     6,   122,    -1,    17,    85,    -1,    17,
      85,    41,   121,    42,    -1,   105,    60,   105,    -1,   105,
      61,   105,    -1,   119,    54,   119,    -1,   119,    55,   119,
      -1,   119,    56,   119,    -1,   119,    57,   119,    -1,   119,
      58,   119,    -1,   119,    59,   119,    -1,    23,    41,   105,
      42,    43,    91,    44,    -1,    23,    41,   105,    42,    43,
      91,    44,    24,    43,    91,    44,    -1,    23,    41,   105,
      42,    43,    91,    44,    24,   106,    -1,   118,    43,    91,
      44,    -1,   107,   118,    43,    91,    44,    -1,    25,    43,
     107,    44,    -1,    25,    43,   107,    24,    43,    91,    44,
      44,    -1,    26,    41,    34,    42,    43,   107,    44,    -1,
      26,    41,    34,    42,    43,   107,    24,    43,    91,    44,
      44,    -1,    -1,    35,    -1,   104,    -1,   101,    -1,    20,
     110,    43,    91,    44,    -1,    21,   110,    43,    91,    44,
      -1,    18,    87,   118,   110,    43,    91,    44,    -1,    19,
      87,   118,   110,    43,    91,    44,    -1,   106,    -1,   108,
      -1,   109,    -1,    -1,    15,    34,    -1,    16,    34,    -1,
      -1,    14,   114,    -1,   115,    -1,   114,    47,   115,    -1,
      34,    -1,    34,    41,   119,    42,    -1,    34,    41,   121,
      42,    -1,   117,    -1,   116,    47,   117,    -1,    11,    85,
      -1,     3,    85,    -1,    45,   119,    46,    -1,    86,    -1,
      88,    -1,    85,    -1,    34,    45,   119,    46,    -1,    34,
      45,   119,    46,    45,   119,    46,    -1,    41,   119,    42,
      -1,   119,    49,   119,    -1,   119,    50,   119,    -1,   119,
      51,   119,    -1,   119,    52,   119,    -1,   119,    53,   119,
      -1,    50,   119,    -1,   120,    -1,    34,    41,   121,    42,
      -1,    -1,   119,    -1,   121,    47,   119,    -1,    34,    40,
     119,    -1,   122,    47,    34,    40,   119,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   187,   187,   191,   195,   202,   211,   219,   227,   232,
     240,   248,   249,   253,   254,   255,   256,   257,   258,   262,
     263,   268,   269,   273,   277,   284,   289,   299,   304,   309,
     314,   322,   323,   329,   334,   339,   347,   354,   361,   362,
     367,   380,   389,   390,   394,   405,   414,   422,   431,   432,
     435,   436,   440,   441,   444,   465,   466,   470,   471,   475,
     476,   480,   481,   489,   490,   491,   495,   496,   497,   501,
     506,   511,   520,   525,   536,   540,   544,   548,   553,   558,
     568,   575,   581,   595,   600,   605,   610,   618,   623,   631,
     638,   642,   649,   653,   661,   665,   673,   674,   675,   676,
     677,   678,   679,   680,   684,   689,   695,   703,   709,   718,
     723,   731,   736,   744,   745,   749,   753,   757,   762,   767,
     777,   787,   791,   795,   802,   803,   804,   808,   809,   813,
     814,   818,   819,   820,   824,   825,   829,   830,   834,   837,
     838,   839,   840,   841,   842,   843,   844,   845,   846,   847,
     848,   849,   853,   861,   862,   863,   867,   873
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TKW_PARAM", "TKW_ENUM", "TKW_IN",
  "TKW_WITH", "TKW_OF", "TKW_SIZE", "TKW_MODEL", "TKW_KERNEL", "TKW_DATA",
  "TKW_SAMPLE", "TKW_IMPORT", "TKW_AS", "TKW_TO", "TKW_FROM", "TKW_CALL",
  "TKW_ITERATE", "TKW_MAP", "TKW_PAR", "TKW_SEQ", "TKW_EXECUTE", "TKW_IF",
  "TKW_ELSE", "TKW_PROBABILITY", "TKW_SWITCH", "TKW_RESOURCE",
  "TKW_CONFLICT", "TKW_POWER", "TKW_STATIC", "TKW_DYNAMIC", "TKW_PROPERTY",
  "TKW_CONNECT", "TIDENT", "TSTRING", "TINT", "TREAL", "TKW_10POWER",
  "TDOTDOT", "TEQUAL", "TLPAREN", "TRPAREN", "TLBRACE", "TRBRACE",
  "TLBRACKET", "TRBRACKET", "TCOMMA", "TDOT", "TPLUS", "TMINUS", "TMUL",
  "TDIV", "TEXP", "TCOMPEQ", "TCOMPNE", "TCOMPLT", "TCOMPGT", "TCOMPLE",
  "TCOMPGE", "TAND", "TOR", "NEG", "$accept", "begin", "mach",
  "machcontent", "component", "componentstatements", "componentstmt",
  "subcomponent", "property", "connect", "conflict", "resource",
  "traitdefinitions", "power", "map", "mapkernel", "mapdata", "mapstmts",
  "mapstmt", "app", "kernels", "kernel", "ident", "value", "optionalident",
  "constant", "globalstatements", "localstatements", "kernelstatements",
  "execstatements", "globalstmt", "localstmt", "kernelstmt", "execstmt",
  "paramstmt", "arraylist", "valuelist", "identlist", "samplestmt",
  "datastmt", "importstmt", "kernelcall", "comparison", "ifthencontrol",
  "caseitems", "probabilitycontrol", "switchcontrol", "optionalstring",
  "controlstmt", "tofrom", "optionaltraitlist", "traitlist", "trait",
  "argdecllist", "argdecl", "quantity", "expr", "functioncall",
  "arguments", "namedarguments", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    63,    64,    64,    64,    65,    66,    66,    66,    66,
      67,    68,    68,    69,    69,    69,    69,    69,    69,    70,
      70,    71,    71,    72,    73,    74,    74,    75,    75,    75,
      75,    76,    76,    77,    77,    77,    78,    79,    80,    80,
      81,    82,    83,    83,    84,    84,    84,    84,    85,    85,
      86,    86,    87,    87,    88,    89,    89,    90,    90,    91,
      91,    92,    92,    93,    93,    93,    94,    94,    94,    95,
      95,    95,    96,    96,    97,    97,    97,    97,    97,    97,
      97,    98,    98,    99,    99,    99,    99,   100,   100,   101,
     102,   102,   103,   103,   104,   104,   105,   105,   105,   105,
     105,   105,   105,   105,   106,   106,   106,   107,   107,   108,
     108,   109,   109,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   112,   112,   112,   113,   113,   114,
     114,   115,   115,   115,   116,   116,   117,   117,   118,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   120,   121,   121,   121,   122,   122
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       5,     1,     2,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     2,     4,     4,     8,     6,     2,     5,     4,
       7,     3,     3,     7,     6,     6,     4,     4,     1,     2,
       4,     6,     1,     2,     5,     6,     8,     9,     1,     3,
       1,     1,     0,     1,     1,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     5,
       6,     1,     4,     7,     4,     8,     6,     8,     9,    10,
       5,     3,     5,     1,     1,     3,     3,     1,     3,     4,
       4,     3,     4,     6,     2,     5,     3,     3,     3,     3,
       3,     3,     3,     3,     7,    11,     9,     4,     5,     4,
       8,     7,    11,     0,     1,     1,     1,     5,     5,     7,
       7,     1,     1,     1,     0,     2,     2,     0,     2,     1,
       3,     1,     4,     4,     1,     3,     2,     2,     3,     1,
       1,     1,     4,     7,     3,     3,     3,     3,     3,     3,
       2,     1,     4,     0,     1,     3,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     2,     5,
       7,     4,     3,     6,    64,    65,    63,    48,     0,     0,
       0,     0,     0,     0,     1,     9,     8,     0,     0,     0,
       0,     0,     0,     0,    91,     0,     0,    49,    48,    50,
      51,    54,     0,     0,   141,   139,   140,    74,   151,     0,
      87,     0,     0,    55,     0,    90,     0,    92,     0,     0,
       0,     0,     0,     0,     0,     0,    11,    18,    13,    14,
      17,    15,    16,   153,     0,     0,   150,     0,     0,     0,
       0,     0,     0,     0,     0,    80,     0,     0,     0,     0,
       0,     0,    42,    56,   138,     0,     0,     0,     0,     0,
      22,     0,    19,     0,    10,    12,   154,     0,     0,   144,
      76,     0,   145,   146,   147,   148,   149,     0,     0,     0,
      88,     0,     0,     0,    35,     0,    34,     0,     0,    41,
      43,     0,    93,     0,     0,    31,    32,    21,     0,    20,
     152,     0,   142,     0,   153,    83,    84,     0,     0,     0,
       0,    38,     0,     0,     0,    33,     0,     0,     0,    24,
      23,   155,     0,    75,     0,    77,     0,     0,     0,    36,
      39,     0,     0,     0,   134,     0,     0,    52,    52,   113,
     113,   113,     0,     0,     0,     0,     0,    57,    59,    67,
     116,    68,    66,   115,   121,   122,   123,    71,    37,   156,
       0,     0,     0,    78,    85,    86,     0,     0,     0,   137,
     136,     0,     0,     0,    94,    53,     0,     0,   114,     0,
       0,     0,   113,     0,     0,     0,     0,    58,    44,    60,
       0,    26,   143,     0,    79,     0,    40,     0,   135,     0,
     153,   113,   113,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    45,   157,     0,    81,     0,     0,     0,    89,
       0,     0,     0,     0,     0,     0,     0,    61,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   109,
       0,     0,     0,     0,    25,     0,     0,    46,    95,     0,
       0,   117,   118,   124,    69,    62,     0,     0,    96,    97,
      98,    99,   100,   101,   102,   103,     0,     0,     0,     0,
       0,    27,     0,    82,    47,     0,     0,     0,     0,     0,
     127,    70,     0,     0,     0,   107,     0,     0,     0,   119,
     120,     0,   125,   126,     0,    72,   104,     0,   108,     0,
     111,     0,     0,    29,   124,   131,   128,   129,     0,   110,
       0,    28,     0,   127,   153,     0,     0,   106,     0,     0,
      73,   154,     0,   130,     0,     0,    30,   132,   133,   105,
     112
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    65,    66,    67,    68,    69,
      70,    71,   284,    72,    11,    89,    90,   150,   151,    12,
      91,    92,    44,    45,   216,    46,    52,   185,   186,   266,
      13,   187,   188,   267,    14,   207,   147,    51,   190,    15,
      16,   193,   247,   194,   249,   195,   196,   219,   197,   320,
     335,   346,   347,   173,   174,   250,   106,    48,   107,   132
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -253
static const yytype_int16 yypact[] =
{
      58,    81,   102,   141,   167,   192,   209,   133,  -253,    16,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,   205,   119,   216,
     218,    -3,    57,   230,  -253,  -253,  -253,    81,   115,   228,
     255,   144,   275,   115,  -253,   273,   501,  -253,   435,  -253,
    -253,  -253,   115,   115,  -253,  -253,  -253,    89,  -253,   157,
    -253,    99,   443,  -253,   278,  -253,   490,   310,   287,   298,
     304,   308,   316,   315,     0,   159,  -253,  -253,  -253,  -253,
    -253,  -253,  -253,   115,   115,   132,   311,   441,   115,   115,
     115,   115,   115,   -13,   317,  -253,   327,    25,    71,   321,
      -4,     2,  -253,  -253,  -253,   343,   345,   342,   346,   346,
     346,   255,  -253,   358,  -253,  -253,   511,    23,   498,  -253,
     463,    73,    50,    50,   311,   311,   311,   352,    53,   365,
    -253,   395,    -8,   395,  -253,   374,  -253,   375,    81,  -253,
    -253,   380,   386,   387,   408,  -253,  -253,  -253,   195,  -253,
    -253,   115,   400,   115,   115,  -253,  -253,   210,   405,    81,
      -6,  -253,   160,   448,     3,  -253,   115,   415,   413,  -253,
    -253,   511,   115,   511,   178,  -253,   163,   414,   449,  -253,
    -253,    81,    81,   194,  -253,    81,    81,    81,    81,   423,
     423,    -9,   422,   429,   440,   448,   196,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,   511,
     444,   346,   506,  -253,  -253,  -253,    53,   241,    81,  -253,
    -253,   442,   160,   470,   447,  -253,   346,   346,  -253,   446,
     456,   460,   423,   115,   346,   471,   212,  -253,  -253,  -253,
     115,   484,  -253,   254,  -253,   464,  -253,   448,  -253,   275,
     115,   423,   423,   475,   475,   472,   466,    38,   468,    39,
     467,   488,  -253,   511,   503,  -253,    53,   448,   246,  -253,
     213,   495,   502,   258,   274,   346,    30,  -253,   472,   510,
     115,   115,   115,   115,   115,   115,   115,   115,   522,  -253,
     523,   475,   524,    76,   499,   259,   305,  -253,  -253,   475,
     475,  -253,  -253,   151,  -253,  -253,    62,   475,  -253,  -253,
     511,   511,   511,   511,   511,   511,   475,   475,   322,   346,
     520,  -253,   534,  -253,  -253,   334,   350,   561,   536,   537,
     558,  -253,   362,   378,   390,  -253,    63,   531,   182,  -253,
    -253,   346,  -253,  -253,   540,  -253,   551,   532,  -253,   535,
    -253,   346,   543,  -253,   191,   538,   533,  -253,     8,  -253,
     475,  -253,   539,   558,   115,   540,   475,  -253,   406,   346,
    -253,   262,   240,  -253,   418,   541,  -253,  -253,  -253,  -253,
    -253
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -253,  -253,  -253,  -253,   573,  -253,   518,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,   494,  -253,   465,   -20,  -253,
    -253,   496,     1,  -253,   411,  -253,  -253,   349,  -162,   323,
       6,  -181,  -154,  -252,  -146,  -253,  -196,   489,  -253,  -144,
    -137,  -253,    65,   244,   284,  -253,  -253,  -156,  -253,   250,
     242,  -253,   243,  -253,   384,   -21,   -25,   -27,  -143,  -253
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -49
static const yytype_int16 yytable[] =
{
      34,   164,    18,    47,   227,    55,   125,   189,    56,   191,
     233,    32,   128,   149,   295,    26,   192,    75,    76,     1,
       2,   117,   149,   226,   220,   221,   218,     4,    37,     5,
     118,   182,   229,   152,   102,   153,    33,    53,   169,   189,
     126,   191,    33,   103,   295,    33,   129,   198,   192,   108,
       6,   356,   111,   112,   113,   114,   115,   116,    93,    17,
     285,     1,     2,   278,   265,   140,   246,     3,   121,     4,
     141,     5,   229,    35,   294,   258,   227,   135,   136,   137,
     269,   263,   264,   279,    33,   261,   262,   339,   122,   145,
     146,   189,     6,   191,    77,   286,   265,   260,   270,   271,
     192,    80,    81,    82,   229,    21,   321,   340,    33,   229,
     229,   189,   143,   191,   123,    17,   161,   310,   163,   308,
     192,    33,    78,    79,    80,    81,    82,   315,   316,   122,
     170,   199,   229,    24,   170,   322,    19,   202,    78,    79,
      80,    81,    82,    85,   323,   324,    86,     1,     2,    38,
     168,    39,    40,    41,   229,     4,    42,     5,   317,    28,
     222,   229,   229,   171,    29,    43,   318,   319,   229,   229,
     229,   172,   209,   210,   109,    20,   213,   214,   215,   215,
     231,    78,    79,    80,    81,    82,    58,    59,   358,    60,
      61,    62,    63,    64,   364,   241,   242,    83,   248,   204,
     205,    21,    84,   104,   229,   253,   318,   319,   175,   236,
     229,   362,   259,   176,   177,   178,   179,   180,   181,   182,
     203,   183,   184,   342,   175,   141,    22,    33,   280,   176,
     177,   178,   179,   180,   181,   182,   211,   183,   184,   160,
     228,   212,    86,    23,   293,   248,   248,   300,   301,   302,
     303,   304,   305,    27,   165,   288,   252,   166,   175,    30,
     141,    31,   311,   176,   177,   178,   179,   180,   181,   182,
     175,   183,   184,    36,    49,   176,   177,   178,   179,   180,
     181,   182,   368,   183,   184,   234,   175,   141,   235,    50,
     287,   176,   177,   178,   179,   180,   181,   182,   255,   183,
     184,   166,   291,   313,   367,   280,   166,   343,    57,    54,
     344,    78,    79,    80,    81,    82,    95,   175,   292,    73,
     351,    96,   176,   177,   178,   179,   180,   181,   182,   361,
     183,   184,    97,    98,   175,   298,   299,    99,   366,   176,
     177,   178,   179,   180,   181,   182,   175,   183,   184,   314,
     100,   176,   177,   178,   179,   180,   181,   182,   101,   183,
     184,   120,   175,   119,    82,   124,   325,   176,   177,   178,
     179,   180,   181,   182,   175,   183,   184,   131,   329,   176,
     177,   178,   179,   180,   181,   182,   133,   183,   184,   134,
     175,    33,   139,   144,   330,   176,   177,   178,   179,   180,
     181,   182,   175,   183,   184,   148,   336,   176,   177,   178,
     179,   180,   181,   182,   149,   183,   184,   121,   175,   155,
     156,   158,   337,   176,   177,   178,   179,   180,   181,   182,
     175,   183,   184,   157,   338,   176,   177,   178,   179,   180,
     181,   182,   159,   183,   184,   162,     1,     2,   167,   200,
     365,     1,     2,    87,    88,   201,     5,   206,   218,     4,
     175,     5,   369,   223,   208,   176,   177,   178,   179,   180,
     181,   182,   224,   183,   184,   110,    73,    39,    40,    41,
      74,   225,    42,    27,   230,   237,   239,   175,   240,   243,
     254,    43,   176,   177,   178,   179,   180,   181,   182,   244,
     183,   184,   -48,   245,    73,   251,   265,   256,    74,   268,
     281,    27,   -48,   -48,   -48,   -48,   -48,    78,    79,    80,
      81,    82,   272,   273,   274,   275,   276,   277,    58,    59,
     282,    60,    61,    62,    63,    64,    94,   283,   289,    78,
      79,    80,    81,    82,   142,   290,   312,    78,    79,    80,
      81,    82,   232,   297,   327,    78,    79,    80,    81,    82,
      78,    79,    80,    81,    82,   306,   307,   309,   328,   331,
     332,   333,   334,   341,   345,   348,   349,   352,   350,   354,
     355,   359,    25,   105,   127,   370,   257,   130,   154,   217,
     138,   296,   357,   326,   353,   360,   238,     0,   363
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-253)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      21,   144,     1,    28,   185,    32,    10,   153,    33,   153,
     206,    14,    10,    19,   266,     9,   153,    42,    43,     3,
       4,    34,    19,   185,   180,   181,    35,    11,    27,    13,
      43,    23,   186,    41,    34,    43,    45,    31,    44,   185,
      44,   185,    45,    64,   296,    45,    44,    44,   185,    74,
      34,    43,    77,    78,    79,    80,    81,    82,    52,    34,
     256,     3,     4,    24,    34,    42,   222,     9,    43,    11,
      47,    13,   226,    16,    44,   237,   257,    98,    99,   100,
      42,   243,   244,    44,    45,   241,   242,    24,    87,    36,
      37,   237,    34,   237,     5,   257,    34,   240,    60,    61,
     237,    51,    52,    53,   258,    34,    44,    44,    45,   263,
     264,   257,    39,   257,    43,    34,   141,    41,   143,   281,
     257,    45,    49,    50,    51,    52,    53,   289,   290,   128,
     150,   156,   286,     0,   154,   297,    34,   162,    49,    50,
      51,    52,    53,    44,   306,   307,    47,     3,     4,    34,
     149,    36,    37,    38,   308,    11,    41,    13,     7,    40,
     181,   315,   316,     3,    45,    50,    15,    16,   322,   323,
     324,    11,   171,   172,    42,    34,   175,   176,   177,   178,
     201,    49,    50,    51,    52,    53,    27,    28,   350,    30,
      31,    32,    33,    34,   356,   216,   217,    40,   223,    36,
      37,    34,    45,    44,   358,   230,    15,    16,    12,   208,
     364,   354,   239,    17,    18,    19,    20,    21,    22,    23,
      42,    25,    26,    41,    12,    47,    34,    45,   249,    17,
      18,    19,    20,    21,    22,    23,    42,    25,    26,    44,
      44,    47,    47,    34,   265,   270,   271,   272,   273,   274,
     275,   276,   277,    48,    44,    42,    44,    47,    12,    43,
      47,    43,   283,    17,    18,    19,    20,    21,    22,    23,
      12,    25,    26,    43,    46,    17,    18,    19,    20,    21,
      22,    23,    42,    25,    26,    44,    12,    47,    47,    34,
      44,    17,    18,    19,    20,    21,    22,    23,    44,    25,
      26,    47,    44,    44,    42,   326,    47,   328,    35,    34,
     331,    49,    50,    51,    52,    53,     6,    12,    44,    41,
     341,    34,    17,    18,    19,    20,    21,    22,    23,   354,
      25,    26,    34,    29,    12,   270,   271,    29,   359,    17,
      18,    19,    20,    21,    22,    23,    12,    25,    26,    44,
      34,    17,    18,    19,    20,    21,    22,    23,    43,    25,
      26,    34,    12,    46,    53,    44,    44,    17,    18,    19,
      20,    21,    22,    23,    12,    25,    26,    34,    44,    17,
      18,    19,    20,    21,    22,    23,    41,    25,    26,    47,
      12,    45,    34,    41,    44,    17,    18,    19,    20,    21,
      22,    23,    12,    25,    26,    40,    44,    17,    18,    19,
      20,    21,    22,    23,    19,    25,    26,    43,    12,    44,
      40,    34,    44,    17,    18,    19,    20,    21,    22,    23,
      12,    25,    26,    47,    44,    17,    18,    19,    20,    21,
      22,    23,    34,    25,    26,    45,     3,     4,    43,    34,
      44,     3,     4,    10,    11,    42,    13,    43,    35,    11,
      12,    13,    44,    41,    15,    17,    18,    19,    20,    21,
      22,    23,    43,    25,    26,    34,    41,    36,    37,    38,
      45,    41,    41,    48,    40,    43,    16,    12,    41,    43,
       6,    50,    17,    18,    19,    20,    21,    22,    23,    43,
      25,    26,    39,    43,    41,    34,    34,    43,    45,    43,
      43,    48,    49,    50,    51,    52,    53,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    27,    28,
      42,    30,    31,    32,    33,    34,    46,    34,    43,    49,
      50,    51,    52,    53,    46,    43,    47,    49,    50,    51,
      52,    53,    46,    43,    34,    49,    50,    51,    52,    53,
      49,    50,    51,    52,    53,    43,    43,    43,    34,     8,
      34,    34,    14,    42,    34,    24,    44,    34,    43,    41,
      47,    42,     9,    65,    90,    44,   237,    91,   123,   178,
     101,   268,   348,   309,   344,   353,   212,    -1,   355
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     9,    11,    13,    34,    64,    65,    66,
      67,    77,    82,    93,    97,   102,   103,    34,    85,    34,
      34,    34,    34,    34,     0,    67,    93,    48,    40,    45,
      43,    43,    14,    45,   118,    16,    43,    85,    34,    36,
      37,    38,    41,    50,    85,    86,    88,   119,   120,    46,
      34,   100,    89,    93,    34,   120,   119,    35,    27,    28,
      30,    31,    32,    33,    34,    68,    69,    70,    71,    72,
      73,    74,    76,    41,    45,   119,   119,     5,    49,    50,
      51,    52,    53,    40,    45,    44,    47,    10,    11,    78,
      79,    83,    84,    93,    46,     6,    34,    34,    29,    29,
      34,    43,    34,   118,    44,    69,   119,   121,   119,    42,
      34,   119,   119,   119,   119,   119,   119,    34,    43,    46,
      34,    43,    85,    43,    44,    10,    44,    78,    10,    44,
      84,    34,   122,    41,    47,   118,   118,   118,   100,    34,
      42,    47,    46,    39,    41,    36,    37,    99,    40,    19,
      80,    81,    41,    43,    80,    44,    40,    47,    34,    34,
      44,   119,    45,   119,   121,    44,    47,    43,    85,    44,
      81,     3,    11,   116,   117,    12,    17,    18,    19,    20,
      21,    22,    23,    25,    26,    90,    91,    94,    95,    97,
     101,   102,   103,   104,   106,   108,   109,   111,    44,   119,
      34,    42,   119,    42,    36,    37,    43,    98,    15,    85,
      85,    42,    47,    85,    85,    85,    87,    87,    35,   110,
     110,   110,   118,    41,    43,    41,    91,    94,    44,    95,
      40,   118,    46,    99,    44,    47,    85,    43,   117,    16,
      41,   118,   118,    43,    43,    43,   110,   105,   119,   107,
     118,    34,    44,   119,     6,    44,    43,    90,    91,   120,
     121,   110,   110,    91,    91,    34,    92,    96,    43,    42,
      60,    61,    54,    55,    56,    57,    58,    59,    24,    44,
     118,    43,    42,    34,    75,    99,    91,    44,    42,    43,
      43,    44,    44,   118,    44,    96,    92,    43,   105,   105,
     119,   119,   119,   119,   119,   119,    43,    43,    91,    43,
      41,   118,    47,    44,    44,    91,    91,     7,    15,    16,
     112,    44,    91,    91,    91,    44,   107,    34,    34,    44,
      44,     8,    34,    34,    14,   113,    44,    44,    44,    24,
      44,    42,    41,   118,   118,    34,   114,   115,    24,    44,
      43,   118,    34,   112,    41,    47,    43,   106,    91,    42,
     113,   119,   121,   115,    91,    44,   118,    42,    42,    44,
      44
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
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
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
      int yyn = yypact[*yyssp];
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
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
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
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
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
      yychar = YYLEX;
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 188 "parser/AspenParser.y"
    {
    globalmach = (yyvsp[(1) - (1)].mach);
}
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 192 "parser/AspenParser.y"
    {
    globalapp = (yyvsp[(1) - (1)].app);
}
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 196 "parser/AspenParser.y"
    {
    globalmap = (yyvsp[(1) - (1)].map);
}
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 203 "parser/AspenParser.y"
    {
    (yyval.mach) = new ASTMachModel((yyvsp[(1) - (1)].machcontent).first, (yyvsp[(1) - (1)].machcontent).second);
    (yyvsp[(1) - (1)].machcontent).first.release();
    (yyvsp[(1) - (1)].machcontent).second.release();
}
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 212 "parser/AspenParser.y"
    {
    // allocate/clear both vectors, because we don't know
    // if the next one might be a component
    (yyval.machcontent).first.clear();
    (yyval.machcontent).second.clear();
    (yyval.machcontent).first.push_back((yyvsp[(1) - (1)].stmt));
}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 220 "parser/AspenParser.y"
    {
    // allocate/clear both vectors, because we don't know
    // if the next one might be a globalstmt
    (yyval.machcontent).first.clear();
    (yyval.machcontent).second.clear();
    (yyval.machcontent).second.push_back((yyvsp[(1) - (1)].component));
}
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 228 "parser/AspenParser.y"
    {
    (yyval.machcontent) = (yyvsp[(1) - (2)].machcontent);
    (yyval.machcontent).first.push_back((yyvsp[(2) - (2)].stmt));
}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 233 "parser/AspenParser.y"
    {
    (yyval.machcontent) = (yyvsp[(1) - (2)].machcontent);
    (yyval.machcontent).second.push_back((yyvsp[(2) - (2)].component));
}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 241 "parser/AspenParser.y"
    {
    (yyval.component) = new ASTMachComponent((yyvsp[(1) - (5)].str), (yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].astnodelist));
    (yyvsp[(4) - (5)].astnodelist).release();
}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 248 "parser/AspenParser.y"
    { (yyval.astnodelist).clear(); (yyval.astnodelist).push_back((yyvsp[(1) - (1)].astnode)); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 249 "parser/AspenParser.y"
    { (yyval.astnodelist)=(yyvsp[(1) - (2)].astnodelist); (yyval.astnodelist).push_back((yyvsp[(2) - (2)].astnode)); }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 253 "parser/AspenParser.y"
    { (yyval.astnode) = (yyvsp[(1) - (1)].property); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 254 "parser/AspenParser.y"
    { (yyval.astnode) = (yyvsp[(1) - (1)].connect); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 255 "parser/AspenParser.y"
    { (yyval.astnode) = (yyvsp[(1) - (1)].resource); }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 256 "parser/AspenParser.y"
    { (yyval.astnode) = (yyvsp[(1) - (1)].power); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 257 "parser/AspenParser.y"
    { (yyval.astnode) = (yyvsp[(1) - (1)].conflict); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 258 "parser/AspenParser.y"
    { (yyval.astnode) = (yyvsp[(1) - (1)].subcomponent); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 262 "parser/AspenParser.y"
    { (yyval.subcomponent) = new ASTSubComponent((yyvsp[(1) - (2)].str), (yyvsp[(2) - (2)].str), NULL); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 263 "parser/AspenParser.y"
    { (yyval.subcomponent) = new ASTSubComponent((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str), (yyvsp[(2) - (3)].expr)); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 268 "parser/AspenParser.y"
    { (yyval.property) = new ASTMachProperty((yyvsp[(2) - (3)].str), (yyvsp[(3) - (3)].expr)); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 269 "parser/AspenParser.y"
    { (yyval.property) = new ASTMachProperty((yyvsp[(2) - (2)].str), NULL); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 273 "parser/AspenParser.y"
    { (yyval.connect) = new ASTMachConnectivity((yyvsp[(3) - (4)].stringlist)); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 278 "parser/AspenParser.y"
    {
    (yyval.conflict) = new ASTResourceConflict((yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].str));
}
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 285 "parser/AspenParser.y"
    {
    (yyval.resource) = new ASTMachResource((yyvsp[(2) - (8)].str), (yyvsp[(4) - (8)].str), (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].traitdeflist));
    (yyvsp[(8) - (8)].traitdeflist).release();
}
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 290 "parser/AspenParser.y"
    {
    ParseVector<ASTTraitDefinition*> empty;
    empty.clear();
    (yyval.resource) = new ASTMachResource((yyvsp[(2) - (6)].str), (yyvsp[(4) - (6)].str), (yyvsp[(6) - (6)].expr), empty);
    empty.release();
}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 300 "parser/AspenParser.y"
    {
    (yyval.traitdeflist).clear();
    (yyval.traitdeflist).push_back(new ASTTraitDefinition((yyvsp[(1) - (2)].str), (yyvsp[(2) - (2)].expr)));
}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 305 "parser/AspenParser.y"
    {
    (yyval.traitdeflist).clear();
    (yyval.traitdeflist).push_back(new ASTTraitDefinition((yyvsp[(1) - (5)].str), (yyvsp[(3) - (5)].str), (yyvsp[(5) - (5)].expr)));
}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 310 "parser/AspenParser.y"
    {
    (yyval.traitdeflist) = (yyvsp[(1) - (4)].traitdeflist);
    (yyval.traitdeflist).push_back(new ASTTraitDefinition((yyvsp[(3) - (4)].str), (yyvsp[(4) - (4)].expr)));
}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 315 "parser/AspenParser.y"
    {
    (yyval.traitdeflist) = (yyvsp[(1) - (7)].traitdeflist);
    (yyval.traitdeflist).push_back(new ASTTraitDefinition((yyvsp[(3) - (7)].str), (yyvsp[(5) - (7)].str), (yyvsp[(7) - (7)].expr)));
}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 322 "parser/AspenParser.y"
    { (yyval.power) = new ASTMachPower((yyvsp[(3) - (3)].expr), NULL); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 323 "parser/AspenParser.y"
    { (yyval.power) = new ASTMachPower(NULL, (yyvsp[(3) - (3)].expr)); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 330 "parser/AspenParser.y"
    {
  (yyval.map) = new ASTMapModel((yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].stmtlist), (yyvsp[(5) - (7)].localmap), (yyvsp[(6) - (7)].localmap));
  (yyvsp[(4) - (7)].stmtlist).release();
}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 335 "parser/AspenParser.y"
    {
  (yyval.map) = new ASTMapModel((yyvsp[(2) - (6)].str), (yyvsp[(4) - (6)].stmtlist), (yyvsp[(5) - (6)].localmap), NULL);
  (yyvsp[(4) - (6)].stmtlist).release();
}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 340 "parser/AspenParser.y"
    {
  (yyval.map) = new ASTMapModel((yyvsp[(2) - (6)].str), (yyvsp[(4) - (6)].stmtlist), NULL, (yyvsp[(5) - (6)].localmap));
  (yyvsp[(4) - (6)].stmtlist).release();
}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 348 "parser/AspenParser.y"
    {
  (yyval.localmap) = new ASTMap(0, (yyvsp[(3) - (4)].mapstmtlist));
  (yyvsp[(3) - (4)].mapstmtlist).release();
}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 355 "parser/AspenParser.y"
    {
  (yyval.localmap) = new ASTMap(1, (yyvsp[(3) - (4)].mapstmtlist));
  (yyvsp[(3) - (4)].mapstmtlist).release();
}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 361 "parser/AspenParser.y"
    { (yyval.mapstmtlist).clear(); (yyval.mapstmtlist).push_back((yyvsp[(1) - (1)].mapstmt)); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 362 "parser/AspenParser.y"
    { (yyval.mapstmtlist) = (yyvsp[(1) - (2)].mapstmtlist); (yyval.mapstmtlist).push_back((yyvsp[(2) - (2)].mapstmt)); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 368 "parser/AspenParser.y"
    {
  (yyval.mapstmt) = new ASTMapStatement((yyvsp[(2) - (4)].ident),(yyvsp[(4) - (4)].ident));
}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 381 "parser/AspenParser.y"
    {
    (yyval.app) = new ASTAppModel((yyvsp[(2) - (6)].str), (yyvsp[(4) - (6)].stmtlist), (yyvsp[(5) - (6)].kernellist));
    (yyvsp[(4) - (6)].stmtlist).release();
    (yyvsp[(5) - (6)].kernellist).release();
}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 389 "parser/AspenParser.y"
    { (yyval.kernellist).clear(); (yyval.kernellist).push_back((yyvsp[(1) - (1)].kernel)); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 390 "parser/AspenParser.y"
    { (yyval.kernellist) = (yyvsp[(1) - (2)].kernellist); (yyval.kernellist).push_back((yyvsp[(2) - (2)].kernel)); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 395 "parser/AspenParser.y"
    {
    ParseVector<ASTKernelArgDecl*> empty1;
    empty1.clear();
    ParseVector<ASTStatement*> empty2;
    empty2.clear();
    (yyval.kernel) = new ASTKernel((yyvsp[(2) - (5)].ident), (yyvsp[(4) - (5)].controlstmtlist), empty1, empty2);
    (yyvsp[(4) - (5)].controlstmtlist).release();
    empty1.release();
    empty2.release();
}
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 406 "parser/AspenParser.y"
    {
    ParseVector<ASTKernelArgDecl*> empty;
    empty.clear();
    (yyval.kernel) = new ASTKernel((yyvsp[(2) - (6)].ident), (yyvsp[(5) - (6)].controlstmtlist), empty, (yyvsp[(4) - (6)].stmtlist));
    (yyvsp[(5) - (6)].controlstmtlist).release();
    (yyvsp[(4) - (6)].stmtlist).release();
    empty.release();
}
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 415 "parser/AspenParser.y"
    {
    ParseVector<ASTStatement*> empty;
    empty.clear();
    (yyval.kernel) = new ASTKernel((yyvsp[(2) - (8)].ident), (yyvsp[(7) - (8)].controlstmtlist), (yyvsp[(4) - (8)].argdecllist), empty);
    (yyvsp[(4) - (8)].argdecllist).release();
    (yyvsp[(7) - (8)].controlstmtlist).release();
}
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 423 "parser/AspenParser.y"
    {
    (yyval.kernel) = new ASTKernel((yyvsp[(2) - (9)].ident), (yyvsp[(8) - (9)].controlstmtlist), (yyvsp[(4) - (9)].argdecllist), (yyvsp[(7) - (9)].stmtlist));
    (yyvsp[(4) - (9)].argdecllist).release();
    (yyvsp[(8) - (9)].controlstmtlist).release();
    (yyvsp[(7) - (9)].stmtlist).release();
}
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 431 "parser/AspenParser.y"
    { (yyval.ident) = new Identifier((yyvsp[(1) - (1)].str)); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 432 "parser/AspenParser.y"
    { (yyval.ident) = (yyvsp[(3) - (3)].ident); (yyval.ident)->Prefix((yyvsp[(1) - (3)].str)); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 435 "parser/AspenParser.y"
    { (yyval.expr) = new Integer(atol((yyvsp[(1) - (1)].str).c_str())); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 436 "parser/AspenParser.y"
    { (yyval.expr) = new Real(atof((yyvsp[(1) - (1)].str).c_str())); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 440 "parser/AspenParser.y"
    { (yyval.ident) = NULL; }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 441 "parser/AspenParser.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].ident); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 445 "parser/AspenParser.y"
    {
    double v = 1;
    if ((yyvsp[(1) - (1)].str) == "nano")  v = 1e-9;
    if ((yyvsp[(1) - (1)].str) == "micro") v = 1e-6;
    if ((yyvsp[(1) - (1)].str) == "milli") v = 1e-3;
    if ((yyvsp[(1) - (1)].str) == "kilo")  v = 1e3;
    if ((yyvsp[(1) - (1)].str) == "mega")  v = 1e6;
    if ((yyvsp[(1) - (1)].str) == "giga")  v = 1e9;
    if ((yyvsp[(1) - (1)].str) == "tera")  v = 1e12;
    if ((yyvsp[(1) - (1)].str) == "peta")  v = 1e15;
    if ((yyvsp[(1) - (1)].str) == "exa")   v = 1e18;
    (yyval.expr) = new Real(v);
}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 465 "parser/AspenParser.y"
    { (yyval.stmtlist).clear(); (yyval.stmtlist).push_back((yyvsp[(1) - (1)].stmt)); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 466 "parser/AspenParser.y"
    { (yyval.stmtlist)=(yyvsp[(1) - (2)].stmtlist); (yyval.stmtlist).push_back((yyvsp[(2) - (2)].stmt)); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 470 "parser/AspenParser.y"
    { (yyval.stmtlist).clear(); (yyval.stmtlist).push_back((yyvsp[(1) - (1)].stmt)); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 471 "parser/AspenParser.y"
    { (yyval.stmtlist)=(yyvsp[(1) - (2)].stmtlist); (yyval.stmtlist).push_back((yyvsp[(2) - (2)].stmt)); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 475 "parser/AspenParser.y"
    { (yyval.controlstmtlist).clear(); (yyval.controlstmtlist).push_back((yyvsp[(1) - (1)].controlstmt)); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 476 "parser/AspenParser.y"
    { (yyval.controlstmtlist)=(yyvsp[(1) - (2)].controlstmtlist); (yyval.controlstmtlist).push_back((yyvsp[(2) - (2)].controlstmt)); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 480 "parser/AspenParser.y"
    { (yyval.execstmtlist).clear(); (yyval.execstmtlist).push_back((yyvsp[(1) - (1)].execstmt)); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 481 "parser/AspenParser.y"
    { (yyval.execstmtlist)=(yyvsp[(1) - (2)].execstmtlist); (yyval.execstmtlist).push_back((yyvsp[(2) - (2)].execstmt)); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 489 "parser/AspenParser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 490 "parser/AspenParser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 491 "parser/AspenParser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 495 "parser/AspenParser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 496 "parser/AspenParser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 497 "parser/AspenParser.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 502 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTExecutionBlock((yyvsp[(2) - (5)].str), NULL, (yyvsp[(4) - (5)].execstmtlist));
    (yyvsp[(4) - (5)].execstmtlist).release();
}
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 507 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTExecutionBlock((yyvsp[(3) - (6)].str), (yyvsp[(2) - (6)].expr), (yyvsp[(5) - (6)].execstmtlist));
    (yyvsp[(5) - (6)].execstmtlist).release();
}
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 511 "parser/AspenParser.y"
    { (yyval.controlstmt) =  (yyvsp[(1) - (1)].controlstmt); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 521 "parser/AspenParser.y"
    {
    (yyval.execstmt) = new ASTRequiresStatement((yyvsp[(1) - (4)].str), (yyvsp[(2) - (4)].expr), NULL, (yyvsp[(3) - (4)].str), (yyvsp[(4) - (4)].traitlist));
    (yyvsp[(4) - (4)].traitlist).release();
}
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 526 "parser/AspenParser.y"
    {
    (yyval.execstmt) = new ASTRequiresStatement((yyvsp[(1) - (7)].str), (yyvsp[(2) - (7)].expr), (yyvsp[(5) - (7)].expr), (yyvsp[(6) - (7)].str), (yyvsp[(7) - (7)].traitlist));
    (yyvsp[(7) - (7)].traitlist).release();
}
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 537 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTAssignStatement((yyvsp[(2) - (4)].ident),(yyvsp[(4) - (4)].expr));
}
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 541 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTAssignRangeStatement((yyvsp[(2) - (8)].ident), (yyvsp[(4) - (8)].expr), (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].expr));
}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 545 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTAssignEnumStatement((yyvsp[(2) - (6)].ident), (yyvsp[(4) - (6)].expr), (yyvsp[(6) - (6)].str));
}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 549 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTAssignStatement((yyvsp[(2) - (8)].ident), new Array((yyvsp[(7) - (8)].valuelist)));
    (yyvsp[(7) - (8)].valuelist).release();
}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 554 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTAssignStatement((yyvsp[(2) - (9)].ident), new FunctionCall((yyvsp[(6) - (9)].str),(yyvsp[(8) - (9)].exprlist)));
    (yyvsp[(8) - (9)].exprlist).release();
}
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 559 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTAssignStatement((yyvsp[(2) - (10)].ident), new Table((yyvsp[(9) - (10)].valuelist)));
    (yyvsp[(9) - (10)].valuelist).release();
}
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 569 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTEnumDeclaration((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].stringlist));
}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 576 "parser/AspenParser.y"
    {
    (yyval.valuelist).clear();
    (yyval.valuelist).append((yyvsp[(2) - (3)].valuelist));
    (yyvsp[(2) - (3)].valuelist).release();
}
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 582 "parser/AspenParser.y"
    {
    (yyval.valuelist) = (yyvsp[(1) - (5)].valuelist);

    // add two sentinels so we know where to break
    (yyval.valuelist).push_back(1e37);
    (yyval.valuelist).push_back(-1e37);

    (yyval.valuelist).append((yyvsp[(4) - (5)].valuelist));
    (yyvsp[(4) - (5)].valuelist).release();
}
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 596 "parser/AspenParser.y"
    {
    (yyval.valuelist).clear();
    (yyval.valuelist).push_back(atol((yyvsp[(1) - (1)].str).c_str()));
}
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 601 "parser/AspenParser.y"
    {
    (yyval.valuelist).clear();
    (yyval.valuelist).push_back(atof((yyvsp[(1) - (1)].str).c_str()));
}
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 606 "parser/AspenParser.y"
    {
    (yyval.valuelist) = (yyvsp[(1) - (3)].valuelist);
    (yyval.valuelist).push_back(atol((yyvsp[(3) - (3)].str).c_str()));
}
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 611 "parser/AspenParser.y"
    {
    (yyval.valuelist) = (yyvsp[(1) - (3)].valuelist);
    (yyval.valuelist).push_back(atof((yyvsp[(3) - (3)].str).c_str()));
}
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 619 "parser/AspenParser.y"
    {
    (yyval.stringlist).clear();
    (yyval.stringlist).push_back((yyvsp[(1) - (1)].str));
}
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 624 "parser/AspenParser.y"
    {
    (yyval.stringlist) = (yyvsp[(1) - (3)].stringlist);
    (yyval.stringlist).push_back((yyvsp[(3) - (3)].str));
}
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 632 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTSampleStatement((yyvsp[(2) - (4)].ident),(yyvsp[(4) - (4)].call));
}
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 639 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTDataStatement((yyvsp[(2) - (4)].str),NULL,(yyvsp[(4) - (4)].call));
}
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 643 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTDataStatement((yyvsp[(2) - (3)].str),(yyvsp[(3) - (3)].expr),NULL);
}
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 650 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTImportStatement((yyvsp[(2) - (4)].str),(yyvsp[(4) - (4)].str));
}
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 654 "parser/AspenParser.y"
    {
    (yyval.stmt) = new ASTImportStatement((yyvsp[(2) - (6)].str),(yyvsp[(4) - (6)].str),(yyvsp[(6) - (6)].assignlist));
    (yyvsp[(6) - (6)].assignlist).release();
}
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 662 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlKernelCallStatement((yyvsp[(2) - (2)].ident));
}
    break;

  case 95:
/* Line 1792 of yacc.c  */
#line 666 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlKernelCallStatement((yyvsp[(2) - (5)].ident), (yyvsp[(4) - (5)].exprlist));
    (yyvsp[(4) - (5)].exprlist).release();
}
    break;

  case 96:
/* Line 1792 of yacc.c  */
#line 673 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison("and",(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 674 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison("or",(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 675 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison("==",(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 676 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison("!=",(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 677 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison("<", (yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 101:
/* Line 1792 of yacc.c  */
#line 678 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison(">", (yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 102:
/* Line 1792 of yacc.c  */
#line 679 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison("<=",(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 680 "parser/AspenParser.y"
    { (yyval.expr) = new Comparison(">=",(yyvsp[(1) - (3)].expr),(yyvsp[(3) - (3)].expr)); }
    break;

  case 104:
/* Line 1792 of yacc.c  */
#line 685 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlIfThenStatement((yyvsp[(3) - (7)].expr), new ASTControlSequentialStatement("", (yyvsp[(6) - (7)].controlstmtlist)), NULL);
    (yyvsp[(6) - (7)].controlstmtlist).release();
}
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 690 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlIfThenStatement((yyvsp[(3) - (11)].expr), new ASTControlSequentialStatement("", (yyvsp[(6) - (11)].controlstmtlist)), new ASTControlSequentialStatement("", (yyvsp[(10) - (11)].controlstmtlist)));
    (yyvsp[(6) - (11)].controlstmtlist).release();
    (yyvsp[(10) - (11)].controlstmtlist).release();
}
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 696 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlIfThenStatement((yyvsp[(3) - (9)].expr), new ASTControlSequentialStatement("", (yyvsp[(6) - (9)].controlstmtlist)), (yyvsp[(9) - (9)].controlstmt));
    (yyvsp[(6) - (9)].controlstmtlist).release();
}
    break;

  case 107:
/* Line 1792 of yacc.c  */
#line 704 "parser/AspenParser.y"
    {
    (yyval.caselist).clear();
    (yyval.caselist).push_back(new ASTCaseItem((yyvsp[(1) - (4)].expr), new ASTControlSequentialStatement("", (yyvsp[(3) - (4)].controlstmtlist))));
    (yyvsp[(3) - (4)].controlstmtlist).release();
}
    break;

  case 108:
/* Line 1792 of yacc.c  */
#line 710 "parser/AspenParser.y"
    {
    (yyval.caselist) = (yyvsp[(1) - (5)].caselist);
    (yyval.caselist).push_back(new ASTCaseItem((yyvsp[(2) - (5)].expr), new ASTControlSequentialStatement("", (yyvsp[(4) - (5)].controlstmtlist))));
    (yyvsp[(4) - (5)].controlstmtlist).release();
}
    break;

  case 109:
/* Line 1792 of yacc.c  */
#line 719 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlProbabilityStatement((yyvsp[(3) - (4)].caselist), NULL);
    (yyvsp[(3) - (4)].caselist).release();
}
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 724 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlProbabilityStatement((yyvsp[(3) - (8)].caselist), new ASTControlSequentialStatement("", (yyvsp[(6) - (8)].controlstmtlist)));
    (yyvsp[(3) - (8)].caselist).release();
}
    break;

  case 111:
/* Line 1792 of yacc.c  */
#line 732 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlSwitchStatement(new Identifier((yyvsp[(3) - (7)].str)), (yyvsp[(6) - (7)].caselist), NULL);
    (yyvsp[(6) - (7)].caselist).release();
}
    break;

  case 112:
/* Line 1792 of yacc.c  */
#line 737 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlSwitchStatement(new Identifier((yyvsp[(3) - (11)].str)), (yyvsp[(6) - (11)].caselist), new ASTControlSequentialStatement("", (yyvsp[(9) - (11)].controlstmtlist)));
    (yyvsp[(6) - (11)].caselist).release();
}
    break;

  case 113:
/* Line 1792 of yacc.c  */
#line 744 "parser/AspenParser.y"
    { (yyval.str) = ""; }
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 745 "parser/AspenParser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 115:
/* Line 1792 of yacc.c  */
#line 750 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = (yyvsp[(1) - (1)].controlstmt);
}
    break;

  case 116:
/* Line 1792 of yacc.c  */
#line 754 "parser/AspenParser.y"
    {
    (yyval.controlstmt) =  (yyvsp[(1) - (1)].controlstmt);
}
    break;

  case 117:
/* Line 1792 of yacc.c  */
#line 758 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlParallelStatement((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].controlstmtlist));
    (yyvsp[(4) - (5)].controlstmtlist).release();
}
    break;

  case 118:
/* Line 1792 of yacc.c  */
#line 763 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = new ASTControlSequentialStatement((yyvsp[(2) - (5)].str), (yyvsp[(4) - (5)].controlstmtlist));
    (yyvsp[(4) - (5)].controlstmtlist).release();
}
    break;

  case 119:
/* Line 1792 of yacc.c  */
#line 768 "parser/AspenParser.y"
    {
    if ((yyvsp[(6) - (7)].controlstmtlist).size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ((yyvsp[(6) - (7)].controlstmtlist).size() == 1)
        (yyval.controlstmt) = new ASTControlIterateStatement((yyvsp[(4) - (7)].str), (yyvsp[(2) - (7)].ident), (yyvsp[(3) - (7)].expr), (yyvsp[(6) - (7)].controlstmtlist)[0]);
    else // size > 1
        (yyval.controlstmt) = new ASTControlIterateStatement((yyvsp[(4) - (7)].str), (yyvsp[(2) - (7)].ident), (yyvsp[(3) - (7)].expr), new ASTControlSequentialStatement("", (yyvsp[(6) - (7)].controlstmtlist)));
    (yyvsp[(6) - (7)].controlstmtlist).release();
}
    break;

  case 120:
/* Line 1792 of yacc.c  */
#line 778 "parser/AspenParser.y"
    {
    if ((yyvsp[(6) - (7)].controlstmtlist).size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ((yyvsp[(6) - (7)].controlstmtlist).size() == 1)
        (yyval.controlstmt) = new ASTControlMapStatement((yyvsp[(4) - (7)].str), (yyvsp[(2) - (7)].ident), (yyvsp[(3) - (7)].expr), (yyvsp[(6) - (7)].controlstmtlist)[0]);
    else // size > 1
        (yyval.controlstmt) = new ASTControlMapStatement((yyvsp[(4) - (7)].str), (yyvsp[(2) - (7)].ident), (yyvsp[(3) - (7)].expr), new ASTControlSequentialStatement("", (yyvsp[(6) - (7)].controlstmtlist)));
    (yyvsp[(6) - (7)].controlstmtlist).release();
}
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 788 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = (yyvsp[(1) - (1)].controlstmt);
}
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 792 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = (yyvsp[(1) - (1)].controlstmt);
}
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 796 "parser/AspenParser.y"
    {
    (yyval.controlstmt) = (yyvsp[(1) - (1)].controlstmt);
}
    break;

  case 124:
/* Line 1792 of yacc.c  */
#line 802 "parser/AspenParser.y"
    { (yyval.str) = ""; }
    break;

  case 125:
/* Line 1792 of yacc.c  */
#line 803 "parser/AspenParser.y"
    { (yyval.str) = (yyvsp[(2) - (2)].str); }
    break;

  case 126:
/* Line 1792 of yacc.c  */
#line 804 "parser/AspenParser.y"
    { (yyval.str) = (yyvsp[(2) - (2)].str); }
    break;

  case 127:
/* Line 1792 of yacc.c  */
#line 808 "parser/AspenParser.y"
    { (yyval.traitlist).clear(); }
    break;

  case 128:
/* Line 1792 of yacc.c  */
#line 809 "parser/AspenParser.y"
    { (yyval.traitlist) = (yyvsp[(2) - (2)].traitlist); }
    break;

  case 129:
/* Line 1792 of yacc.c  */
#line 813 "parser/AspenParser.y"
    { (yyval.traitlist).clear(); (yyval.traitlist).push_back((yyvsp[(1) - (1)].trait)); }
    break;

  case 130:
/* Line 1792 of yacc.c  */
#line 814 "parser/AspenParser.y"
    { (yyval.traitlist)=(yyvsp[(1) - (3)].traitlist); (yyval.traitlist).push_back((yyvsp[(3) - (3)].trait)); }
    break;

  case 131:
/* Line 1792 of yacc.c  */
#line 818 "parser/AspenParser.y"
    { (yyval.trait) = new ASTTrait((yyvsp[(1) - (1)].str), NULL); }
    break;

  case 132:
/* Line 1792 of yacc.c  */
#line 819 "parser/AspenParser.y"
    { (yyval.trait) = new ASTTrait((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].expr)); }
    break;

  case 133:
/* Line 1792 of yacc.c  */
#line 820 "parser/AspenParser.y"
    { (yyval.trait) = new ASTTrait((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].exprlist)); }
    break;

  case 134:
/* Line 1792 of yacc.c  */
#line 824 "parser/AspenParser.y"
    { (yyval.argdecllist).clear(); (yyval.argdecllist).push_back((yyvsp[(1) - (1)].argdecl)); }
    break;

  case 135:
/* Line 1792 of yacc.c  */
#line 825 "parser/AspenParser.y"
    { (yyval.argdecllist)=(yyvsp[(1) - (3)].argdecllist); (yyval.argdecllist).push_back((yyvsp[(3) - (3)].argdecl)); }
    break;

  case 136:
/* Line 1792 of yacc.c  */
#line 829 "parser/AspenParser.y"
    { (yyval.argdecl) = new ASTKernelArgDecl("data", (yyvsp[(2) - (2)].ident)); }
    break;

  case 137:
/* Line 1792 of yacc.c  */
#line 830 "parser/AspenParser.y"
    { (yyval.argdecl) = new ASTKernelArgDecl("param", (yyvsp[(2) - (2)].ident)); }
    break;

  case 138:
/* Line 1792 of yacc.c  */
#line 834 "parser/AspenParser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 139:
/* Line 1792 of yacc.c  */
#line 837 "parser/AspenParser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); }
    break;

  case 140:
/* Line 1792 of yacc.c  */
#line 838 "parser/AspenParser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); }
    break;

  case 141:
/* Line 1792 of yacc.c  */
#line 839 "parser/AspenParser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].ident); }
    break;

  case 142:
/* Line 1792 of yacc.c  */
#line 840 "parser/AspenParser.y"
    { (yyval.expr) = new ArrayDereference(new Identifier((yyvsp[(1) - (4)].str)), (yyvsp[(3) - (4)].expr)); }
    break;

  case 143:
/* Line 1792 of yacc.c  */
#line 841 "parser/AspenParser.y"
    { (yyval.expr) = new TableDereference(new Identifier((yyvsp[(1) - (7)].str)), (yyvsp[(3) - (7)].expr), (yyvsp[(6) - (7)].expr)); }
    break;

  case 144:
/* Line 1792 of yacc.c  */
#line 842 "parser/AspenParser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 145:
/* Line 1792 of yacc.c  */
#line 843 "parser/AspenParser.y"
    { (yyval.expr) = new BinaryExpr("+", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 146:
/* Line 1792 of yacc.c  */
#line 844 "parser/AspenParser.y"
    { (yyval.expr) = new BinaryExpr("-", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 147:
/* Line 1792 of yacc.c  */
#line 845 "parser/AspenParser.y"
    { (yyval.expr) = new BinaryExpr("*", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 148:
/* Line 1792 of yacc.c  */
#line 846 "parser/AspenParser.y"
    { (yyval.expr) = new BinaryExpr("/", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 149:
/* Line 1792 of yacc.c  */
#line 847 "parser/AspenParser.y"
    { (yyval.expr) = new BinaryExpr("^", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); }
    break;

  case 150:
/* Line 1792 of yacc.c  */
#line 848 "parser/AspenParser.y"
    { (yyval.expr) = new UnaryExpr("-", (yyvsp[(2) - (2)].expr)); }
    break;

  case 151:
/* Line 1792 of yacc.c  */
#line 849 "parser/AspenParser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].call); }
    break;

  case 152:
/* Line 1792 of yacc.c  */
#line 854 "parser/AspenParser.y"
    {
    (yyval.call) = new FunctionCall((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].exprlist));
    (yyvsp[(3) - (4)].exprlist).release();
}
    break;

  case 153:
/* Line 1792 of yacc.c  */
#line 861 "parser/AspenParser.y"
    { (yyval.exprlist).clear(); }
    break;

  case 154:
/* Line 1792 of yacc.c  */
#line 862 "parser/AspenParser.y"
    { (yyval.exprlist).clear(); (yyval.exprlist).push_back((yyvsp[(1) - (1)].expr)); }
    break;

  case 155:
/* Line 1792 of yacc.c  */
#line 863 "parser/AspenParser.y"
    { (yyval.exprlist)=(yyvsp[(1) - (3)].exprlist); (yyval.exprlist).push_back((yyvsp[(3) - (3)].expr)); }
    break;

  case 156:
/* Line 1792 of yacc.c  */
#line 868 "parser/AspenParser.y"
    {
    ///\todo: decide if namedarguments needs ident or TIDENT here.
    (yyval.assignlist).clear();
    (yyval.assignlist).push_back(new ASTAssignStatement(new Identifier((yyvsp[(1) - (3)].str)),(yyvsp[(3) - (3)].expr)));
}
    break;

  case 157:
/* Line 1792 of yacc.c  */
#line 874 "parser/AspenParser.y"
    {
    (yyval.assignlist)=(yyvsp[(1) - (5)].assignlist);
    (yyval.assignlist).push_back(new ASTAssignStatement(new Identifier((yyvsp[(3) - (5)].str)),(yyvsp[(5) - (5)].expr)));
}
    break;


/* Line 1792 of yacc.c  */
#line 3207 "parser/AspenParser.cpp"
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
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
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
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
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

  yyerror_range[1] = yylloc;

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
		      yytoken, &yylval, &yylloc);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

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

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 880 "parser/AspenParser.y"


void yyerror(const char *s)
{
    cerr << "ERROR: " << s << " text=\""<<yytext<<"\" line="<<yylineno<< endl;
    cerr << "(Detailed location:"
        << " file='" << yylloc.filename << "'"
        << ", filepos=" << yylloc.first_filepos << " to " << yylloc.last_filepos
        << ", lines=" << yylloc.first_line << " to " << yylloc.last_line
        << ", columns=" << yylloc.first_column << " to " << yylloc.last_column
        << ")" << endl;
}
