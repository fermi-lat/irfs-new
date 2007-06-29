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
   std::string irfName("standard");

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

      bool useCaldb;
      loader->addIrfEval("Pass4_v2/front", 
                         new FitsEval("Pass4_v2", "front", 
                                      "PASS4_v2", "2007-06-24",
                                      useCaldb=true));
      loader->addIrfEval("Pass4_v2/back", 
                         new FitsEval("Pass4_v2", "back", 
                                      "PASS4_v2", "2007-06-24",
                                      useCaldb=true));
   }

// The factory to add our IRFs to
   irfInterface::IrfsFactory* myFactory(irfInterface::IrfsFactory::instance());

   std::string eventClass(irfName + "/front");
   myFactory->addIrfs(eventClass, loader->irfs(eventClass, 0), verbose);

   eventClass = irfName + "/back";
   myFactory->addIrfs(eventClass, loader->irfs(eventClass, 1), verbose);

// Pass4_v2 irfs:
   eventClass = "Pass4_v2/front";
   myFactory->addIrfs(eventClass, loader->irfs(eventClass, 0), verbose);

   eventClass = "Pass4_v2/back";
   myFactory->addIrfs(eventClass, loader->irfs(eventClass, 1), verbose);
   
   delete loader;
   
   return irfName;
}

} // namespace handoff_response
