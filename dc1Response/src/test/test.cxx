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
}

