// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
/**
 * @file    parser/AspenParser.y
 * @author  Jeremy Meredith
 *          jsmeredith@gmail.com
 * Bison grammar file for Aspen modeling language
 */

%{
    #include "common/AST.h"
    #include "parser/AspenParseNode.h"
    ASTAppModel *globalapp = NULL;
    ASTMachModel *globalmach = NULL;
    ASTMapModel *globalmap = NULL;

    extern int yylex();
    extern int yylineno;
    extern char *yytext;
    void yyerror(const char *);
%}

%locations

%code requires {
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

}
/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <token> TKW_PARAM
%token <token> TKW_ENUM
%token <token> TKW_IN
%token <token> TKW_WITH
%token <token> TKW_OF
%token <token> TKW_SIZE

%token <token> TKW_MODEL
%token <token> TKW_KERNEL
%token <token> TKW_DATA
%token <token> TKW_SAMPLE
%token <token> TKW_IMPORT
%token <token> TKW_AS
%token <token> TKW_TO
%token <token> TKW_FROM
%token <token> TKW_CALL
%token <token> TKW_ITERATE
%token <token> TKW_MAP
%token <token> TKW_PAR
%token <token> TKW_SEQ
%token <token> TKW_EXECUTE

%token <token> TKW_IF
%token <token> TKW_ELSE
%token <token> TKW_PROBABILITY
%token <token> TKW_SWITCH

%token <token> TKW_RESOURCE
%token <token> TKW_CONFLICT
%token <token> TKW_POWER
%token <token> TKW_STATIC
%token <token> TKW_DYNAMIC
%token <token> TKW_PROPERTY


%token <str> TIDENT TSTRING TINT TREAL TKW_10POWER
%token <token> TDOTDOT TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET
%token <token> TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV TEXP
%token <token> TCOMPEQ TCOMPNE TCOMPLT TCOMPGT TCOMPLE TCOMPGE
%token <token> TAND TOR

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */

%type <map> map
%type <localmap> mapdata mapkernel
%type <mapstmt> mapstmt
%type <mapstmtlist> mapstmts

%type <app> app
%type <mach> mach
%type <expr> expr value quantity constant comparison
%type <exprlist> arguments
%type <stmt> globalstmt localstmt paramstmt datastmt importstmt
%type <stmtlist> globalstatements localstatements
%type <execstmt> execstmt
%type <execstmtlist> execstatements
%type <assignlist> namedarguments
%type <valuelist> valuelist arraylist
%type <stringlist> identlist
%type <kernel> kernel
%type <kernellist> kernels
%type <controlstmt> samplestmt kernelstmt controlstmt kernelcall ifthencontrol probabilitycontrol switchcontrol
%type <controlstmtlist> kernelstatements
%type <argdecl> argdecl
%type <argdecllist> argdecllist
%type <trait> trait
%type <traitlist> optionaltraitlist traitlist
%type <caselist> caseitems
%type <ident> ident optionalident
%type <str> tofrom optionalstring
%type <call> functioncall
%type <astnode> componentstmt
%type <astnodelist> componentstatements
%type <subcomponent> subcomponent
%type <component> component
%type <power> power
%type <property> property
%type <resource> resource
%type <traitdeflist> traitdefinitions
%type <conflict> conflict
%type <machcontent> machcontent

/* Operator associativity and precedence for mathematical operators */
%left TAND TOR
%left TCOMPEQ TCOMPNE TCOMPLT TCOMPGT TCOMPLE TCOMPGE
%left TPLUS TMINUS
%left TMUL TDIV
%left NEG
%right TEXP

%start begin

%%

begin
: mach
{
    globalmach = $1;
}
| app
{
    globalapp = $1;
}
| map
{
    globalmap = $1;
}
;

mach
: machcontent
{
    $$ = new ASTMachModel($1.first, $1.second);
    $1.first.release();
    $1.second.release();
}
;

