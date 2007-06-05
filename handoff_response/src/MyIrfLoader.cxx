/**
 * @file MyIrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>

#include "irfInterface/IrfRegistry.h"

#include "handoff_response/loadIrfs.h"
#include "handoff_response/MyIrfLoader.h"

namespace handoff_response {

void MyIrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * class_names[] = {"standard/front", "standard/back"};
   std::vector<std::string> classNames(class_names, class_names + 2);
   classNames.at(0) = m_className + "/front";
   classNames.at(1) = m_className + "/back";
   registry.registerEventClasses("HANDOFF", classNames);
   registry.registerEventClass("HANDOFF_front", classNames.at(0));
   registry.registerEventClass("HANDOFF_back", classNames.at(1));
}

void MyIrfLoader::loadIrfs() const {
   m_className = load_irfs();
}

} // namespace handoff_response
