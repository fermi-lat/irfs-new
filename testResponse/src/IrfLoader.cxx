/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfLoader/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfLoader/IrfLoaderFactory.h"
#include "irfLoader/IrfRegistry.h"

#include "IrfLoader.h"

namespace testResponse {

void load_irfs();

irfLoader::IrfLoaderFactory<IrfLoader> loaderFactory;

void IrfLoader::registerEventClasses() const {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   std::vector<std::string> classNames;
   classNames.push_back("testIrfs::Front");
   classNames.push_back("testIrfs::Back");
   registry.registerEventClasses("TEST", classNames);
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace testResponse