machcontent
: globalstmt
{
    // allocate/clear both vectors, because we don't know
    // if the next one might be a component
    $$.first.clear();
    $$.second.clear();
    $$.first.push_back($1);
}
| component
{
    // allocate/clear both vectors, because we don't know
    // if the next one might be a globalstmt
    $$.first.clear();
    $$.second.clear();
    $$.second.push_back($1);
}
| machcontent globalstmt
{
    $$ = $1;
    $$.first.push_back($2);
}
| machcontent component
{
    $$ = $1;
    $$.second.push_back($2);
}
;

component
: TIDENT TIDENT TLBRACE componentstatements TRBRACE
{
    $$ = new ASTMachComponent($1, $2, $4);
    $4.release();
}
;

componentstatements
: componentstmt { $$.clear(); $$.push_back($1); }
| componentstatements componentstmt { $$=$1; $$.push_back($2); }
;

componentstmt
: property { $$ = $1; }
| resource { $$ = $1; }
| power { $$ = $1; }
| conflict { $$ = $1; }
| subcomponent { $$ = $1; }
;

subcomponent
: TIDENT TIDENT { $$ = new ASTSubComponent($1, $2, NULL); }
| TIDENT quantity TIDENT { $$ = new ASTSubComponent($1, $3, $2); }
;


property
: TKW_PROPERTY TIDENT quantity { $$ = new ASTMachProperty($2, $3); }
| TKW_PROPERTY TIDENT { $$ = new ASTMachProperty($2, NULL); }
;

conflict
: TKW_CONFLICT TIDENT TCOMMA TIDENT
{
    $$ = new ASTResourceConflict($2, $4);
}
;

resource
: TKW_RESOURCE TIDENT TLPAREN TIDENT TRPAREN quantity TKW_WITH traitdefinitions
{
    $$ = new ASTMachResource($2, $4, $6, $8);
    $8.release();
}
| TKW_RESOURCE TIDENT  TLPAREN TIDENT TRPAREN quantity
{
    ParseVector<ASTTraitDefinition*> empty;
    empty.clear();
    $$ = new ASTMachResource($2, $4, $6, empty);
    empty.release();
}
;

traitdefinitions
: TIDENT quantity
{
    $$.clear();
    $$.push_back(new ASTTraitDefinition($1, $2));
}
| TIDENT TLPAREN TIDENT TRPAREN quantity
{
    $$.clear();
    $$.push_back(new ASTTraitDefinition($1, $3, $5));
}
| traitdefinitions TCOMMA TIDENT quantity
{
    $$ = $1;
    $$.push_back(new ASTTraitDefinition($3, $4));
}
| traitdefinitions TCOMMA TIDENT TLPAREN TIDENT TRPAREN quantity
{
    $$ = $1;
    $$.push_back(new ASTTraitDefinition($3, $5, $7));
}
;

power
: TKW_STATIC TKW_POWER quantity { $$ = new ASTMachPower($3, NULL); }
| TKW_DYNAMIC TKW_POWER quantity { $$ = new ASTMachPower(NULL, $3); }
;

 /* ---------------------- */

map
: TKW_MODEL TIDENT TLBRACE globalstatements mapdata mapkernel TRBRACE
{
  $$ = new ASTMapModel($2, $4, $5, $6);
  $4.release();
  $5.release();
  $6.release();
}
;

mapkernel
: TKW_KERNEL TLBRACE mapstmts TRBRACE
{
  $$ = new ASTMap(0, $3);
  $3.release();
}

mapdata
: TKW_DATA TLBRACE mapstmts   TRBRACE
{
  $$ = new ASTMap(1, $3);
  $3.release();
}

mapstmts
: mapstmt { $$.clear(); $$.push_back($1); }
| mapstmts mapstmt { $$ = $1; $$.push_back($2); }
;


