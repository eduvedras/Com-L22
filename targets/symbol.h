#ifndef __L22_TARGETS_SYMBOL_H__
#define __L22_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace l22 {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    long _value; // hack!
    bool _function; // false for variables
    std::vector<std::shared_ptr<cdk::basic_type>> _argument_types;
    int _qualifier; 
    bool _initialized;
    int _offset = 0; // 0 (zero) means global variable/function

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, long value,bool function,int qualifier, bool initialized) :
        _type(type), _name(name), _value(value), _function(function), _qualifier(qualifier), _initialized(initialized) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    void set_type(std::shared_ptr<cdk::basic_type> t) {
      _type = t;
    }
    const std::string &name() const {
      return _name;
    }
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }
    bool isFunction() const {
      return _function;
    }
    int qualifier() const {
      return _qualifier;
    }
    bool initialized() const {
      return _initialized;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }
    bool global() const {
      return _offset == 0;
    }
    void set_argument_types(const std::vector<std::shared_ptr<cdk::basic_type>> &types) {
      _argument_types = types;
    }

    bool argument_is_typed(size_t ax, cdk::typename_type name) const {
      return _argument_types[ax]->name() == name;
    }
    std::shared_ptr<cdk::basic_type> argument_type(size_t ax) const {
      return _argument_types[ax];
    }

    size_t argument_size(size_t ax) const {
      return _argument_types[ax]->size();
    }

    size_t number_of_arguments() const {
      return _argument_types.size();
    }
  };

  inline auto make_symbol( std::shared_ptr<cdk::basic_type> type, const std::string &name,int value,
                          bool function, int qualifier, bool initialized) {
    return std::make_shared<symbol>( type, name, value, function,qualifier,initialized);
  }

} // l22

#endif
