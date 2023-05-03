#ifndef __L22_AST_PRINT_NODE_H__
#define __L22_AST_PRINT_NODE_H__

#include <cdk/ast/expression_node.h>

namespace l22 {

  /**
   * Class for describing print nodes.
   */
  class print_node: public cdk::basic_node {
    cdk::sequence_node *_arguments;
    bool _newline = false;

  public:
    inline print_node(int lineno, cdk::sequence_node *arguments, bool newline) :
        cdk::basic_node(lineno), _arguments(arguments), _newline(newline) {
    }

  public:
    inline cdk::sequence_node *arguments () {
      return _arguments;
    }

    bool newline() {
      return _newline;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_print_node(this, level);
    }

  };

} // l22

#endif
