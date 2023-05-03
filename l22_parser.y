%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;    /* double value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  l22::block_node      *block;
  std::vector<std::shared_ptr<cdk::basic_type>> *types;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token tARROW tWHILE tVAR tIF tOR tINPUT tBEGIN tEND tAND tNOT tTYPEINT tTYPEDOUBLE tTYPETEXT tTYPEVOID tFOREIGN tUSE tPUBLIC tELIF tWRITE tWRITELN tSTOP tAGAIN tRETURN tNULL tSIZEOF tTHEN tELSE tDO

%right '='
%left tOR
%left tAND
%right tNOT
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-' '('
%left '*' '/' '%'

%nonassoc tUNARY
%nonassoc '['

%type <node> program var param ifs cicles elif instr
%type <sequence> fparams instrs exprs params decls file opt_exprs
%type <expression> expr funcall literal func opt_expr
%type <lvalue> lval
%type <type> type functype general_type
%type<types> func_types
%type <s> string
%type <block> block

%define parse.error verbose

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%


file  : decls ';' program  {compiler->ast($$ = new cdk::sequence_node(LINE, $3, $1));}
      | decls program      {compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1));}
      | program           {compiler->ast($$ = new cdk::sequence_node(LINE, $1));}
      ;

program	: tBEGIN block tEND           {$$ = new l22::program_node(LINE, $2);}
	     ;

var  : type tIDENTIFIER                      {$$ = new l22::variable_declaration_node(LINE,0,$1, *$2,nullptr);}
     | type tIDENTIFIER '=' expr             {$$ = new l22::variable_declaration_node(LINE,0,$1, *$2, $4);}
     | tVAR tIDENTIFIER '=' expr             {$$ = new l22::variable_declaration_node(LINE,0, cdk::primitive_type::create(8,cdk::TYPE_UNSPEC), *$2, $4);}
     | tPUBLIC type tIDENTIFIER '=' expr     {$$ = new l22::variable_declaration_node(LINE,3,$2, *$3, $5);}
     | tFOREIGN type tIDENTIFIER             {$$ = new l22::variable_declaration_node(LINE,1,$2, *$3, nullptr);}
     | tUSE type tIDENTIFIER                 {$$ = new l22::variable_declaration_node(LINE,2,$2, *$3, nullptr);}
     | tPUBLIC tVAR tIDENTIFIER '=' expr     {$$ = new l22::variable_declaration_node(LINE,3,cdk::primitive_type::create(8,cdk::TYPE_UNSPEC), *$3, $5);}
     | tPUBLIC tIDENTIFIER '=' expr          {$$ = new l22::variable_declaration_node(LINE,3,cdk::primitive_type::create(8,cdk::TYPE_UNSPEC), *$2, $4);}
     ;

func : '(' fparams ')' tARROW general_type ':' block       {$$ = new l22::function_node(LINE,$2,$5,$7);}
     ;

fparams : params {$$ = $1;}
        |        {$$ = new cdk::sequence_node(LINE);}
        ;

params : param            {$$ = new cdk::sequence_node(LINE, $1);}
       | params ',' param {$$ = new cdk::sequence_node(LINE, $3, $1);}
       ;

param : type tIDENTIFIER {$$ = new l22::variable_declaration_node(LINE,0,$1,*$2,nullptr);}
      ;

type : tTYPEINT     {$$ = cdk::primitive_type::create(4, cdk::TYPE_INT);}
     | tTYPEDOUBLE  {$$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE);}
     | tTYPETEXT    {$$ = cdk::primitive_type::create(4, cdk::TYPE_STRING);}
     | '[' general_type ']' {$$ = cdk::reference_type::create(4, $2);}
     | functype     {$$ = $1;}
     ;

general_type : type           {$$ = $1;}
             | tTYPEVOID    {$$ = cdk::primitive_type::create(0, cdk::TYPE_VOID);}
             ;

functype : general_type '<' func_types '>'  {$$ = cdk::functional_type::create(*$3,$1);}
         | general_type '<' '>'             {$$ = cdk::functional_type::create($1);}
         ;

func_types : type                   {$$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1);}
           | func_types ',' type    {$$ = $1;$$->push_back($3);}


block : '{' '}'                   {$$ = new l22::block_node(LINE,new cdk::sequence_node(LINE), new cdk::sequence_node(LINE));}
      | '{' decls '}'             {$$ = new l22::block_node(LINE, $2, new cdk::sequence_node(LINE));}
      | '{' instrs '}'            {$$ = new l22::block_node(LINE,new cdk::sequence_node(LINE), $2);}
      | '{' decls ';' instrs '}'  {$$ = new l22::block_node(LINE,$2, $4);}
      | '{' decls instrs '}'      {$$ = new l22::block_node(LINE,$2, $3);}
      ;

decls : var            {$$ = new cdk::sequence_node(LINE,$1);}
      | decls ';' var  {$$ = new cdk::sequence_node(LINE,$3,$1);}
      | decls var      {$$ = new cdk::sequence_node(LINE,$2,$1);}
      ;

instrs : instr                {$$ = new cdk::sequence_node(LINE,$1);}
       | instrs ';' instr     {$$ = new cdk::sequence_node(LINE,$3,$1);}
       | instrs instr         {$$ = new cdk::sequence_node(LINE,$2,$1);}
       ;

instr : expr             {$$ = new l22::evaluation_node(LINE,$1);}
      | tWRITE exprs     {$$ = new l22::print_node(LINE,$2,false);}
      | tWRITELN exprs   {$$ = new l22::print_node(LINE,$2,true);}
      | tSTOP            {$$ = new l22::stop_node(LINE);}
      | tAGAIN           {$$ = new l22::again_node(LINE);}
      | tRETURN opt_expr     {$$ = new l22::return_node(LINE,$2);}
      | ifs              {$$ = $1;}
      | cicles           {$$ = $1;}
      | block            {$$ = $1;}
      ;

opt_expr : /*empty*/ {$$ = nullptr;}
         | expr      {$$=$1;}
         ;

