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
      double totalAeff(0);
      std::cout << "FrontA: " << myIrfs->aeff()->upperLimit() << std::endl;
      totalAeff += myIrfs->aeff()->upperLimit();
      delete myIrfs;
      myIrfs = myFactory->create("DC2::BackA");
      std::cout << "BackA: " << myIrfs->aeff()->upperLimit() << std::endl;
      totalAeff += myIrfs->aeff()->upperLimit();
      delete myIrfs;
      myIrfs = myFactory->create("DC2::FrontB");
      std::cout << "FrontB: " << myIrfs->aeff()->upperLimit() << std::endl;
      totalAeff += myIrfs->aeff()->upperLimit();
      delete myIrfs;
      myIrfs = myFactory->create("DC2::BackB");
      std::cout << "BackB: " << myIrfs->aeff()->upperLimit() << std::endl;
      totalAeff += myIrfs->aeff()->upperLimit();
      delete myIrfs;
      std::cout << "Total: " << totalAeff << std::endl;
      std::exit(0);
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   } catch (std::string & what) {
      std::cout << what << std::endl;
   }
   std::exit(1);
}
