/**
 * @file test.cxx
 * @brief Test program for irfLoader.
 * @author J. Chiang
 *
 * $Header$
 */

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <algorithm>
#include <stdexcept>

#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "irfInterface/IrfsFactory.h"
#include "irfLoader/Loader.h"

using irfInterface::IrfsFactory;
using namespace irfLoader;

class irfLoaderTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(irfLoaderTests);

   CPPUNIT_TEST(initialization);
   CPPUNIT_TEST(load_single_irfs);
   CPPUNIT_TEST(use_IrfsFactory);
   CPPUNIT_TEST_EXCEPTION(access_missing_irfs, std::invalid_argument);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void initialization();
   void load_single_irfs();
   void use_IrfsFactory();
   void access_missing_irfs();

private:

};

void irfLoaderTests::setUp() {
// Tabula rasa
   IrfsFactory::delete_instance();
}

void irfLoaderTests::tearDown() {
}

void irfLoaderTests::initialization() {
   CPPUNIT_ASSERT(Loader::irfsNames().size() == 3);
}

void irfLoaderTests::load_single_irfs() {
   Loader::go("DC1");
   IrfsFactory * myFactory = IrfsFactory::instance();
   std::vector<std::string> names;
   myFactory->getIrfsNames(names);
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("DC1::Front")) != names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("DC1::Back")) != names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("Glast25::Front")) == names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("Glast25::Back")) == names.end());
}

void irfLoaderTests::use_IrfsFactory() {
   Loader::go();
   IrfsFactory * myFactory = IrfsFactory::instance();
   irfInterface::Irfs * my_irfs = myFactory->create("DC1::Front");
   delete my_irfs;
   my_irfs = myFactory->create("DC1::Back");
   delete my_irfs;
   my_irfs = myFactory->create("Glast25::Front");
   delete my_irfs;
   my_irfs = myFactory->create("Glast25::Front");
   delete my_irfs;
}

void irfLoaderTests::access_missing_irfs() {
   Loader::go("DC2");
}
   
int main() {
   CppUnit::TextTestRunner runner;
   
   runner.addTest(irfLoaderTests::suite());
    
   bool result = runner.run();
   if (result) {
      return 0;
   } else {
      return 1;
   }
}
