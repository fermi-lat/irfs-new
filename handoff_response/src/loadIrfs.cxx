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
   std::string getEnv(const std::string & envVarName) {
      char * envvar(::getenv(envVarName.c_str()));
      if (envvar == 0) {
         throw std::runtime_error("Please set the " + envVarName 
                                  + " environment variable.");
      }
      return envvar;
   }
} // anonymous namespace

namespace handoff_response {

std::string load_irfs(const std::string & name, bool verbose) {
   std::string filename(name);

   std::string irfName;
   try {
      irfName = ::getEnv("HANDOFF_IRF_NAME");
   } catch (std::runtime_error & eObj) {
      irfName = "standard";
   }
   
   if (::getenv("HANDOFF_IRF_DIR")) { // use FITS file
      filename = irfName;
   } else { // use default ROOT file
      std::string path(::getEnv("HANDOFF_RESPONSEROOT"));
      filename = path + "/data/parameters.root";
      irfName = "standard";
   }
   IrfLoader loader(filename);
   
// The factory to add our IRFs to
   irfInterface::IrfsFactory* myFactory(irfInterface::IrfsFactory::instance());
   
// Event classes within each set of IRFs have to be unique within that
// set of IRFs, and the event class is given by the
// irfInterface::Irfs::irfID() function and set in the Irfs
// constructor: Irfs(IAeff *aeff, IPsf *psf, IEdisp *edisp, int
// irfID), Front vs Back constitute two separate classes, so the
// argument to IrfLoader::irfs() must be different for each.
    int id(0);
    for (IrfLoader::const_iterator it(loader.begin()); 
         it != loader.end(); ++it, id++) {
       const std::string & eventclass = it->first;
       if (verbose) {
          std::cout << "Loading irfs for event class "
                    << eventclass << std::endl;
       }
       myFactory->addIrfs(eventclass, loader.irfs(eventclass, id), verbose);
    }

    return irfName;
}

} // namespace handoff_response
