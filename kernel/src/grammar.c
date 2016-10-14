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

basfunc_f basic_getfunc(char const *name);

typedef struct arg {
	value_t value;
	struct arg *next;
} arg_t;

typedef union {
	value_t val;
	arg_t * arg;
	char * id;
} token_t;

static value_t result;

#line 51 "src/grammar.c"
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
#define YYNOCODE 28
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
#define YYNSTATE 47
#define YYNRULE 27
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
#define YY_ACTTAB_COUNT (115)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    48,   17,   18,   15,   16,   14,   13,   12,   33,   11,
 /*    10 */    10,    9,    8,    7,    6,   34,   17,   18,   15,   16,
 /*    20 */    14,   13,   12,   49,   11,   10,    9,    8,    7,    6,
 /*    30 */    20,   31,   35,   40,    2,   17,   18,   15,   16,   14,
 /*    40 */    13,   12,   76,   11,   10,    9,    8,    7,    6,   75,
 /*    50 */    19,   15,   16,   14,   13,   12,    1,   11,   10,    9,
 /*    60 */     8,    7,    6,   14,   13,   12,   24,   11,   10,    9,
 /*    70 */     8,    7,    6,   47,   23,    4,   26,   20,   36,   25,
 /*    80 */    30,    3,   29,   11,   10,    9,    8,    7,    6,   32,
 /*    90 */     5,   28,   39,   38,   37,    4,   46,   45,   76,   76,
 /*   100 */    44,    3,   43,   42,   41,   21,   22,   27,   76,   32,
 /*   110 */     5,   76,   39,   38,   37,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     0,    1,    2,    3,    4,    5,    6,    7,   16,    9,
 /*    10 */    10,   11,   12,   13,   14,   15,    1,    2,    3,    4,
 /*    20 */     5,    6,    7,    0,    9,   10,   11,   12,   13,   14,
 /*    30 */    25,   26,   18,   18,   17,    1,    2,    3,    4,    5,
 /*    40 */     6,    7,   27,    9,   10,   11,   12,   13,   14,   24,
 /*    50 */    25,    3,    4,    5,    6,    7,   22,    9,   10,   11,
 /*    60 */    12,   13,   14,    5,    6,    7,   25,    9,   10,   11,
 /*    70 */    12,   13,   14,    0,   25,    2,   25,   25,   26,   25,
 /*    80 */    25,    8,   25,    9,   10,   11,   12,   13,   14,   16,
 /*    90 */    17,   25,   19,   20,   21,    2,   25,   25,   27,   27,
 /*   100 */    25,    8,   25,   25,   25,   25,   25,   25,   27,   16,
 /*   110 */    17,   27,   19,   20,   21,
};
#define YY_SHIFT_USE_DFLT (-9)
#define YY_SHIFT_COUNT (34)
#define YY_SHIFT_MIN   (-8)
#define YY_SHIFT_MAX   (93)
static const signed char yy_shift_ofst[] = {
 /*     0 */    73,   93,   93,   93,   93,   93,   93,   93,   93,   93,
 /*    10 */    93,   93,   93,   93,   93,   93,   93,   93,   93,    0,
 /*    20 */    34,   15,   48,   48,   48,   58,   58,   74,   74,   74,
 /*    30 */    74,   14,   17,   23,   -8,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (18)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (82)
static const signed char yy_reduce_ofst[] = {
 /*     0 */    25,   52,    5,   82,   81,   80,   79,   78,   77,   75,
 /*    10 */    72,   71,   66,   57,   55,   54,   51,   49,   41,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */    74,   71,   71,   74,   74,   74,   74,   74,   74,   74,
 /*    10 */    74,   74,   74,   74,   74,   74,   74,   74,   74,   74,
 /*    20 */    72,   74,   64,   51,   50,   53,   52,   65,   56,   55,
 /*    30 */    54,   74,   69,   74,   74,   70,   73,   68,   67,   66,
 /*    40 */    63,   62,   61,   60,   59,   58,   57,
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
  "ID",            "BRO",           "BRC",           "INTEGER",     
  "BOOL",          "STRING",        "COMMA",         "error",       
  "program",       "expr",          "arglist",     
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::=",
 /*   1 */ "program ::= expr",
 /*   2 */ "program ::= expr ASS ID",
 /*   3 */ "expr ::= expr MINUS expr",
 /*   4 */ "expr ::= expr PLUS expr",
 /*   5 */ "expr ::= expr TIMES expr",
 /*   6 */ "expr ::= expr DIVIDE expr",
 /*   7 */ "expr ::= expr AND expr",
 /*   8 */ "expr ::= expr OR expr",
 /*   9 */ "expr ::= expr XOR expr",
 /*  10 */ "expr ::= expr LESS expr",
 /*  11 */ "expr ::= expr LESSEQ expr",
 /*  12 */ "expr ::= expr GREATER expr",
 /*  13 */ "expr ::= expr GREATEREQ expr",
 /*  14 */ "expr ::= expr EQ expr",
 /*  15 */ "expr ::= expr NEQ expr",
 /*  16 */ "expr ::= BRO expr BRC",
 /*  17 */ "expr ::= MINUS expr",
 /*  18 */ "expr ::= NOT expr",
 /*  19 */ "expr ::= INTEGER",
 /*  20 */ "expr ::= BOOL",
 /*  21 */ "expr ::= STRING",
 /*  22 */ "expr ::= ID",
 /*  23 */ "expr ::= ID BRO arglist BRC",
 /*  24 */ "arglist ::=",
 /*  25 */ "arglist ::= expr",
 /*  26 */ "arglist ::= expr COMMA arglist",
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
  { 24, 0 },
  { 24, 1 },
  { 24, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 3 },
  { 25, 2 },
  { 25, 2 },
  { 25, 1 },
  { 25, 1 },
  { 25, 1 },
  { 25, 1 },
  { 25, 4 },
  { 26, 0 },
  { 26, 1 },
  { 26, 3 },
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
#line 58 "src/grammar.lg"
{ result = basic_mknull(); }
#line 769 "src/grammar.c"
        break;
      case 1: /* program ::= expr */
#line 59 "src/grammar.lg"
{ result = yymsp[0].minor.yy0.val; }
#line 774 "src/grammar.c"
        break;
      case 2: /* program ::= expr ASS ID */
#line 60 "src/grammar.lg"
{
	variable_t * var = var_byname(yymsp[0].minor.yy0.id);
	if(var == NULL) basic_error(ERR_INVALID_VAR);
	var_set(var, yymsp[-2].minor.yy0.val);
	result = yymsp[-2].minor.yy0.val;
}
#line 784 "src/grammar.c"
        break;
      case 3: /* expr ::= expr MINUS expr */
#line 67 "src/grammar.lg"
{ yygotominor.yy0.val = val_sub(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 789 "src/grammar.c"
        break;
      case 4: /* expr ::= expr PLUS expr */
#line 68 "src/grammar.lg"
{ yygotominor.yy0.val = val_add(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 794 "src/grammar.c"
        break;
      case 5: /* expr ::= expr TIMES expr */
#line 69 "src/grammar.lg"
{ yygotominor.yy0.val = val_mul(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 799 "src/grammar.c"
        break;
      case 6: /* expr ::= expr DIVIDE expr */
#line 70 "src/grammar.lg"
{ yygotominor.yy0.val = val_div(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 804 "src/grammar.c"
        break;
      case 7: /* expr ::= expr AND expr */
#line 72 "src/grammar.lg"
{ yygotominor.yy0.val = val_and(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 809 "src/grammar.c"
        break;
      case 8: /* expr ::= expr OR expr */
#line 73 "src/grammar.lg"
{ yygotominor.yy0.val = val_or(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 814 "src/grammar.c"
        break;
      case 9: /* expr ::= expr XOR expr */
#line 74 "src/grammar.lg"
{ yygotominor.yy0.val = val_xor(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 819 "src/grammar.c"
        break;
      case 10: /* expr ::= expr LESS expr */
#line 76 "src/grammar.lg"
{ yygotominor.yy0.val = val_lt(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 824 "src/grammar.c"
        break;
      case 11: /* expr ::= expr LESSEQ expr */
#line 77 "src/grammar.lg"
{ yygotominor.yy0.val = val_le(yymsp[-2].minor.yy0.val, yymsp[0].minor.yy0.val); }
#line 829 "src/grammar.c"
        break;
      case 12: /* expr ::= expr GREATER expr */
#line 78 "src/grammar.lg"
{ yygotominor.yy0.val = val_lt(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 834 "src/grammar.c"
        break;
      case 13: /* expr ::= expr GREATEREQ expr */
#line 79 "src/grammar.lg"
{ yygotominor.yy0.val = val_le(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 839 "src/grammar.c"
        break;
      case 14: /* expr ::= expr EQ expr */
#line 80 "src/grammar.lg"
{ yygotominor.yy0.val = val_eq(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 844 "src/grammar.c"
        break;
      case 15: /* expr ::= expr NEQ expr */
#line 81 "src/grammar.lg"
{ yygotominor.yy0.val = val_neq(yymsp[0].minor.yy0.val, yymsp[-2].minor.yy0.val); }
#line 849 "src/grammar.c"
        break;
      case 16: /* expr ::= BRO expr BRC */
#line 84 "src/grammar.lg"
{ yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 854 "src/grammar.c"
        break;
      case 17: /* expr ::= MINUS expr */
#line 86 "src/grammar.lg"
{ yygotominor.yy0.val = val_neg(yymsp[0].minor.yy0.val); }
#line 859 "src/grammar.c"
        break;
      case 18: /* expr ::= NOT expr */
#line 87 "src/grammar.lg"
{ yygotominor.yy0.val = val_not(yymsp[0].minor.yy0.val); }
#line 864 "src/grammar.c"
        break;
      case 19: /* expr ::= INTEGER */
      case 20: /* expr ::= BOOL */ yytestcase(yyruleno==20);
      case 21: /* expr ::= STRING */ yytestcase(yyruleno==21);
#line 89 "src/grammar.lg"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 871 "src/grammar.c"
        break;
      case 22: /* expr ::= ID */
#line 93 "src/grammar.lg"
{ 
	variable_t * var = var_byname(yymsp[0].minor.yy0.id);
	if(var == NULL) basic_error(ERR_INVALID_VAR);
	var_get(var, &yygotominor.yy0.val);
}
#line 880 "src/grammar.c"
        break;
      case 23: /* expr ::= ID BRO arglist BRC */
#line 99 "src/grammar.lg"
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
	
	basfunc_f fun = basic_getfunc(yymsp[-3].minor.yy0.id);
	if(fun == NULL) {
		basic_error(ERR_FUNC_NOT_FOUND);
	}

	yygotominor.yy0.val = fun(cnt, args);
}
#line 908 "src/grammar.c"
        break;
      case 24: /* arglist ::= */
#line 124 "src/grammar.lg"
{ yygotominor.yy0.arg = NULL; }
#line 913 "src/grammar.c"
        break;
      case 25: /* arglist ::= expr */
#line 126 "src/grammar.lg"
{ 
	yygotominor.yy0.arg = basic_alloc(sizeof(arg_t));
	yygotominor.yy0.arg->value = yymsp[0].minor.yy0.val;
	yygotominor.yy0.arg->next = NULL;
}
#line 922 "src/grammar.c"
        break;
      case 26: /* arglist ::= expr COMMA arglist */
#line 132 "src/grammar.lg"
{
	yygotominor.yy0.arg = basic_alloc(sizeof(arg_t));
	yygotominor.yy0.arg->value = yymsp[-2].minor.yy0.val;
	yygotominor.yy0.arg->next = yymsp[0].minor.yy0.arg;
}
#line 931 "src/grammar.c"
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
#line 54 "src/grammar.lg"
  
  basic_error(ERR_SYNTAX_ERROR);
#line 996 "src/grammar.c"
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
#line 138 "src/grammar.lg"


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
					case TOK_ID:
					{
						int len = *((int*)&tokens[i]);
						i += sizeof(int);
						
						if(conditionalMode != 3) {
							currtok.id = basic_alloc(len + 1);
							str_copy(currtok.id, (char const *)&tokens[i]);
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
					// TODO: Evaluate if this actually solves memory leaking
					basic_memreset();
				}
				else
				{
					// Reset conditional mode
					conditionalMode = 0;
				}
			}
			else if(token_type == TOKEN_EXIT)
			{
				if(conditionalMode != 3)
					break;
				conditionalMode = 0;
			}
			else if(token_type == TOKEN_STOP)
			{
				if(conditionalMode != 3)
					basic_stop_silent = true;
				conditionalMode = 0;
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
		
		return result;
	}
#line 1358 "src/grammar.c"
