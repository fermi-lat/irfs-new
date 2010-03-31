/**
 * @file Registrar.cxx
 * @brief Manage irfInterface::IrfLoader instances for each set of IRFs
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "facilities/commonUtilities.h"

#include "irfInterface/IrfRegistry.h"

// #include "dc1Response/IrfLoader.h"
#include "dc1aResponse/IrfLoader.h"
// #include "dc2Response/IrfLoader.h"
// #include "g25Response/IrfLoader.h"
#include "latResponse/IrfLoader.h"
#include "testResponse/IrfLoader.h"
//#include "egretResponse/IrfLoader.h"

#include "Registrar.h"

namespace irfLoader {

Registrar::Registrar() {
   char * caldb(::getenv("CALDB"));
   facilities::commonUtilities::setupEnvironment();
   if (caldb) {
      facilities::commonUtilities::setEnvironment("CALDB", caldb, true);
   }
   
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());

//    registry.registerLoader(new dc1Response::IrfLoader());
   registry.registerLoader(new dc1aResponse::IrfLoader());
//    registry.registerLoader(new dc2Response::IrfLoader());
//    registry.registerLoader(new g25Response::IrfLoader());
   registry.registerLoader(new latResponse::IrfLoader());
   registry.registerLoader(new testResponse::IrfLoader());
//    registry.registerLoader(new egretResponse::IrfLoader());

   std::vector<std::string> names(registry.irfNames());
}

} // namespace irfLoader
