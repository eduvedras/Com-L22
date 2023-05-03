#ifndef __L22_AST_FUNCTION_CALL_H__
#define __L22_AST_FUNCTION_CALL_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "ast/function_node.h"

namespace l22 {

  //!
  //! Class for describing function call nodes.
  //!
  //!
  class function_call_node: public cdk::expression_node {
    cdk::expression_node *_pointer;
    cdk::sequence_node *_arguments;

  public:
    /**
     * Constructor for a function call without arguments.
     * An empty sequence is automatically inserted to represent
     * the missing arguments.
     */
    function_call_node(int lineno) :
        cdk::expression_node(lineno), _arguments(new cdk::sequence_node(lineno)) {
    }

    /**
     * Constructor for a function call with arguments.
     */
    function_call_node(int lineno, cdk::expression_node *pointer,cdk::sequence_node *arguments) :
        cdk::expression_node(lineno), _pointer(pointer), _arguments(arguments) {
    }

  public:
    cdk::expression_node* pointer() const {
      return _pointer;
    }
    cdk::sequence_node* arguments() {
      return _arguments;
    }
    cdk::expression_node *argument(size_t ix) {
      return dynamic_cast<cdk::expression_node*>(_arguments->node(ix));
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // l22

#endif