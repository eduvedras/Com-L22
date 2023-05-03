#ifndef __L22_TUPLE_NODE_H__
#define __L22_TUPLE_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/block_node.h"

namespace l22 {

  class function_node: public cdk::expression_node {
    cdk::sequence_node *_fields;
    l22::block_node *_block;

  public:
    function_node(int lineno, cdk::sequence_node *fields, std::shared_ptr<cdk::basic_type> returnType,  l22::block_node *block) :
        cdk::expression_node(lineno), _fields(fields), _block(block)  {
          this->type(returnType);
    }

  public:
    inline cdk::sequence_node* fields() {
      return _fields;
    }

    inline l22::block_node* block() {
      return _block;
    }

    cdk::typed_node* argument(size_t ax) {
      return dynamic_cast<cdk::typed_node*>(_fields->node(ax));
    }

  public:
    inline void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_node(this, level);
    }

  };

}// l22

#endif