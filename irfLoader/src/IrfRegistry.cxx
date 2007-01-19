/**
 * @file IrfRegistry.cxx
 * @brief Class to manage concrete irfInterface implementations.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfLoader/IrfLoader.h"
#include "irfLoader/IrfRegistry.h"

namespace irfLoader {

IrfRegistry * IrfRegistry::s_instance(0);

void IrfRegistry::registerLoader(IrfLoader * irfLoader) {
   m_irfLoaders[irfLoader->name()] = irfLoader;
   irfLoader->registerEventClasses();
}

void IrfRegistry::loadIrfs(const std::string & irfsName) {
   m_irfLoaders[irfsName]->loadIrfs();
}

} // namespace irfLoader
