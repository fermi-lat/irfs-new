/**
 * @file Registrar.cxx
 * @brief Manage irfInterface::IrfLoader instances for each set of IRFs
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "irfInterface/IrfRegistry.h"

#include "dc1Response/IrfLoader.h"
#include "dc1aResponse/IrfLoader.h"
#include "dc2Response/IrfLoader.h"
#include "g25Response/IrfLoader.h"
#include "handoff_response/MyIrfLoader.h"
#include "testResponse/IrfLoader.h"

#include "Registrar.h"

namespace irfLoader {

Registrar::Registrar() {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());

   registry.registerLoader(new dc1Response::IrfLoader());
   registry.registerLoader(new dc1aResponse::IrfLoader());
   registry.registerLoader(new dc2Response::IrfLoader());
   registry.registerLoader(new g25Response::IrfLoader());
   registry.registerLoader(new handoff_response::MyIrfLoader());
   registry.registerLoader(new testResponse::IrfLoader());

//    typedef std::map<std::string, std::vector<std::string> > RespIdMap_t;
//    const RespIdMap_t & respIds(registry.respIds());
//    for (RespIdMap_t::const_iterator it(respIds.begin());
//         it != respIds.end(); ++it) {
//       std::cout << it->first << "\n";
//       for (size_t i(0); i < it->second.size(); i++) {
//          std::cout << it->second.at(i) << std::endl;
//       }
//    }
}

} // namespace irfLoader