ifs : tIF '(' expr ')' tTHEN block          {$$ = new l22::if_node(LINE,$3,$6);}
    | tIF '(' expr ')' tTHEN block elif     {$$ = new l22::if_else_node(LINE,$3,$6,$7);}
    ;


elif : tELSE block                             {$$ = $2;}
     | tELIF '(' expr ')' tTHEN block          {$$ = new l22::if_node(LINE,$3,$6);}
     | tELIF '(' expr ')' tTHEN block elif     {$$ = new l22::if_else_node(LINE,$3,$6,$7);}
     ;

cicles : tWHILE '(' expr ')' tDO block          {$$ = new l22::while_node(LINE,$3,$6);}
       ;
    


expr : literal                 {$$ = $1;}   
     | tINPUT                  {$$ = new l22::read_node(LINE);}
     | '-' expr %prec tUNARY   {$$ = new cdk::neg_node(LINE, $2);}
     | '+' expr %prec tUNARY   {$$ = new l22::identity_node(LINE,$2);}
     | tNOT expr               {$$ = new cdk::not_node(LINE,$2);}
     | expr tAND expr          {$$ = new cdk::and_node(LINE,$1, $3);}
     | expr tOR expr           {$$ = new cdk::or_node(LINE,$1, $3);}
     | expr '+' expr	      {$$ = new cdk::add_node(LINE, $1, $3);}
     | expr '-' expr	      {$$ = new cdk::sub_node(LINE, $1, $3);}
     | expr '*' expr	      {$$ = new cdk::mul_node(LINE, $1, $3);}
     | expr '/' expr	      {$$ = new cdk::div_node(LINE, $1, $3);}
     | expr '%' expr	      {$$ = new cdk::mod_node(LINE, $1, $3);}
     | expr '<' expr	      {$$ = new cdk::lt_node(LINE, $1, $3);}
     | expr '>' expr	      {$$ = new cdk::gt_node(LINE, $1, $3);}
     | expr tGE expr	      {$$ = new cdk::ge_node(LINE, $1, $3);}
     | expr tLE expr           {$$ = new cdk::le_node(LINE, $1, $3);}
     | expr tNE expr	      {$$ = new cdk::ne_node(LINE, $1, $3);}
     | expr tEQ expr	      {$$ = new cdk::eq_node(LINE, $1, $3);}
     | '(' expr ')'            {$$ = $2;}
     | '[' expr ']'            {$$ = new l22::stack_alloc_node(LINE,$2);}
     | tSIZEOF '(' expr ')'    {$$ = new l22::sizeof_node(LINE,$3);}
     | lval                    {$$ = new cdk::rvalue_node(LINE, $1);}  //FIXME
     | lval '=' expr           {$$ = new cdk::assignment_node(LINE, $1, $3);}
     | lval '?'                {$$ = new l22::address_of_node(LINE, $1);}
     | funcall                 {$$ = $1;}
     | func                    {$$ = $1;}
     ;

literal : tINTEGER   {$$ = new cdk::integer_node(LINE, $1);}
        | tDOUBLE    {$$ = new cdk::double_node(LINE,$1);}
        | string     {$$ = new cdk::string_node(LINE,$1);}
        | tNULL      {$$ = new l22::null_node(LINE);}
        ;
     
string : string tSTRING       {$$ = new std::string(*$1+*$2); delete $1; delete $2;}
       | tSTRING              {$$ = $1;}
       ;

lval : tIDENTIFIER             {$$ = new cdk::variable_node(LINE, $1);}
     | expr '[' expr ']'       {$$ = new l22::index_node(LINE,$1,$3);}
     ;

funcall : expr '(' opt_exprs ')'           {$$ = new l22::function_call_node(LINE, $1, $3);}
        | '@''(' opt_exprs ')'             {$$ = new l22::function_call_node(LINE,nullptr, $3);}
        | '(' func ')' '(' opt_exprs ')'   {$$ = new l22::function_call_node(LINE,$2, $5);}
        ;

exprs : expr                  {$$ = new cdk::sequence_node(LINE,$1);}
      | exprs ',' expr        {$$ = new cdk::sequence_node(LINE,$3,$1);}
      ;

opt_exprs : /*empty*/    {$$ = new cdk::sequence_node(LINE);}
          | exprs        {$$=$1;}
          ;

%%
