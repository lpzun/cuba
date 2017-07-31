/*******************************************************************************
 ** Name:    	BoPP: parser
 ** Authors: 	Peizun Liu
 ** Version: 	0.5
 ** Copyright: 	It belongs to Thomas Wahl's group in CAR Lab, CCIS, NEU
 ** Create on:  Feb, 2014
 ** Modified :  Jan, 2016
 ** Decription: BoPP is a Boolean Program Parser written with C++. It aims  at
 *		parsing Boolean programs to generate a control folow graph and 
 *		the corresponding weakest preconditions (strongest postcondit-
 *              ions) for each statement when computing a preimage (postimage).
 *
 *              parser: 
 *              v0.5: adding the forward-based CFG and so on
 ******************************************************************************/
%language "C++"
%locations     // enable location tracking
%error-verbose // verbose error messages

%code requires
{
//# include "aide.hh"
//  using namespace iotf;
}

 // tell Bison that yyparse should take an extra parameter paide
%parse-param { paide &aide }

%define parser_class_name {bp} // define the parser's name
%{
%}

// use Bison's %union construct to define a semantic value type for integers 
// and character pointers (strings)
%union {
  int   t_val; // token's value
  char *t_str; // token's name
}

/* declare tokens */
%token T_BEGIN "begin"
%token T_END "end"

%token T_DECL "decl"
%token T_GOTO "goto"
%token T_ASSUME "assume"
%token T_SKIP "skip"
%token T_ASSERT "assert"
%token T_IF "if"
%token T_FI "fi"
%token T_THEN "then"
%token T_VOID "void"
%token T_CSTR "constrain"
%token T_START_THREAD "start_thread"
%token T_END_THREAD "end_thread"
%token T_ATOMIC_BEGIN "atomic_begin"
%token T_ATOMIC_END "atomic_end"
%token T_WAIT "wait"
%token T_BROADCAST "broadcast"

%token T_NONDET "*"
%token T_ASSIGN ":="
%token T_EQ_OP "="
%token T_NE_OP "!="
%token T_AND   "&"
%token T_OR    "|"
%token T_TERNARY "?"


%token <t_val> T_INT
%token <t_str> T_IDEN

%type <t_str> prm_expr una_expr equ_expr and_expr xor_expr or_expr expr // value
%type <t_str> to_line_list// metastmt statement labelstmt declstmt stmt stmtlist

%start prog
%{
  extern int yylex(yy::bp::semantic_type *yylval, yy::bp::location_type* yylloc);
%}

%initial-action {
  // add filename to location info here
  @$.begin.filename = @$.end.filename = new std::string("stdin");
 }


/*******************************************************************************
 * ** bison rules for BoPP parser 
 * ** BNF: prog
 *	    |-s_decllist 
 *	    |-funclist
 * 	      |-function
 *              |-funchead
 *              |-funcbody
 * 	          |-funcstmt
 * 	             |-l_declstmt
 * 	             |-initstmt
 * 	             |-labelstmt
 * 	               |-expr
 *               |-funcend
 ******************************************************************************/
%%
prog: s_decllist funclist
| funclist
;

funclist: function
| funclist function
;

function: funchead funcend;
|funchead funcbody funcend;

funchead: T_VOID T_IDEN '(' ')' T_BEGIN;

funcend: T_END;

funcbody: funcstmt
| funcbody funcstmt
;

funcstmt: l_declstmt
| initistmt
| labelstmt
;

/// shared variables decls
s_decllist: s_declstmt
| s_decllist s_declstmt
;

s_declstmt: T_DECL s_id_list ';'
;

s_id_list: s_id
| s_id_list ',' s_id
;

s_id: T_IDEN {
  aide.add_vars($1, sool::N, true);
  free($1); // free it to avoid storage leaks
 }
| T_IDEN T_ASSIGN T_NONDET {
  aide.add_vars($1, sool::N, true);
  free($1); // free it to avoid storage leaks
 }
| T_IDEN T_ASSIGN T_INT {
  aide.add_vars($1, $3 == 0 ? sool::F : sool::N, true);
  free($1); // free it to avoid storage leaks
 }
;

/* local decls */
l_declstmt: T_DECL l_id_list ';' { }
;

l_id_list: l_id
| l_id_list ',' l_id {}
;

l_id: T_IDEN {
  aide.add_vars($1, sool::N, false);
  free($1);
 }
| T_IDEN T_ASSIGN T_NONDET {
  aide.add_vars($1, sool::N, false);
  free($1);
 }
| T_IDEN T_ASSIGN T_INT {
  aide.add_vars($1, $3 == 0 ? sool::F : sool::N, false);
  free($1);
 }
;

///////////// stmts /////////////////
/// initialization
initistmt: T_IDEN T_ASSIGN T_NONDET ';' {
  aide.init_vars($1, sool::N);
  free($1);
 }
|T_IDEN T_ASSIGN T_INT ';' {
  aide.init_vars($1, $3 == 0 ? sool::F : sool::N);
  free($1);
 }
