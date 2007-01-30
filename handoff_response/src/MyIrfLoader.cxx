/**
 * @file MyIrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/IrfRegistry.h"

#include "handoff_response/loadIrfs.h"
#include "handoff_response/MyIrfLoader.h"

namespace handoff_response {

void MyIrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * class_names[] = {"standard/front", "standard/back"};
   std::vector<std::string> classNames(class_names, class_names + 2);
   registry.registerEventClasses("HANDOFF", classNames);
}

void MyIrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace handoff_response
