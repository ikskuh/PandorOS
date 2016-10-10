/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 1 "src/grammar.lg"

#include "interpreter.h"
#include "basic/var.h"
#include "basic/lexer.h"
#include "basic/arithmetic.h"
#include "string.h"
#include "standard.h"
#include "alloc.h"
#include "malloc.h"
#include "file.h"
#include "debug.h"
#include <stdbool.h>

#include "hal.h"
#include <stdarg.h>
typedef struct file FILE;

#define fprintf(a, ...) hal_debug(__VA_ARGS__)

static void assert(int x, char const * xstr)
{
	if(x) return;
	hal_debug("Assertion failed: '%s'\n", xstr);
	basic_error(ERR_ASSERTION);
}
#define assert(x) assert(x, #x);

basfunc_f basic_getfunc(int type, char const *name);

typedef struct arg {
	value_t value;
	struct arg *next;
} arg_t;

typedef union {
	value_t val;
	variable_t * var;
	arg_t * arg;
	basfunc_f fun;
} token_t;

static value_t result;

static allocator_t * argalloc;

#line 54 "src/grammar.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 31
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE token_t
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL
#define ParseARG_PDECL
#define ParseARG_FETCH
#define ParseARG_STORE
#define YYNSTATE 50
#define YYNRULE 29
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
#define YY_ACTTAB_COUNT (131)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    51,   18,   19,   16,   17,   15,   14,   13,   52,   12,
 /*    10 */    11,   10,    9,    8,    7,   20,   18,   19,   16,   17,
 /*    20 */    15,   14,   13,   53,   12,   11,   10,    9,    8,    7,
 /*    30 */    22,   22,   34,   37,   42,   36,   18,   19,   16,   17,
 /*    40 */    15,   14,   13,   49,   12,   11,   10,    9,    8,    7,
 /*    50 */    12,   11,   10,    9,    8,    7,   22,   26,   33,    2,
 /*    60 */    80,   21,   81,   16,   17,   15,   14,   13,   35,   12,
 /*    70 */    11,   10,    9,    8,    7,   50,   25,    5,   28,   27,
 /*    80 */    32,   31,   30,    4,   48,   47,   46,   81,   45,   44,
 /*    90 */    81,    1,   38,    6,   43,   41,   40,   39,    3,   15,
 /*   100 */    14,   13,   81,   12,   11,   10,    9,    8,    7,    5,
 /*   110 */    23,   24,   29,   81,   81,    4,   81,   81,   81,   81,
 /*   120 */    81,   81,   81,   81,   38,    6,   81,   41,   40,   39,
 /*   130 */     3,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     0,    1,    2,    3,    4,    5,    6,    7,    0,    9,
 /*    10 */    10,   11,   12,   13,   14,   15,    1,    2,    3,    4,
 /*    20 */     5,    6,    7,    0,    9,   10,   11,   12,   13,   14,
 /*    30 */    27,   27,   29,   29,   19,   19,    1,    2,    3,    4,
 /*    40 */     5,    6,    7,   17,    9,   10,   11,   12,   13,   14,
 /*    50 */     9,   10,   11,   12,   13,   14,   27,   27,   29,   24,
 /*    60 */    26,   27,   30,    3,    4,    5,    6,    7,   28,    9,
 /*    70 */    10,   11,   12,   13,   14,    0,   27,    2,   27,   27,
 /*    80 */    27,   27,   27,    8,   27,   27,   27,   30,   27,   27,
 /*    90 */    30,   16,   17,   18,   27,   20,   21,   22,   23,    5,
 /*   100 */     6,    7,   30,    9,   10,   11,   12,   13,   14,    2,
 /*   110 */    27,   27,   27,   30,   30,    8,   30,   30,   30,   30,
 /*   120 */    30,   30,   30,   30,   17,   18,   30,   20,   21,   22,
 /*   130 */    23,
};
#define YY_SHIFT_USE_DFLT (-1)
#define YY_SHIFT_COUNT (35)
#define YY_SHIFT_MIN   (0)
#define YY_SHIFT_MAX   (107)
static const signed char yy_shift_ofst[] = {
 /*     0 */    75,  107,  107,  107,  107,  107,  107,  107,  107,  107,
 /*    10 */   107,  107,  107,  107,  107,  107,  107,  107,  107,  107,
 /*    20 */    26,    0,   35,   15,   60,   60,   60,   94,   94,   41,
 /*    30 */    41,   41,   41,   23,   16,    8,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (20)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (85)
static const signed char yy_reduce_ofst[] = {
 /*     0 */    34,   29,    4,    3,   85,   84,   83,   67,   62,   61,
 /*    10 */    59,   58,   57,   55,   54,   53,   52,   51,   49,   30,
 /*    20 */    40,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */    79,   76,   76,   76,   79,   79,   79,   79,   79,   79,
 /*    10 */    79,   79,   79,   79,   79,   79,   79,   79,   79,   79,
 /*    20 */    79,   79,   77,   79,   69,   56,   55,   58,   57,   70,
 /*    30 */    61,   60,   59,   79,   79,   79,   75,   78,   74,   73,
 /*    40 */    72,   71,   68,   67,   66,   65,   64,   63,   62,   54,
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "PLUS",          "MINUS",         "DIVIDE",      
  "TIMES",         "AND",           "OR",            "XOR",         
  "NOT",           "LESS",          "LESSEQ",        "GREATER",     
  "GREATEREQ",     "EQ",            "NEQ",           "ASS",         
  "ORDER",         "VAR",           "BRO",           "BRC",         
  "INTEGER",       "BOOL",          "STRING",        "FUN",         
  "COMMA",         "error",         "program",       "expr",        
  "variable",      "arglist",     
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::=",
 /*   1 */ "program ::= expr",
 /*   2 */ "program ::= expr ASS variable",
 /*   3 */ "program ::= ORDER arglist",
 /*   4 */ "variable ::= VAR",
 /*   5 */ "expr ::= expr MINUS expr",
 /*   6 */ "expr ::= expr PLUS expr",
 /*   7 */ "expr ::= expr TIMES expr",
 /*   8 */ "expr ::= expr DIVIDE expr",
 /*   9 */ "expr ::= expr AND expr",
 /*  10 */ "expr ::= expr OR expr",
 /*  11 */ "expr ::= expr XOR expr",
 /*  12 */ "expr ::= expr LESS expr",
 /*  13 */ "expr ::= expr LESSEQ expr",
 /*  14 */ "expr ::= expr GREATER expr",
 /*  15 */ "expr ::= expr GREATEREQ expr",
 /*  16 */ "expr ::= expr EQ expr",
 /*  17 */ "expr ::= expr NEQ expr",
 /*  18 */ "expr ::= BRO expr BRC",
 /*  19 */ "expr ::= MINUS expr",
 /*  20 */ "expr ::= NOT expr",
 /*  21 */ "expr ::= INTEGER",
 /*  22 */ "expr ::= BOOL",
 /*  23 */ "expr ::= STRING",
 /*  24 */ "expr ::= VAR",
 /*  25 */ "expr ::= FUN arglist BRC",
 /*  26 */ "arglist ::=",
 /*  27 */ "arglist ::= expr",
 /*  28 */ "arglist ::= expr COMMA arglist",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_COUNT
   || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 26, 0 },
  { 26, 1 },
  { 26, 3 },
  { 26, 2 },
  { 28, 1 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 3 },
  { 27, 2 },
  { 27, 2 },
  { 27, 1 },
  { 27, 1 },
  { 27, 1 },
  { 27, 1 },
  { 27, 3 },
  { 29, 0 },
  { 29, 1 },
  { 29, 3 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* program ::= */
#line 61 "src/grammar.lg"
{ result = basic_mknull(); }
#line 782 "src/grammar.c"
        break;
      case 1: /* program ::= expr */
#line 62 "src/grammar.lg"
{ result = yymsp[0].minor.yy0.val; }
#line 787 "src/grammar.c"
        break;
      case 2: /* program ::= expr ASS variable */
#line 63 "src/grammar.lg"
{
	var_set(yymsp[0].minor.yy0.var, yymsp[-2].minor.yy0.val);
	result = yymsp[-2].minor.yy0.val;
}
#line 795 "src/grammar.c"
        break;
      case 3: /* program ::= ORDER arglist */
#line 67 "src/grammar.lg"
{
	
	arg_t *a;

	int cnt = 0;
	for(a = yymsp[0].minor.yy0.arg; a != NULL; a = a->next)
	{
		cnt++;
	}

	value_t args[cnt];
	
	int i;
	for(a = yymsp[0].minor.yy0.arg, i = 0; a != NULL; a = a->next, i++)
	{
		args[i] = a->value;
	}

	result = yymsp[-1].minor.yy0.fun(cnt, args);
}
#line 819 "src/grammar.c"
        break;
      case 4: /* variable ::= VAR */
      case 21: /* expr ::= INTEGER */ yytestcase(yyruleno==21);
      case 22: /* expr ::= BOOL */ yytestcase(yyruleno==22);
      case 23: /* expr ::= STRING */ yytestcase(yyruleno==23);
#line 88 "src/grammar.lg"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 827 "src/grammar.c"
        break;
      case 5: /* expr ::= expr MINUS expr */
#line 90 "src/grammar.lg"
{ yygotominor.yy0.val = val_sub(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 832 "src/grammar.c"
        break;
      case 6: /* expr ::= expr PLUS expr */
#line 91 "src/grammar.lg"
{ yygotominor.yy0.val = val_add(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 837 "src/grammar.c"
        break;
      case 7: /* expr ::= expr TIMES expr */
#line 92 "src/grammar.lg"
{ yygotominor.yy0.val = val_mul(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 842 "src/grammar.c"
        break;
      case 8: /* expr ::= expr DIVIDE expr */
#line 93 "src/grammar.lg"
{ yygotominor.yy0.val = val_div(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 847 "src/grammar.c"
        break;
      case 9: /* expr ::= expr AND expr */
#line 95 "src/grammar.lg"
{ yygotominor.yy0.val = val_and(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 852 "src/grammar.c"
        break;
      case 10: /* expr ::= expr OR expr */
#line 96 "src/grammar.lg"
{ yygotominor.yy0.val = val_or(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 857 "src/grammar.c"
        break;
      case 11: /* expr ::= expr XOR expr */
#line 97 "src/grammar.lg"
{ yygotominor.yy0.val = val_xor(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 862 "src/grammar.c"
        break;
      case 12: /* expr ::= expr LESS expr */
#line 99 "src/grammar.lg"
{ yygotominor.yy0.val = val_lt(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 867 "src/grammar.c"
        break;
      case 13: /* expr ::= expr LESSEQ expr */
#line 100 "src/grammar.lg"
{ yygotominor.yy0.val = val_le(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 872 "src/grammar.c"
        break;
      case 14: /* expr ::= expr GREATER expr */
#line 101 "src/grammar.lg"
{ yygotominor.yy0.val = val_lt(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 877 "src/grammar.c"
        break;
      case 15: /* expr ::= expr GREATEREQ expr */
#line 102 "src/grammar.lg"
{ yygotominor.yy0.val = val_le(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 882 "src/grammar.c"
        break;
      case 16: /* expr ::= expr EQ expr */
#line 103 "src/grammar.lg"
{ yygotominor.yy0.val = val_eq(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 887 "src/grammar.c"
        break;
      case 17: /* expr ::= expr NEQ expr */
#line 104 "src/grammar.lg"
{ yygotominor.yy0.val = val_neq(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 892 "src/grammar.c"
        break;
      case 18: /* expr ::= BRO expr BRC */
#line 107 "src/grammar.lg"
{ yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 897 "src/grammar.c"
        break;
      case 19: /* expr ::= MINUS expr */
#line 109 "src/grammar.lg"
{ yygotominor.yy0.val = val_neg(yymsp[0].minor.yy0.val); }
#line 902 "src/grammar.c"
        break;
      case 20: /* expr ::= NOT expr */
#line 110 "src/grammar.lg"
{ yygotominor.yy0.val = val_not(yymsp[0].minor.yy0.val); }
#line 907 "src/grammar.c"
        break;
      case 24: /* expr ::= VAR */
#line 116 "src/grammar.lg"
{ 
	var_get(yymsp[0].minor.yy0.var, &yygotominor.yy0.val);
}
#line 914 "src/grammar.c"
        break;
      case 25: /* expr ::= FUN arglist BRC */
#line 120 "src/grammar.lg"
{
	arg_t *a;

	int cnt = 0;
	for(a = yymsp[-1].minor.yy0.arg; a != NULL; a = a->next)
	{
		cnt++;
	}

	value_t args[cnt];
	
	int i;
	for(a = yymsp[-1].minor.yy0.arg, i = 0; a != NULL; a = a->next, i++)
	{
		args[i] = a->value;
	}

	yygotominor.yy0.val = yymsp[-2].minor.yy0.fun(cnt, args);
}
#line 937 "src/grammar.c"
        break;
      case 26: /* arglist ::= */
#line 140 "src/grammar.lg"
{ yygotominor.yy0.arg = NULL; }
#line 942 "src/grammar.c"
        break;
      case 27: /* arglist ::= expr */
#line 142 "src/grammar.lg"
{ 
	yygotominor.yy0.arg = allocator_alloc(argalloc);
	yygotominor.yy0.arg->value = yymsp[0].minor.yy0.val;
	yygotominor.yy0.arg->next = NULL;
}
#line 951 "src/grammar.c"
        break;
      case 28: /* arglist ::= expr COMMA arglist */
#line 148 "src/grammar.lg"
{
	yygotominor.yy0.arg = allocator_alloc(argalloc);
	yygotominor.yy0.arg->value = yymsp[-2].minor.yy0.val;
	yygotominor.yy0.arg->next = yymsp[0].minor.yy0.arg;
}
#line 960 "src/grammar.c"
        break;
      default:
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 57 "src/grammar.lg"
  
  basic_error(ERR_SYNTAX_ERROR);
#line 1025 "src/grammar.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
#line 154 "src/grammar.lg"


	volatile bool basic_break;
	volatile bool basic_stop_silent = false;

	static token_t nulltoken = { };
	
	
	value_t basic_execute2(uint8_t const * tokens, int length)
	{
		basic_break = false;
		basic_stop_silent = false;
		
		basic_memreset();
		
		// ParseTrace(&nulltoken, "PARSER: ");
		
		// hexdump("Executed Code", tokens, length);
		
		void* pParser = ParseAlloc (malloc);
		
		argalloc = allocator_new(sizeof(arg_t));
		
		int conditionalMode = 0;
		
		result = basic_mknull();
		for(int i = 0; i < length; )
		{
			if(basic_stop_silent) {
				break;
			}
			if(basic_break) {
				basic_error(ERR_BREAK);
			}
			int token_type = (char)tokens[i++];
			if(token_type > 0)
			{
				token_t currtok;
				switch(token_type)
				{
					case TOK_INTEGER:
					{
						if(conditionalMode != 3) {
							int ival = *((int*)&tokens[i]);
							currtok.val = basic_mknum(ival);
						}
						i += sizeof(int);
						break;
					}
					case TOK_BOOL:
					{
						if(conditionalMode != 3) {
							currtok.val = basic_mknum(*((bool*)&tokens[i]));
						}
						i += sizeof(bool);
						break;
					}
					case TOK_STRING:
					{
						int len = *((int*)&tokens[i]);
						i += sizeof(int);
						
						if(conditionalMode != 3) {
							currtok.val = basic_mkstr((char const *)&tokens[i]);
						}
						i += len;
						break;
					}
					case TOK_VAR:
					{
						int len = *((int*)&tokens[i]);
						i += sizeof(int);
						
						if(conditionalMode != 3) {
							currtok.var = var_byname((char const *)&tokens[i]);
							if(currtok.var == NULL) {
								basic_error(ERR_INVALID_VAR);
							}
						}
						i += len;
						break;
					}
					case TOK_FUN:
					{
						int len = *((int*)&tokens[i]);
						i += sizeof(int);
						
						if(conditionalMode != 3) {
							currtok.fun = basic_getfunc(BASIC_FUNCTION, (char const *)&tokens[i]);
							if(currtok.fun == NULL) {
								basic_error(ERR_FUNC_NOT_FOUND);
							}
						}
						i += len;
						
						break;
					}
					case TOK_ORDER:
					{
						int len = *((int*)&tokens[i]);
						i += sizeof(int);
						if(conditionalMode != 3) {
							currtok.fun = basic_getfunc(BASIC_ORDER, (char const *)&tokens[i]);
							if(currtok.fun == NULL) {
								basic_error(ERR_FUNC_NOT_FOUND);
							}
						}
						i += len;
						
						break;
					}
				}
				if(conditionalMode != 3) {
					Parse(pParser, token_type, currtok);
				}
			}
			else if(token_type == TOKEN_INVALID)
			{
				basic_error(ERR_INVALID_TOKEN);
			}
			else if(token_type == TOKEN_GOTO)
			{
				if(conditionalMode != 3)
				{
					// Simple goto.
					i = *((int*)&tokens[i]);
					conditionalMode = 0;
				}
			}
			else if(token_type == TOKEN_IF)
			{
				conditionalMode = 1;
			}
			else if(token_type == TOKEN_EOF || token_type == TOKEN_EOL)
			{
				if(conditionalMode != 3)
				{ // Only evaluate when we have taken the condition
					Parse(pParser, 0, nulltoken);
					
					if(conditionalMode == 0 || conditionalMode == 2)
					{
						if(basic_isnull(result) == false) {
							var_setans(result);
							
							// This seems odd, but our temporary work buffer will be
							// released after this function returns, so we
							// copy the persistent 'Ans' variable content to it.
							var_getans(&result);
						}
						
						// Normal execution, resets conditional mode.
						conditionalMode = 0;
					}
					else if(conditionalMode == 1)
					{
						// Execute when non-zero numeric value.
						if(basic_getnum(result) != 0) {
							conditionalMode = 2;
						} else {
							conditionalMode = 3;
						}
					}
					else
					{
						debug("Huh? conditionalMode=%d\n", conditionalMode);
					}
				}
				else
				{
					// Reset conditional mode
					conditionalMode = 0;
				}
			}
			else if(token_type == TOKEN_EXIT)
			{
				break;
			}
			else if(token_type == TOKEN_STOP)
			{
				basic_stop_silent = true;
			}
			else if(token_type == TOKEN_WHITESPACE || token_type == TOKEN_EMPTY) 
			{
				// Silently ignore
			}
			else
			{
				debug("Unhandled token: %d\n", token_type);
			}
			if(token_type == TOKEN_EOF)
				break;
		}
		ParseFree(pParser, free );
		
		allocator_delete(argalloc);
		
		return result;
	}
#line 1415 "src/grammar.c"