;
/// labeling statement
labelstmt: T_INT { 
  ++aide.lineno; // counting the program counters
  aide.ipc = (int)($1); // obtain current pc
  if(!aide.is_pc_unique($1)) // pc's uniqueness
    YYABORT; 
 } ':' statement {
   cout << "TEST:: I am in statement " << $1 <<endl;
   }
;

statement: metastmt
| statement T_AND metastmt // multi-statements
| statement T_OR metastmt
;

metastmt: T_SKIP ';' { // "skip" statement
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::SKIP);	
 }
| T_GOTO {} to_line_list ';' { // "goto" statement
  aide.add_edge(aide.ipc, type_stmt::GOTO);
  aide.suc_pc_set.clear();
 }
| iden_list T_ASSIGN expr_list ';' {// "parallel assignment" statement
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::ASSG);	
  // reset containers
  aide.assg_stmt_lhs.clear();
  aide.assg_stmt_rhs.clear();
 }
| iden_list T_ASSIGN expr_list T_CSTR expr ';' {// "PA with constrain" 
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::ASSG, true);
  // reset containers
  aide.expr_in_list.clear();
  aide.assg_stmt_lhs.clear();
  aide.assg_stmt_rhs.clear();
 }
| T_IF expr T_THEN T_GOTO T_INT ';' T_FI ';' { // "if...then goto..." statement
  aide.add_edge(aide.ipc, $5, type_stmt::IFEL, true);
  aide.expr_in_list.clear();
 } 
| T_ASSERT '(' expr ')' ';' { // "assert" statement
  /// negate the expression in assertions
  aide.expr_in_list.emplace_back(solver::PAR);
  aide.expr_in_list.emplace_back(solver::NEG);
  /// collect all of PCs w.r.t. assertions
  aide.asse_pc_set.insert(aide.ipc);
  
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::ASSE, true);		
  aide.expr_in_list.clear();
 }
| T_ASSUME '(' expr ')' ';' { // "assume" statement
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::ASSU, true);
  aide.expr_in_list.clear();
 }
| T_START_THREAD T_GOTO T_INT ';' { // "thread creation" statement
  aide.add_edge(aide.ipc, $3, type_stmt::NTHR);
 }
| T_END_THREAD ';' { // thread termination statement
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::ETHR);
 }
| T_ATOMIC_BEGIN ';' { // atomic section beginning
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::ATOM);
 }
| T_ATOMIC_END ';' { // atomic section ending
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::EATM);
 }
| T_BROADCAST ';' { // broadcast statement
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::BCST);
 }
| T_WAIT ';' { // wait statement
  aide.add_edge(aide.ipc, aide.ipc+1, type_stmt::WAIT);
 }
;

iden_list: T_IDEN {
  string s($1);
  if(s.back() == '$')
  	s.pop_back();
  aide.assg_stmt_lhs.emplace_back(aide.encode(s));
  free($1);
 }
| iden_list ',' T_IDEN {
  string s($3);
  if(s.back() == '$')
  	s.pop_back();
  aide.assg_stmt_lhs.emplace_back(aide.encode(s));
  free($3); 
  }
;

expr_list: expr { 
  aide.assg_stmt_rhs.emplace_back(aide.expr_in_list); 
  aide.expr_in_list.clear();
 }
| expr_list ',' expr { 
  aide.assg_stmt_rhs.emplace_back(aide.expr_in_list); 
  aide.expr_in_list.clear(); 
  }
;

to_line_list: T_INT  {
  aide.suc_pc_set.emplace($1);
 }
| to_line_list ',' T_INT {
  aide.suc_pc_set.emplace($3);
  }
;

/* expressions */
expr: or_expr { }
| expr T_TERNARY expr ':' or_expr { }
;

or_expr: xor_expr
| or_expr T_OR xor_expr { aide.add_to_expr_in_list(solver::OR); }
;

xor_expr: and_expr
| xor_expr '^' and_expr { aide.add_to_expr_in_list(solver::XOR); }
;

and_expr: equ_expr
| and_expr T_AND equ_expr { aide.add_to_expr_in_list(solver::AND); }
;

equ_expr: una_expr
| equ_expr T_EQ_OP una_expr { aide.add_to_expr_in_list( solver::EQ); }
| equ_expr T_NE_OP una_expr { aide.add_to_expr_in_list(solver::NEQ); }
;

una_op: '!' 
;

una_expr: prm_expr
| una_op prm_expr { aide.add_to_expr_in_list(solver::NEG); }
;

prm_expr: '(' expr ')' { aide.add_to_expr_in_list(solver::PAR); }
| T_NONDET { aide.add_to_expr_in_list(solver::CONST_N); }
| T_INT    { aide.add_to_expr_in_list($1); }
| T_IDEN { 
  aide.add_to_expr_in_list(aide.encode($1));
  free($1);
  }
;
%%

/*******************************************************************************
 * ** From here, 
 *         functions used in parser, defined in c++
 *
 *    Mar. 2013
 ******************************************************************************/
namespace yy {
  void bp::error(location const &loc, const std::string& s) {
    std::cerr << "error at " << loc << ": " << s << std::endl;
  }
}
