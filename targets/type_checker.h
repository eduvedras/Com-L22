#ifndef __L22_TARGETS_TYPE_CHECKER_H__
#define __L22_TARGETS_TYPE_CHECKER_H__

#include "targets/basic_ast_visitor.h"
#include <cdk/types/functional_type.h>

namespace l22 {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public basic_ast_visitor {
    cdk::symbol_table<l22::symbol> &_symtab;
    basic_ast_visitor *_parent;
    int _funclevel = 0;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<l22::symbol> &symtab, basic_ast_visitor *parent) :
        basic_ast_visitor(compiler), _symtab(symtab), _parent(parent) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    void do_BooleanLogicalExpression(cdk::binary_operation_node *const node, int lvl);

    void processPIDExpression(cdk::binary_operation_node *const node, int lvl);
    std::string pointRefTypeToString(std::shared_ptr<cdk::basic_type> type);
    void processIDExpression(cdk::binary_operation_node *const node, int lvl);
    void processIntExpression(cdk::binary_operation_node *const node, int lvl);
    void processScalarLogicalExpression(cdk::binary_operation_node *const node, int lvl);
    void processGeneralLogicalExpression(cdk::binary_operation_node *const node, int lvl);
    template<typename T>
    void process_literal(cdk::literal_node<T> *const node, int lvl) {
    }

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end

  };

} // l22

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, node) { \
  try { \
    l22::type_checker checker(compiler, symtab, this); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, node)

#endif
