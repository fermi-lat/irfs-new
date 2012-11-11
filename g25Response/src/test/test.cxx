/**
 * @file test.cxx
 * @brief Code to exercise g25Response functions
 * 
 * $Header$
 */

#include <cstdlib>
#include <iostream>
#include "st_facilities/Environment.h"
#include "irfInterface/IrfsFactory.h"
#include "g25Response/loadIrfs.h"

int main() {
   st_facilities::Environment::instance();
   if (!std::getenv("CALDB")) {
      std::cout << "CALDB not set, exiting." << std::endl;
      std::exit(0);
   }
   g25Response::load_irfs();
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * myIrfs = myFactory->create("Glast25::Front");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Back");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Combined");
   delete myIrfs;

// Try loading everything again.
   g25Response::load_irfs();
   myIrfs = myFactory->create("Glast25::Front");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Back");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Combined");
   delete myIrfs;

// Investigate the maximum aeff values
   std::cout << "maximum effective areas: " << std::endl;
   g25Response::load_irfs();
   myIrfs = myFactory->create("Glast25::Front");
   std::cout << "G25::Front: " << myIrfs->aeff()->upperLimit() << std::endl;
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Back");
   std::cout << "G25::Back: " << myIrfs->aeff()->upperLimit() << std::endl;
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Combined");
   std::cout << "G25::Combined: " << myIrfs->aeff()->upperLimit() << std::endl;
   delete myIrfs;
}
