#ifndef __L22_FACTORY_H__
#define __L22_FACTORY_H__

#include <memory>
#include <cdk/yy_factory.h>
#include "l22_scanner.h"

namespace l22 {

  /**
   * This class implements the compiler factory for the Simple compiler.
   */
  class factory: public cdk::yy_factory<l22_scanner> {
    /**
     * This object is automatically registered by the constructor in the
     * superclass' language registry.
     */
    static factory _self;

  protected:
    /**
     * @param language name of the language handled by this factory (see .cpp file)
     */
    factory(const std::string &language = "l22") :
        cdk::yy_factory<l22_scanner>(language) {
    }

  };

} // l22

#endif
