#ifndef irfLoader_MyLoader_h
#define irfLoader_MyLoader_h

#include <iostream>

#include "irfInterface/IrfsFactory.h"

#include "irfLoader/IrfLoader.h"
#include "irfLoader/IrfRegistry.h"

namespace irfLoader {

class MyLoader : public IrfLoader {

public:

   virtual void registerEventClasses() const {
      IrfRegistry & registry(*IrfRegistry::instance());
      char * class_names[] = {"FrontA", "BackA", "FrontB", "BackB"};
      std::vector<std::string> classNames(class_names, class_names + 4);
      registry.registerEventClasses(name(), classNames);
   }

   virtual void loadIrfs() const {
      IrfRegistry & registry(*IrfRegistry::instance());
      std::vector<std::string> names(registry[name()]);
      for (size_t i(0); i < names.size(); i++) {
         irfInterface::IrfsFactory::
            instance()->addIrfs(name() + "::" + names.at(i), 0, false);
      }
   }
   
   virtual std::string name() const {
      return "my_classes";
   }

};

} // namespace irfLoader

#endif // irfLoader_MyLoader_h