mapstmt
: TKW_MAP ident TKW_TO ident
{
  $$ = new ASTMapStatement($2,$4);
}




 /* ---------------------- */

///\todo: allow models with no statements and no kernels, or
/// possibly intermixed statements and kernels
app
: TKW_MODEL TIDENT TLBRACE globalstatements kernels TRBRACE
{
    $$ = new ASTAppModel($2, $4, $5);
    $4.release();
    $5.release();
}
;

kernels
: kernel { $$.clear(); $$.push_back($1); }
| kernels kernel { $$ = $1; $$.push_back($2); }
;

kernel
: TKW_KERNEL ident TLBRACE kernelstatements TRBRACE
{
    ParseVector<ASTKernelArgDecl*> empty1;
    empty1.clear();
    ParseVector<ASTStatement*> empty2;
    empty2.clear();
    $$ = new ASTKernel($2, $4, empty1, empty2);
    $4.release();
    empty1.release();
    empty2.release();
}
| TKW_KERNEL ident TLBRACE localstatements kernelstatements TRBRACE
{
    ParseVector<ASTKernelArgDecl*> empty;
    empty.clear();
    $$ = new ASTKernel($2, $5, empty, $4);
    $5.release();
    $4.release();
    empty.release();
}
| TKW_KERNEL ident TLPAREN argdecllist TRPAREN TLBRACE kernelstatements TRBRACE
{
    ParseVector<ASTStatement*> empty;
    empty.clear();
    $$ = new ASTKernel($2, $7, $4, empty);
    $4.release();
    $7.release();
}
| TKW_KERNEL ident TLPAREN argdecllist TRPAREN TLBRACE localstatements kernelstatements TRBRACE
{
    $$ = new ASTKernel($2, $8, $4, $7);
    $4.release();
    $8.release();
    $7.release();
}
;

ident : TIDENT                        { $$ = new Identifier($1); }
      | TIDENT TDOT ident             { $$ = $3; $$->Prefix($1); }
      ;

value : TINT                             { $$ = new Integer(atol($1.c_str())); }
      | TREAL                            { $$ = new Real(atof($1.c_str())); }
      ;

optionalident
: /*blank*/ { $$ = NULL; }
| ident     { $$ = $1; }

constant
: TKW_10POWER
{
    double v = 1;
    if ($1 == "nano")  v = 1e-9;
    if ($1 == "micro") v = 1e-6;
    if ($1 == "milli") v = 1e-3;
    if ($1 == "kilo")  v = 1e3;
    if ($1 == "mega")  v = 1e6;
    if ($1 == "giga")  v = 1e9;
    if ($1 == "tera")  v = 1e12;
    if ($1 == "peta")  v = 1e15;
    if ($1 == "exa")   v = 1e18;
    $$ = new Real(v);
}
;

//
// Lists of statements of various types
//

globalstatements
: globalstmt { $$.clear(); $$.push_back($1); }
| globalstatements globalstmt { $$=$1; $$.push_back($2); }
;

localstatements
: localstmt { $$.clear(); $$.push_back($1); }
| localstatements localstmt { $$=$1; $$.push_back($2); }
;

kernelstatements
: kernelstmt { $$.clear(); $$.push_back($1); }
| kernelstatements kernelstmt { $$=$1; $$.push_back($2); }
;

execstatements
: execstmt { $$.clear(); $$.push_back($1); }
| execstatements execstmt { $$=$1; $$.push_back($2); }
;

//
// Define each section of statements
//

globalstmt
: importstmt { $$ = $1; }
| paramstmt { $$ = $1; }
| datastmt { $$ = $1; }
;

localstmt
: importstmt { $$ = $1; }
| paramstmt { $$ = $1; }
| datastmt { $$ = $1; }
;

