#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated

//---------------------------------------------------------------------------
std::string boolToString(bool boolean){
  if(boolean == 0){
    return "false";
  }
  if(boolean == 1){
    return "true";
  }
  return "invalid";
}

std::string typeToString(l22::variable_declaration_node * const node){
  std::string type = cdk::to_string(node->type());
  if(type == "(unknown or unsupported type)"){
    return "functype";
  }
  else{
    return type;
  }
}

std::string qualifier(l22::variable_declaration_node * const node){
  if(node->qualifier()  == 1){
    return "isForeign = true";
  }
  if(node->qualifier()  == 2){
    return "isUse = true";
  }
  if(node->qualifier()  == 3){
    return "isPublic = true";
  }
  return "isNormal = true";
}

void l22::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void l22::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void l22::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node,lvl);
}
void l22::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node,lvl);
}
void l22::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node,lvl);
}
void l22::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node,lvl);
}
void l22::xml_writer::do_address_of_node(l22::address_of_node * const node, int lvl) {
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}
void l22::xml_writer::do_null_node(l22::null_node * const node, int lvl) {
  openTag(node,lvl);
  closeTag(node,lvl);
}
void l22::xml_writer::do_variable_declaration_node(l22::variable_declaration_node * const node, int lvl) {
  os()<<std::string(lvl, ' ')<<"<"<<node->label()
  <<qualifier(node)
  <<" id = "<<node->identifier()
  <<" type = "<<typeToString(node) << std::endl;
  openTag("value",lvl+2);
  if(node->initializer() != nullptr){
    node->initializer()->accept(this,lvl+4);
  }
  closeTag("value",lvl+2);
  closeTag(node,lvl);
}

void l22::xml_writer::do_function_call_node(l22::function_call_node * const node, int lvl) {
  openTag(node,lvl);
  openTag("arguments",lvl+2);
  if(node->arguments() != nullptr){
    node->arguments()->accept(this,lvl+4);
  }
  closeTag("arguments",lvl+2);
  closeTag(node,lvl);
}
void l22::xml_writer::do_block_node(l22::block_node * const node, int lvl) {
  openTag(node,lvl);
  openTag("declarations",lvl+2);
  if(node->declarations() != nullptr){
    node->declarations()->accept(this,lvl+4);
  }
  closeTag("declarations",lvl+2);
  openTag("instructions",lvl+2);
  if(node->instructions() != nullptr){
    node->instructions()->accept(this,lvl+4);
  }
  closeTag("instructions",lvl+2);
  closeTag(node,lvl);
}
void l22::xml_writer::do_stop_node(l22::stop_node * const node, int lvl) {
  openTag(node,lvl);
  closeTag(node,lvl);
}
void l22::xml_writer::do_again_node(l22::again_node * const node, int lvl) {
  openTag(node,lvl);
  closeTag(node,lvl);
}
void l22::xml_writer::do_return_node(l22::return_node * const node, int lvl) {
  openTag(node,lvl);
  closeTag(node,lvl);
}
void l22::xml_writer::do_function_node(l22::function_node * const node, int lvl) {
  os()<<std::string(lvl, ' ')<<"<"<<node->label()
  <<" returnType: "<<cdk::to_string(node->type())<<std::endl;
  openTag("fields",lvl+2);
  if(node->fields() != nullptr){
    node->fields()->accept(this,lvl+4);
  }
  closeTag("fields",lvl+2);
  openTag("block",lvl+2);
  if(node->block() != nullptr){
    node->block()->accept(this,lvl+4);
  }
  closeTag("block",lvl+2);
  closeTag(node,lvl);
}
void l22::xml_writer::do_index_node(l22::index_node * const node, int lvl) {
  openTag(node,lvl);
  openTag("base",lvl+2);
  if(node->base() != nullptr){
    node->base()->accept(this,lvl+4);
  }
  closeTag("base",lvl+2);
  openTag("index",lvl+2);
   if(node->index() != nullptr){
    node->index()->accept(this,lvl+4);
  }
  closeTag("index",lvl+2);
  closeTag(node,lvl);
}
void l22::xml_writer::do_stack_alloc_node(l22::stack_alloc_node * const node, int lvl) {
  do_unary_operation(node,lvl);
}
void l22::xml_writer::do_sizeof_node(l22::sizeof_node * const node, int lvl) {
  openTag(node, lvl);
  node->expression()->accept(this, lvl + 2);
  closeTag(node, lvl);
}
void l22::xml_writer::do_identity_node(l22::identity_node * const node, int lvl) {
  do_unary_operation(node,lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void l22::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void l22::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void l22::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void l22::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void l22::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void l22::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl);
  reset_new_symbol();

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_program_node(l22::program_node * const node, int lvl) {
  openTag(node, lvl);
  node->statements()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_evaluation_node(l22::evaluation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void l22::xml_writer::do_print_node(l22::print_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->arguments()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_read_node(l22::read_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_while_node(l22::while_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void l22::xml_writer::do_if_node(l22::if_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void l22::xml_writer::do_if_else_node(l22::if_else_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}
