/**
 * @file IrfLoader.h
 * @brief Abstract base class for defining the interface expected by
 * IrfLoaderFactory when instantiating a factory instance by
 * irfInterface concrete implementations.
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef irfLoader_IrfLoader_h
#define irfLoader_IrfLoader_h

#include <string>

namespace irfLoader {

/**
 * @class IrfLoader
 *
 * @brief Abstract base class for defining the interface expected by
 * IrfLoaderFactory when instantiating a factory instance by
 * irfInterface concrete implementations.
 */

class IrfLoader {

public:

   IrfLoader() {}

   virtual void registerEventClasses() const = 0;

   virtual void loadIrfs() const = 0;

   virtual std::string name() const = 0;

};

} // namespace irfLoader

#endif // irfLoader_IrfLoader_h
