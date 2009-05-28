/**
 * @file loadIrfs.cxx
 * @brief Function to load the test response functions into the
 * IrfsFactory instance.
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>
#include <string>
#include <vector>

#include "irfInterface/IrfsFactory.h"

#include "Aeff.h"
#include "Psf.h"
#include "Edisp.h"

namespace dc1aResponse {

void loadIrfs() {
   irfInterface::IAeff * aeff;
   irfInterface::IPsf * psf;
   irfInterface::IEdisp * edisp;

   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();

   try {
      aeff = new Aeff(5.5e3, 120., 2.);

//       double aeff_front[] = {-5507.14, -8975.31, -0.674, 3727.2,
//                              211.18, 193.43, -0.675, 156.5, 2.97};
//       std::vector<double> aeffFront(aeff_front, aeff_front + 9);
//       aeff = new Aeff(aeffFront);

      double front_params[] = {3.392, 1.630, 0.786, 4.93e4, 2., 2.117, 0.695};
      std::vector<double> frontParams(front_params, front_params + 7);
      psf = new Psf(frontParams);
      edisp = new Edisp();
      myFactory->addIrfs("DC1A::Front",
                         new irfInterface::Irfs(aeff, psf, edisp, 0));
      
      aeff = new Aeff(4.3e3, 120., 2.);

//       double aeff_back[] = {-4103.6, -5216.1, -0.663, 2894.3,
//                              132.6, 215.2, -0.536, 127.4, 2.01};
//       std::vector<double> aeffBack(aeff_back, aeff_back + 9);
//       aeff = new Aeff(aeffBack);

      double back_params[] = {4.884, 2.899, 0.798, 3.52e4, 2., 2.900, 0.688};
      std::vector<double> backParams(back_params, back_params + 7);
      psf = new Psf(backParams);
      edisp = new Edisp();
      myFactory->addIrfs("DC1A::Back",
                         new irfInterface::Irfs(aeff, psf, edisp, 1));
   } catch (std::exception &eObj) {
      std::cout << "dc1aResponse::loadIrfs:\n"
                << eObj.what() << std::endl;
   } catch (...) {
      std::cout << "dc1aResponse::loadIrfs:\n"
                << "unknown exception" << std::endl;
   }
}

} // namespace dc1aResponse
