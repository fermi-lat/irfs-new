#ifndef irfLoader_MyLoader_h
#define irfLoader_MyLoader_h

#include <iostream>

#include "irfInterface/IrfsFactory.h"
#include "irfInterface/IrfLoader.h"
#include "irfInterface/IrfRegistry.h"

namespace irfLoader {

class MyLoader : public irfInterface::IrfLoader {

public:

   virtual void registerEventClasses() const {
      irfInterface::IrfRegistry & registry = 
         irfInterface::IrfRegistry::instance();
      char * class_names[] = {"FrontA", "BackA", "FrontB", "BackB"};
      std::vector<std::string> classNames(class_names, class_names + 4);
      registry.registerEventClasses(name(), classNames);
   }

   virtual void loadIrfs() const {
      char * class_names[] = {"FrontA", "BackA", "FrontB", "BackB"};
      std::vector<std::string> names(class_names, class_names + 4);
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
