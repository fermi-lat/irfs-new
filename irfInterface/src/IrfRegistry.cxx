/**
 * @file IrfRegistry.cxx
 * @brief Class to manage concrete irfInterface implementations.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "irfInterface/IrfLoader.h"
#include "irfInterface/IrfRegistry.h"

namespace irfInterface {

IrfRegistry * IrfRegistry::s_instance(0);

IrfRegistry & IrfRegistry::instance() {
   if (s_instance == 0) {
      s_instance = new IrfRegistry();
   }
   return *s_instance;
}

void IrfRegistry::registerLoader(IrfLoader * irfLoader) {
   m_irfLoaders[irfLoader->name()] = irfLoader;
   irfLoader->registerEventClasses();
}

void IrfRegistry::
registerEventClasses(const std::string & name,
                     const char ** class_list) {
   size_t nclasses(sizeof(class_list)/sizeof(void*));
   std::vector<std::string> classList(class_list, class_list + nclasses);
   m_respIds[name] = classList;
}

void IrfRegistry::
registerEventClasses(const std::string & name,
                     const std::vector<std::string> & classList) {
   m_respIds[name] = classList;
}

void IrfRegistry::
registerEventClasses(const std::string & name, const std::string & className) {
   std::vector<std::string> classList(1, className);
   m_respIds[name] = classList;
}

void IrfRegistry::loadIrfs(const std::string & irfsName) {
   if (m_irfLoaders.find(irfsName) != m_irfLoaders.end()) {
      m_irfLoaders[irfsName]->loadIrfs();
   } else {
      throw std::runtime_error("IrfRegistry::loadIrfs: Cannot load IRFs named "
                               + irfsName);
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
      throw std::runtime_error("IrfRegistry: Cannot find response named " 
                               + respName);
   }
   return it->second;
}

} // namespace irfInterface
