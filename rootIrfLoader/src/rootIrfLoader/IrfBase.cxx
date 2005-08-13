/**
 * @file IrfBase.cxx
 * @brief Base class for irfInterface wrapper classes for use from ROOT
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "irfInterface/IrfsFactory.h"
#include "irfInterface/Irfs.h"
#include "irfLoader/Loader.h"

#include "IrfBase.h"

namespace rootIrfLoader {

bool IrfBase::s_irfsLoaded(false);

IrfBase::IrfBase(const std::string & irfsName) : m_currentIrfs(irfsName) {
   if (!s_irfsLoaded) {
      irfLoader::Loader::go();
      s_irfsLoaded = true;
   }

   irfInterface::IrfsFactory * my_factory = 
      irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * irfs(0);
   try {
      irfs = my_factory->create(irfsName);
      delete irfs;
   } catch (std::exception & eObj) {
      m_currentIrfs = "DC1A::Front";
      std::cout << eObj.what() << "\n"
                << "Using the default, " << m_currentIrfs 
                << std::endl;
   }
}

void IrfBase::setIrfs(const std::string & irfsName) {
   irfInterface::IrfsFactory * my_factory = 
      irfInterface::IrfsFactory::instance();

   irfInterface::Irfs * irfs(0);

   try {
      irfs = my_factory->create(irfsName);
      getRef(irfs);
      m_currentIrfs = irfsName;
      delete irfs;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << "\n"
                << "Keeping the current set, " << m_currentIrfs
                << std::endl;
   }
}

} // namespace rootIrfLoader
