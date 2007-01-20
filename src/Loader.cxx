/**
 * @file Loader.cxx
 * @brief Create the various IrfLoaderFactory instances.
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfs/Loader.h"

#include "irfLoader/IrfLoaderFactory.h"

#include "dc1Response/IrfLoader.h"
#include "dc1aResponse/IrfLoader.h"
#include "dc2Response/IrfLoader.h"
#include "g25Response/IrfLoader.h"
#include "handoff_response/MyIrfLoader.h"
#include "testResponse/IrfLoader.h"

namespace irfs {

Loader::Loader() {
   irfLoader::IrfLoaderFactory<dc1Response::IrfLoader>();
   irfLoader::IrfLoaderFactory<dc1aResponse::IrfLoader>();
   irfLoader::IrfLoaderFactory<dc2Response::IrfLoader>();
   irfLoader::IrfLoaderFactory<g25Response::IrfLoader>();
   irfLoader::IrfLoaderFactory<handoff_response::MyIrfLoader>();
   irfLoader::IrfLoaderFactory<testResponse::IrfLoader>();
}

} // namespace irfs