kernelstmt
: TKW_EXECUTE optionalstring TLBRACE execstatements TRBRACE
{
    $$ = new ASTExecutionBlock($2, NULL, $4);
    $4.release();
}
| TKW_EXECUTE quantity optionalstring TLBRACE execstatements TRBRACE
{
    $$ = new ASTExecutionBlock($3, $2, $5);
    $5.release();
}
| controlstmt { $$ =  $1; }
///\todo: we want local variables (at least samples) inside kernels,
/// though the question is if there's a new ControlStatement subclass
/// for them, or if Kernels contain (just) statements, or if
/// kernels contain both vars and statements.
///| paramstmt { $$ = $1; }
;

execstmt
: TIDENT quantity tofrom optionaltraitlist
{
    $$ = new ASTRequiresStatement($1, $2, NULL, $3, $4);
    $4.release();
}
| TIDENT quantity TKW_OF TKW_SIZE quantity tofrom optionaltraitlist
{
    $$ = new ASTRequiresStatement($1, $2, $5, $6, $7);
    $7.release();
}
;

//
// Define each statement type
//
paramstmt
: TKW_PARAM ident TEQUAL expr
{
    $$ = new ASTAssignStatement($2,$4);
}
| TKW_PARAM ident TEQUAL expr TKW_IN expr TDOTDOT expr
{
    $$ = new ASTAssignRangeStatement($2, $4, $6, $8);
}
| TKW_PARAM ident TEQUAL expr TKW_IN TIDENT
{
    $$ = new ASTAssignEnumStatement($2, $4, $6);
}
| TKW_PARAM ident TLBRACKET TRBRACKET TEQUAL TLBRACE valuelist TRBRACE
{
    $$ = new ASTAssignStatement($2, new Array($7));
    $7.release();
}
| TKW_PARAM ident TLBRACKET TRBRACKET TEQUAL TIDENT TLPAREN arguments TRPAREN
{
    $$ = new ASTAssignStatement($2, new FunctionCall($6,$8));
    $8.release();
}
| TKW_PARAM ident TLBRACKET TRBRACKET TLBRACKET TRBRACKET TEQUAL TLBRACE arraylist TRBRACE
{
    $$ = new ASTAssignStatement($2, new Table($9));
    $9.release();
}
//| TKW_PARAM TIDENT TLBRACKET TINT TRBRACKET TEQUAL expr
//{
//    $$ = new ASTAssignStatement($2, new ASTArrayFillExpression($5));
//    $5.release();
//}
| TKW_ENUM TIDENT TLBRACE identlist TRBRACE
{
    $$ = new ASTEnumDeclaration($2, $4);
}
; 

arraylist
: TLBRACE valuelist TRBRACE
{
    $$.clear();
    $$.append($2);
    $2.release();
}
| arraylist TCOMMA TLBRACE valuelist TRBRACE
{
    $$ = $1;

    // add two sentinels so we know where to break
    $$.push_back(1e37);
    $$.push_back(-1e37);

    $$.append($4);
    $4.release();
}
;

valuelist
: TINT
{
    $$.clear();
    $$.push_back(atol($1.c_str()));
}
| TREAL
{
    $$.clear();
    $$.push_back(atof($1.c_str()));
}
| valuelist TCOMMA TINT
{
    $$ = $1;
    $$.push_back(atol($3.c_str()));
}
| valuelist TCOMMA TREAL
{
    $$ = $1;
    $$.push_back(atof($3.c_str()));
}
;

identlist
: TIDENT
{
    $$.clear();
    $$.push_back($1);
}
| identlist TCOMMA TIDENT
{
    $$ = $1;
    $$.push_back($3);
}
;

samplestmt
: TKW_SAMPLE ident TKW_FROM functioncall
{
    $$ = new ASTSampleStatement($2,$4);
}
; 

datastmt
: TKW_DATA TIDENT TKW_AS functioncall
{
    $$ = new ASTDataStatement($2,NULL,$4);
}
| TKW_DATA TIDENT quantity
{
    $$ = new ASTDataStatement($2,$3,NULL);
}
;

