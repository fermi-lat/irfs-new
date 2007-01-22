#ifndef irfInterface_MyIrfLoader_h
#define irfInterface_MyIrfLoader_h

#include "irfInterface/IrfLoader.h"
#include "irfInterface/IrfRegistry.h"

#include "Aeff.h"
#include "Edisp.h"
#include "Psf.h"

namespace irfInterface {

class MyIrfLoader : public IrfLoader {

public:

   virtual void registerEventClasses() const {
      IrfRegistry & registry(IrfRegistry::instance());
      char * class_names[] = {"FrontA", "BackA", "FrontB", "BackB"};
      std::vector<std::string> classNames(class_names, class_names + 4);
      registry.registerEventClasses(name(), classNames);
   }

   virtual void loadIrfs() const {
      IrfRegistry & registry(IrfRegistry::instance());
      std::vector<std::string> names(registry[name()]);
      for (size_t i(0); i < names.size(); i++) {
         Irfs * myIrfs = new Irfs(new Aeff(), new Psf(), new Edisp, i);
         irfInterface::IrfsFactory::
            instance()->addIrfs(name() + "::" + names.at(i), myIrfs, false);
      }
   }
   
   virtual std::string name() const {
      return "my_classes";
   }

};

} // namespace irfInterface

#endif // irfInterface_MyIrfLoader_h
