#include "irfInterface/IrfsFactory.h"
#include "dc2Response/loadIrfs.h"

int main() {
   dc2Response::loadIrfs();
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * myIrfs = myFactory->create("DC2::Front");
   delete myIrfs;
   myIrfs = myFactory->create("DC2::Back");
   delete myIrfs;
}

