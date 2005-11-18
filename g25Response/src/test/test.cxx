#include "irfInterface/IrfsFactory.h"
#include "g25Response/loadIrfs.h"

int main() {
   g25Response::loadIrfs();
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * myIrfs = myFactory->create("Glast25::Front");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Back");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Combined");
   delete myIrfs;

// Try loading everything again.
   g25Response::loadIrfs();
   myIrfs = myFactory->create("Glast25::Front");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Back");
   delete myIrfs;
   myIrfs = myFactory->create("Glast25::Combined");
   delete myIrfs;
}