importstmt
: TKW_IMPORT TIDENT TKW_FROM TSTRING
{
    $$ = new ASTImportStatement($2,$4);
}
| TKW_IMPORT TIDENT TKW_FROM TSTRING TKW_WITH namedarguments
{
    $$ = new ASTImportStatement($2,$4,$6);
    $6.release();
}
;

kernelcall
: TKW_CALL ident
{
    $$ = new ASTControlKernelCallStatement($2);
}
| TKW_CALL ident TLPAREN arguments TRPAREN
{
    $$ = new ASTControlKernelCallStatement($2, $4);
    $4.release();
}
;

comparison
: comparison TAND comparison { $$ = new Comparison("and",$1,$3); }
| comparison TOR comparison  { $$ = new Comparison("or",$1,$3); }
| expr TCOMPEQ expr  { $$ = new Comparison("==",$1,$3); }
| expr TCOMPNE expr  { $$ = new Comparison("!=",$1,$3); }
| expr TCOMPLT expr  { $$ = new Comparison("<", $1,$3); }
| expr TCOMPGT expr  { $$ = new Comparison(">", $1,$3); }
| expr TCOMPLE expr  { $$ = new Comparison("<=",$1,$3); }
| expr TCOMPGE expr  { $$ = new Comparison(">=",$1,$3); }
;

ifthencontrol
: TKW_IF TLPAREN comparison TRPAREN TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlIfThenStatement($3, new ASTControlSequentialStatement("", $6), NULL);
    $6.release();
}
| TKW_IF TLPAREN comparison TRPAREN TLBRACE kernelstatements TRBRACE TKW_ELSE TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlIfThenStatement($3, new ASTControlSequentialStatement("", $6), new ASTControlSequentialStatement("", $10));
    $6.release();
    $10.release();
}
| TKW_IF TLPAREN comparison TRPAREN TLBRACE kernelstatements TRBRACE TKW_ELSE ifthencontrol
{
    $$ = new ASTControlIfThenStatement($3, new ASTControlSequentialStatement("", $6), $9);
    $6.release();
}
;

caseitems
: quantity TLBRACE kernelstatements TRBRACE
{
    $$.clear();
    $$.push_back(new ASTCaseItem($1, new ASTControlSequentialStatement("", $3)));
    $3.release();
}
| caseitems quantity TLBRACE kernelstatements TRBRACE
{
    $$ = $1;
    $$.push_back(new ASTCaseItem($2, new ASTControlSequentialStatement("", $4)));
    $4.release();
}
;

probabilitycontrol
: TKW_PROBABILITY TLBRACE caseitems TRBRACE
{
    $$ = new ASTControlProbabilityStatement($3, NULL);
    $3.release();
}
| TKW_PROBABILITY TLBRACE caseitems TKW_ELSE TLBRACE kernelstatements TRBRACE TRBRACE
{
    $$ = new ASTControlProbabilityStatement($3, new ASTControlSequentialStatement("", $6));
    $3.release();
}
;

switchcontrol
: TKW_SWITCH TLPAREN TIDENT TRPAREN TLBRACE caseitems TRBRACE
{
    $$ = new ASTControlSwitchStatement(new Identifier($3), $6, NULL);
    $6.release();
}
| TKW_SWITCH TLPAREN TIDENT TRPAREN TLBRACE caseitems TKW_ELSE TLBRACE kernelstatements TRBRACE TRBRACE
{
    $$ = new ASTControlSwitchStatement(new Identifier($3), $6, new ASTControlSequentialStatement("", $9));
    $6.release();
}
;

optionalstring
: /*blank*/ { $$ = ""; }
| TSTRING { $$ = $1; }
;

