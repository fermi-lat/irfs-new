/**
 * @file loadIrfs.cxx
 * @brief Function to load the handoff response functions into the
 * IrfsFactory instance.
 *
 * $Header$
 */

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "st_facilities/Env.h"

#include "irfInterface/IrfsFactory.h"

#include "handoff_response/IrfEval.h"
#include "handoff_response/loadIrfs.h"

#include "irfs/FitsEval.h"
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

std::string load_irfs(const std::string & rootfile, bool verbose) {
// The factory to add our IRFs to
   irfInterface::IrfsFactory* myFactory(irfInterface::IrfsFactory::instance());

   std::string irfName("standard");

// // Check if irfs have been loaded already (this breaks the custom irfs)
//    irfInterface::Irfs * my_irfs(0);
//    try {
//       my_irfs = myFactory->create("standard/front");
//    } catch (std::invalid_argument &) {
//       my_irfs = 0;
//    }
//    if (my_irfs != 0) { // HANDOFF irfs are already in the factory
//       return irfName;
//    }

/// @todo get rid of ROOT and get rid of this annoying IrfLoader class

   IrfLoader * loader(0);
   if (rootfile != "") {
      loader = new IrfLoader(rootfile);
   } else {  // use FITS file
      try {
         irfName = ::getEnv("HANDOFF_IRF_NAME");
      } catch (std::runtime_error & eObj) {
         irfName = "standard";
      }
      loader = new IrfLoader(irfName);

      //bool useCaldb;
      /*
      loader->addIrfEval("P6_v1_transient/front", 
                         new FitsEval("Pass6_v1_Transient", "front", 
                                      "PASS6_v1_TRANSIENT", "2008-04-10",
                                      useCaldb=true));
      loader->addIrfEval("P6_v1_transient/back", 
                         new FitsEval("Pass6_v1_Transient", "back", 
                                      "PASS6_v1_TRANSIENT", "2008-04-10",
                                      useCaldb=true));
      loader->addIrfEval("P6_v1_source/front", 
                         new FitsEval("Pass6_v1", "front", 
                                      "PASS6_v1", "2008-08-20",
                                      useCaldb=true));
      loader->addIrfEval("P6_v1_source/back", 
                         new FitsEval("Pass6_v1", "back", 
                                      "PASS6_v1", "2008-08-20",
                                      useCaldb=true));
      loader->addIrfEval("P6_v1_diffuse/front", 
                         new FitsEval("Pass6_v1_Diffuse", "front", 
                                      "PASS6_v1_DIFFUSE", "2008-04-10",
                                      useCaldb=true));
      loader->addIrfEval("P6_v1_diffuse/back", 
                         new FitsEval("Pass6_v1_Diffuse", "back", 
                                      "PASS6_v1_DIFFUSE", "2008-04-10",
                                      useCaldb=true));
      */
   }
   
   std::string eventClass(irfName + "/front");
   myFactory->addIrfs(eventClass, loader->irfs(eventClass, 0), verbose);
   
   eventClass = irfName + "/back";
   myFactory->addIrfs(eventClass, loader->irfs(eventClass, 1), verbose);
   
   delete loader;
   
   return irfName;
}
  
} // namespace handoff_response
