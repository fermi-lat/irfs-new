/**
* @file test.cxx
* @brief Test program for handoff
* @author 
*
* $Header$
*/

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <cmath>

#include <iostream>

#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "irfInterface/IrfsFactory.h"

#include "handoff_response/loadIrfs.h"

class HandoffResponseTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(HandoffResponseTests);

   CPPUNIT_TEST(psf_zero_separation);
   CPPUNIT_TEST(psf_normalization);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void psf_zero_separation();
   void psf_normalization();

private:

   irfInterface::IrfsFactory * m_irfsFactory;
   std::vector<std::string> m_irfNames;

};

void HandoffResponseTests::setUp() {
   m_irfsFactory = irfInterface::IrfsFactory::instance();
   m_irfsFactory->getIrfsNames(m_irfNames);
}

void HandoffResponseTests::tearDown() {
   m_irfNames.clear();
}

void HandoffResponseTests::psf_zero_separation() {
   double energy(1e3);
   double theta(0);
   double phi(0);

   double delta_sep(1e-4);
   double tol(1e-3);
   
   for (std::vector<std::string>::const_iterator name = m_irfNames.begin();
        name != m_irfNames.end(); ++name) {
      irfInterface::Irfs * myIrfs(m_irfsFactory->create(*name));
      const irfInterface::IPsf & psf(*myIrfs->psf());
      for (theta = 0; theta < 70; theta += 5.) {
         double value0(psf.value(0, energy, theta, phi));
         double value1(psf.value(delta_sep, energy, theta, phi));
         if (std::fabs((value0 - value1)/value0) >= tol) {
            std::cout << *name << ": \n";
            std::cout << "theta = " << theta << ": "
                      << "psf value at 0 = " << value0 << "  "
                      << "psf value at 1e-4 = " << value1 << std::endl;
         }
         CPPUNIT_ASSERT(std::fabs((value0 - value1)/value0) < tol);
      }
   }
}

void HandoffResponseTests::psf_normalization() {
   double phi(0);

   double tol(1e-2);

   std::vector<double> psi;
   double psimin(1e-4);
   double psimax(90);
   size_t npsi(1000);
   double dpsi(std::log(psimax/psimin)/(npsi-1));
   psi.push_back(0);
   for (size_t i = 0; i < npsi; i++) {
      psi.push_back(psimin*std::exp(i*dpsi));
   }

   std::vector<double> energies;
   double emin(30);
   double emax(1.7e5);
   size_t nee(10);
   double dee(std::log(emax/emin)/(nee-1));
   for (size_t i = 0; i < nee; i++) {
      energies.push_back(emin*std::exp(i*dee));
   }

   std::vector<double> thetas;
   double thmin(0);
   double thmax(70);
   size_t nth(8);
   double dth((thmax - thmin)/(nth-1));
   for (size_t i = 0; i < nth; i++) {
      thetas.push_back(i*dth + thmin);
   }

   std::cout << "PSF integral values that fail 1% tolerance: \n"
             << "energy  inclination  integral est.  angularIntegral\n";

   bool integralFailures(false);

   for (std::vector<std::string>::const_iterator name = m_irfNames.begin();
        name != m_irfNames.end(); ++name) {
      std::cout << *name << ": \n";
      irfInterface::Irfs * myIrfs(m_irfsFactory->create(*name));
      const irfInterface::IPsf & psf(*myIrfs->psf());
      
      for (std::vector<double>::const_iterator energy = energies.begin();
           energy != energies.end(); ++energy) {
         for (std::vector<double>::const_iterator theta = thetas.begin();
              theta != thetas.end(); ++theta) {
           
            std::vector<double> psf_values;
            for (size_t i = 0; i < psi.size(); i++) {
               psf_values.push_back(psf.value(psi.at(i), *energy, 
                                              *theta, phi));
            }
            
            double integral(0);
            for (size_t i = 0; i < psi.size() - 1; i++) {
               integral += ((psf_values.at(i)*std::sin(psi.at(i)*M_PI/180.) + 
                       psf_values.at(i+1)*std::sin(psi.at(i+1)*M_PI/180.))/2.)
                  *(psi.at(i+1) - psi.at(i))*M_PI/180.;
            }
            integral *= 2.*M_PI;
            double angInt(psf.angularIntegral(*energy, *theta, phi, 70.));
            if (std::fabs(integral - 1.) >= tol) { 
               std::cout << *energy  << "      " 
                         << *theta   << "           "
                         << integral << "        "
                         << angInt << std::endl;
               integralFailures = true;
            }
//             CPPUNIT_ASSERT(std::fabs(integral - 1.) < tol);
//             CPPUNIT_ASSERT(std::fabs(angInt - 1.) < tol);
         }
      }
   }
   CPPUNIT_ASSERT(!integralFailures);
}


int main() {
#ifdef TRAP_FPE
// Add floating point exception traps.
   feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#endif

   handoff_response::loadIrfs();

   CppUnit::TextTestRunner runner;
   runner.addTest(HandoffResponseTests::suite());
   bool result(runner.run());
   if (result) {
      return 0;
   } else {
      return 1;
   }
}
