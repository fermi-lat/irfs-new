#include <iostream>

#include "irfInterface/IrfsFactory.h"
#include "dc1Response/loadIrfs.h"

int main() {
   dc1Response::loadIrfs();
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * myIrfs = myFactory->create("DC1::Front");
   delete myIrfs;
   myIrfs = myFactory->create("DC1::Back");
   delete myIrfs;

// try loading again
   dc1Response::loadIrfs();
   myIrfs = myFactory->create("DC1::Front");
   delete myIrfs;
   myIrfs = myFactory->create("DC1::Back");
   delete myIrfs;

// access upper limit on aeff
   std::cout << "Maximum effective areas:\n";
   dc1Response::loadIrfs();
   myIrfs = myFactory->create("DC1::Front");
   std::cout << "Front: " << myIrfs->aeff()->upperLimit() << std::endl;
   delete myIrfs;
   myIrfs = myFactory->create("DC1::Back");
   std::cout << "Back: " << myIrfs->aeff()->upperLimit() << std::endl;
   delete myIrfs;
}

