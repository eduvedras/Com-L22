%option c++ prefix="l22_scanner_" outfile="l22_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "l22_parser.tab.h"

static bool restart_functions = false;

static std::ostringstream strlit;
static int lLevelsToPop = 0, lIndex = 0;
static std::vector<int> indents;   // known indentation levels

inline bool valid(int indent) {
    lLevelsToPop = lIndex = -1;
    bool indentOk = false;
    for (size_t ix = 0; ix < indents.size(); ix++) {
        if (indent == indents[ix]) {
            lLevelsToPop = indents.size() - ix - 1;
            lIndex = ix;
            indentOk = true;
        }
    }
    return indentOk;
}

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_COMMENT X_BACKSLASH X_IDENTATION X_NEWLINE X_IGNORE

IDENTIFIER             [a-zA-Z][0-9a-zA-Z_]*
SYMBOL                 [-+%\]()<>*/?,[:=@]
DIGIT                  [0-9]
EXPONENT               [eE][-+]?{DIGIT}+
REAL                   {DIGIT}+{EXPONENT}|({DIGIT}+"."{DIGIT}*|{DIGIT}*"."{DIGIT}+){EXPONENT}?
B7_INT                 0[0-6]+
B10_INT                [1-9]{DIGIT}*|0

%%
  yydebug=1; set_debug(1);

  {
        //std::cerr << "################" << std::endl;
        set_debug(true);
        if (restart_functions) {
          std::cerr << "POPPING FUNCTIONS (" << indents.size() << ")" << std::endl;
          indents.pop_back();
          if (indents.size() == 0) restart_functions = false;
          return '}';
        }
        else {
          if (lLevelsToPop > 0) {
            std::cerr << "POPPING LEVELS (" << lLevelsToPop << ")" << std::endl;
            lLevelsToPop--;
            indents.pop_back();
            return '}';
          }
        }
        
  }


">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 

"or"                   return tOR;
"and"                  return tAND;
"not"                  return tNOT;

"int"                  return tTYPEINT;
"double"               return tTYPEDOUBLE;
"text"                 return tTYPETEXT;
"void"                 return tTYPEVOID;

"foreign"              return tFOREIGN;
"use"                  return tUSE;
"public"               return tPUBLIC;
"var"                  return tVAR;

"while"                return tWHILE; 
"if"                   return tIF; 
"elif"                 return tELIF;
"write"                return tWRITE; 
"writeln"              return tWRITELN; 
"begin"                return tBEGIN; 
"stop"                 return tSTOP;
"again"                return tAGAIN;
"input"                return tINPUT;
"null"                 return tNULL;
"sizeof"               return tSIZEOF;
"then:"                return tTHEN;
"else:"                return tELSE;
"do:"                  return tDO; 
"end"                  return tEND;
"return"               return tRETURN;
"->"                   return tARROW;

{IDENTIFIER}  yylval.s = new std::string(yytext); return tIDENTIFIER;
{SYMBOL}                       {return *yytext;}
{REAL}                         {yylval.d = strtod(yytext,NULL);  if(errno==ERANGE) yyerror("Overflow..."); return tDOUBLE;}
{B10_INT}                      {yylval.i = strtol(yytext,NULL,10);  if(errno==ERANGE) yyerror("Overflow..."); return tINTEGER;}
{B7_INT}                       {yylval.i = strtol(yytext,NULL,7); if(errno==ERANGE) yyerror("Overflow..."); return tINTEGER;}

\n                   yy_push_state(X_NEWLINE); 
<X_NEWLINE>^" "+     {
    yy_pop_state();

    if (valid(yyleng)) {
          std::cerr << "CASE 1" << std::endl;
          if (lLevelsToPop == 0) {
            // at the same level: ignore index
            // do not return tINDENT;
            std::cerr << "CASE 1a (levels to pop = 0)" << std::endl;
            return ';';
          }
          else {
            // nothing is done here: the scanner will ignore the input while levels > 0
            // and send block-end tokens to the parser
            std::cerr << "CASE 1b (levels to pop = " << lLevelsToPop << ")" << std::endl;
            // at least one has to be returned here...
            if (lLevelsToPop > 0) {
              std::cerr << "POPPING LEVELS (" << lLevelsToPop << ")" << std::endl;
              lLevelsToPop--;
              indents.pop_back();
              return '}';
            }
          }
    }
    else if (indents.size() == 0) {
          // first block to open
          std::cerr << "CASE 2" << std::endl;
          lLevelsToPop++;
          indents.push_back(yyleng);
          return '{';
    }
    else if (yyleng > indents.back()) {
          // this is like the one before, but not the first
          std::cerr << "CASE 3" << std::endl;
          lLevelsToPop++;
          indents.push_back(yyleng);
          return '{';
    }
    else {
          // something wrong: bad indent: communicate to user
          std::cerr << "CASE 4" << std::endl;
          std::cerr << "bad indent at line " << yylineno;
    }

}
<X_NEWLINE>\n      ; // ignore successive newline chars
<X_NEWLINE>.       {
  // in this case, we have a new function starting
  // first, return whatever we are seeing to the input
  yyless(0);
  yy_pop_state();
  if (indents.size() > 0) {
    std::cerr << "should restart " << indents.size() << " functions at " << yylineno << std::endl;
    indents.pop_back();
    if (indents.size() > 0) { restart_functions = true; }
    return '}';
  }
  else return ';'; // top-level declarations
}

"\""                           yy_push_state(X_STRING);
<X_STRING>\\                   yy_push_state(X_BACKSLASH);
<X_STRING>"\""                 {
                                 yylval.s = new std::string(strlit.str());
                                 strlit.str("");
                                 yy_pop_state();
                                 return tSTRING;
                               }
<X_STRING>.                    strlit << *yytext;

<X_BACKSLASH>n                 strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                 strlit << '\r'; yy_pop_state();
<X_BACKSLASH>t                 strlit << '\t'; yy_pop_state();
<X_BACKSLASH>\\                strlit << '\\'; yy_pop_state();
<X_BACKSLASH>"\""              strlit << '\"'; yy_pop_state();

<X_BACKSLASH>0                 yy_pop_state();yy_pop_state();yy_push_state(X_IGNORE);
<X_BACKSLASH>00                yy_pop_state();yy_pop_state();yy_push_state(X_IGNORE);
<X_BACKSLASH>000               yy_pop_state();yy_pop_state();yy_push_state(X_IGNORE);

<X_BACKSLASH>[0-6]{1,3} {
                                 strlit << (char)(unsigned char)strtoul(yytext, NULL, 7);
                                 if(errno==ERANGE) yyerror("Overflow...");
                                 yy_pop_state();
                               }
<X_BACKSLASH>.                 strlit << *yytext; yy_pop_state();

<X_IGNORE>"\""                {
                                 yylval.s = new std::string(strlit.str());
                                 strlit.str("");
                                 yy_pop_state();
                                 return tSTRING;
                               }

<X_IGNORE>\\\"                 ;
<X_IGNORE>\\\\                 ;
<X_IGNORE>.|\n                 ;

[ ]*"..."\n        ; /* ignore continuation marks */

"(*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"(*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*)"        yy_pop_state();
<X_COMMENT>.|\n        ;

";".*$                 ;

[ ]+               ; /* ignore whitespace */

.                      yyerror("Unknown character");

%%
