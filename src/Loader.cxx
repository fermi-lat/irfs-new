/**
 * @file Loader.cxx
 * @brief Create the various IrfLoaderFactory instances.
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfs/Loader.h"

#include "irfLoader/IrfLoaderFactory.h"

#include "handoff_response/MyIrfLoader.h"
#include "testResponse/IrfLoader.h"

namespace irfs {

Loader::Loader() {
   irfLoader::IrfLoaderFactory<handoff_response::MyIrfLoader>();
   irfLoader::IrfLoaderFactory<testResponse::IrfLoader>();
}

} // namespace irfs
