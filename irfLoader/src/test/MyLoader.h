#ifndef irfLoader_MyLoader_h
#define irfLoader_MyLoader_h

#include <iostream>

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
//      std::cout << "running loadIrfs" << std::endl;
   }
   
   virtual std::string name() const {
      return "my_classes";
   }

};

} // namespace irfLoader

#endif // irfLoader_MyLoader_h
