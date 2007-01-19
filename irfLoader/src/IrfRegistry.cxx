/**
 * @file IrfRegistry.cxx
 * @brief Class to manage concrete irfInterface implementations.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "irfLoader/IrfLoader.h"
#include "irfLoader/IrfRegistry.h"

namespace irfLoader {

IrfRegistry * IrfRegistry::s_instance(0);

void IrfRegistry::registerLoader(IrfLoader * irfLoader) {
   m_irfLoaders[irfLoader->name()] = irfLoader;
   irfLoader->registerEventClasses();
}

void IrfRegistry::
registerEventClasses(const std::string & name,
                     const std::vector<std::string> & classList) {
   m_respIds[name] = classList;
}

void IrfRegistry::loadIrfs(const std::string & irfsName) {
   if (m_irfLoaders.find(irfsName) != m_irfLoaders.end()) {
      m_irfLoaders[irfsName]->loadIrfs();
   } else {
      throw std::runtime_error("Cannot load IRFs named " + irfsName);
   }
}

std::vector<std::string> IrfRegistry::irfNames() const {
   std::vector<std::string> names;
   std::map<std::string, IrfLoader *>::const_iterator it(m_irfLoaders.begin());
   for ( ; it != m_irfLoaders.end(); ++it) {
      names.push_back(it->first);
   }
   return names;
}

const std::vector<std::string> & 
IrfRegistry::operator[](const std::string & respName) const {
   std::map<std::string, std::vector<std::string> >::const_iterator it;
   if ((it = m_respIds.find(respName)) == m_respIds.end()) {
      throw std::runtime_error("Cannot find response named " + respName);
   }
   return it->second;
}

} // namespace irfLoader