controlstmt
: kernelcall
{
    $$ = $1;
}
| samplestmt
{
    $$ =  $1;
}
| TKW_PAR optionalstring TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlParallelStatement($2, $4);
    $4.release();
}
| TKW_SEQ optionalstring TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlSequentialStatement($2, $4);
    $4.release();
}
| TKW_ITERATE optionalident quantity optionalstring TLBRACE kernelstatements TRBRACE
{
    if ($6.size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ($6.size() == 1)
        $$ = new ASTControlIterateStatement($4, $2, $3, $6[0]);
    else // size > 1
        $$ = new ASTControlIterateStatement($4, $2, $3, new ASTControlSequentialStatement("", $6));
    $6.release();
}
| TKW_MAP optionalident quantity optionalstring TLBRACE kernelstatements TRBRACE
{
    if ($6.size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ($6.size() == 1)
        $$ = new ASTControlMapStatement($4, $2, $3, $6[0]);
    else // size > 1
        $$ = new ASTControlMapStatement($4, $2, $3, new ASTControlSequentialStatement("", $6));
    $6.release();
}
| ifthencontrol
{
    $$ = $1;
}
| probabilitycontrol
{
    $$ = $1;
}
| switchcontrol
{
    $$ = $1;
}
;

tofrom
: /*blank*/ { $$ = ""; }
| TKW_TO TIDENT { $$ = $2; }
| TKW_FROM TIDENT { $$ = $2; }
;

optionaltraitlist
: /*blank*/ { $$.clear(); }
| TKW_AS traitlist { $$ = $2; }
;

traitlist
: trait { $$.clear(); $$.push_back($1); }
| traitlist TCOMMA trait { $$=$1; $$.push_back($3); }
;

trait
: TIDENT { $$ = new ASTTrait($1, NULL); }
| TIDENT TLPAREN expr TRPAREN { $$ = new ASTTrait($1, $3); }
;

argdecllist
: argdecl { $$.clear(); $$.push_back($1); }
| argdecllist TCOMMA argdecl { $$=$1; $$.push_back($3); }
;

argdecl
: TKW_DATA ident { $$ = new ASTKernelArgDecl("data", $2); }
| TKW_PARAM ident { $$ = new ASTKernelArgDecl("param", $2); }
;

quantity
: TLBRACKET expr TRBRACKET { $$ = $2; }

expr
: value { $$ = $1; }
| constant { $$ = $1; }
| ident { $$ = $1; }
| TIDENT TLBRACKET expr TRBRACKET { $$ = new ArrayDereference(new Identifier($1), $3); }
| TIDENT TLBRACKET expr TRBRACKET TLBRACKET expr TRBRACKET { $$ = new TableDereference(new Identifier($1), $3, $6); }
| TLPAREN expr TRPAREN { $$ = $2; }
| expr TPLUS expr { $$ = new BinaryExpr("+", $1, $3); }
| expr TMINUS expr { $$ = new BinaryExpr("-", $1, $3); }
| expr TMUL expr { $$ = new BinaryExpr("*", $1, $3); }
| expr TDIV expr { $$ = new BinaryExpr("/", $1, $3); }
| expr TEXP expr { $$ = new BinaryExpr("^", $1, $3); }
| TMINUS expr %prec NEG { $$ = new UnaryExpr("-", $2); }
| functioncall { $$ = $1; }
;

functioncall
: TIDENT TLPAREN arguments TRPAREN
{
    $$ = new FunctionCall($1, $3);
    $3.release();
}
;

arguments
: /*blank*/ { $$.clear(); }
| expr { $$.clear(); $$.push_back($1); }
| arguments TCOMMA expr { $$=$1; $$.push_back($3); }
;

namedarguments
: TIDENT TEQUAL expr
{
    ///\todo: decide if namedarguments needs ident or TIDENT here.
    $$.clear();
    $$.push_back(new ASTAssignStatement(new Identifier($1),$3));
}
| namedarguments TCOMMA TIDENT TEQUAL expr
{
    $$=$1;
    $$.push_back(new ASTAssignStatement(new Identifier($3),$5));
}
;

%%

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
