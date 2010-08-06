#include <iostream>
#include "irfInterface/IrfsFactory.h"
#include "dc2Response/loadIrfs.h"

int main() {
   try {
      dc2Response::loadIrfs();
      irfInterface::IrfsFactory * myFactory 
         = irfInterface::IrfsFactory::instance();
      irfInterface::Irfs * myIrfs(0);
      myIrfs = myFactory->create("DC2::FrontA");
      delete myIrfs;
      myIrfs = myFactory->create("DC2::BackA");
      delete myIrfs;
      myIrfs = myFactory->create("DC2::FrontB");
      delete myIrfs;
      myIrfs = myFactory->create("DC2::BackB");
      delete myIrfs;
      std::exit(0);
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   } catch (std::string & what) {
      std::cout << what << std::endl;
   }
   std::exit(1);
}
