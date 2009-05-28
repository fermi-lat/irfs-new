#include "irfInterface/IrfsFactory.h"
#include "dc2Response/loadIrfs.h"

int main() {
   dc2Response::loadIrfs();
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * myIrfs = myFactory->create("DC2::FrontA");
   delete myIrfs;
   myIrfs = myFactory->create("DC2::BackA");
   delete myIrfs;
   myIrfs = myFactory->create("DC2::FrontB");
   delete myIrfs;
   myIrfs = myFactory->create("DC2::BackB");
   delete myIrfs;
}
