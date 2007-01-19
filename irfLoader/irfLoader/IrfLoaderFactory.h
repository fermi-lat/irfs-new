/**
 * @file IrfLoaderFactory.h
 * @brief Factory class to ensure IrfLoader objects register
 * their irfInterface implementations with the Singleton IrfRegistry.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfLoader_IrfLoaderFactory_h
#define irfLoader_IrfLoaderFactory_h

#include "irfLoader/IrfRegistry.h"

namespace irfLoader {

/**
 * @class IrfLoaderFactory
 * @brief Factory class to ensure IrfLoader objects register
 * their irfInterface implementations with the Singleton Loader
 * registry.
 */

template <typename T>
class IrfLoaderFactory {

public:

   IrfLoaderFactory() {
      IrfRegistry::instance()->registerLoader(new T());
   }

};

} // namespace irfLoader

#endif // irfLoader_IrfLoaderFactory_h

