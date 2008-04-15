/**
 * @file MyIrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$ */

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

   classNames.at(0) = "P6_v1_transient/front";
   classNames.at(1) = "P6_v1_transient/back";
   registry.registerEventClasses("P6_v1_trans", classNames);
   registry.registerEventClass("P6_v1_transient_front", classNames.at(0));
   registry.registerEventClass("P6_v1_transient_back", classNames.at(1));

   classNames.at(0) = "P6_v1_source/front";
   classNames.at(1) = "P6_v1_source/back";
   registry.registerEventClasses("P6_v1_source", classNames);
   registry.registerEventClass("P6_v1_source_front", classNames.at(0));
   registry.registerEventClass("P6_v1_source_back", classNames.at(1));

   classNames.at(0) = "P6_v1_diffuse/front";
   classNames.at(1) = "P6_v1_diffuse/back";
   registry.registerEventClasses("P6_v1_diffuse", classNames);
   registry.registerEventClass("P6_v1_diffuse_front", classNames.at(0));
   registry.registerEventClass("P6_v1_diffuse_back", classNames.at(1));

}

void MyIrfLoader::loadIrfs() const {
   m_className = load_irfs();
}

} // namespace
