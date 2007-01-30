/**
 * @file loadIrfs.cxx
 * @brief Function to load the handoff response functions into the
 * IrfsFactory instance.
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "st_facilities/Env.h"

#include "irfInterface/IrfsFactory.h"

#include "handoff_response/IrfEval.h"
#include "handoff_response/loadIrfs.h"

#include "irfs/IrfLoader.h"

namespace {
   bool useFits() {
      char * use_fits = ::getenv("USE_FITS");
      if (use_fits == 0) {
         return false;
      }
      return true;
   }
} // anonymous namespace

namespace handoff_response {

void load_irfs(const std::string & name, bool verbose) {
   std::string filename(name);

   IrfLoader * loader(0);
   
   if (::useFits()) {
// hardwire class "standard" for now
      filename = "standard";
   } else if (filename.empty()) { // assume default
      char * rootPath = ::getenv("HANDOFF_RESPONSEROOT");
      if (rootPath == 0) {
         throw std::runtime_error("HANDOFF_RESPONSEROOT not set");
      }
      std::string path(rootPath);
      filename = path + "/data/parameters.root";
   }
   loader = new IrfLoader(filename);
   
// the factory to add our IRFs to
   irfInterface::IrfsFactory * myFactory =
      irfInterface::IrfsFactory::instance();
   
// event classes within each set of IRFs have to be unique within
// that set of IRFs, and the event class is given by the
// irfInterface::Irfs::irfID() function and set in the Irfs constructor:
// Irfs(IAeff *aeff, IPsf *psf, IEdisp *edisp, int irfID),
// Front vs Back constitute two separate classes, so the argument to
// IrfLoader::irfs() must be different for each. 
    int id(0);
    for (IrfLoader::const_iterator it(loader->begin()); 
         it != loader->end(); ++it, id++) {
       const std::string & eventclass = it->first;
       if (verbose) {
          std::cout << "Loading irfs for event class "
                    << eventclass << std::endl;
       }
       myFactory->addIrfs(eventclass, loader->irfs(eventclass, id), verbose);
    }
    delete loader;
}

} // namespace handoff_response
