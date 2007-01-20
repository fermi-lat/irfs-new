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
#include "irfLoader/IrfLoaderFactory.h"

#include "MyLoader.h"

using irfInterface::IrfsFactory;
using namespace irfLoader;

irfLoader::IrfLoaderFactory<MyLoader> loaderFactory;

class irfLoaderTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(irfLoaderTests);

   CPPUNIT_TEST(load_single_irfs);
   CPPUNIT_TEST_EXCEPTION(access_missing_irfs, std::invalid_argument);

   CPPUNIT_TEST(test_IrfRegistry);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void load_single_irfs();
   void access_missing_irfs();

   void test_IrfRegistry();

private:

};

void irfLoaderTests::setUp() {
// Tabula rasa
   IrfsFactory::delete_instance();
   irfLoader::Loader::resetIrfs();
}

void irfLoaderTests::tearDown() {
}

void irfLoaderTests::load_single_irfs() {
   Loader::go();
   IrfsFactory * myFactory = IrfsFactory::instance();
   std::vector<std::string> names;
   myFactory->getIrfsNames(names);
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("my_classes::FrontA")) != names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("my_classes::BackA")) != names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("my_classes::FrontB")) != names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("my_classes::BackB")) != names.end());
}

void irfLoaderTests::access_missing_irfs() {
   Loader::go("DEV");
}

void irfLoaderTests::test_IrfRegistry() {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());

   registry.loadIrfs("my_classes");

   char * class_names[] = {"FrontA", "BackA", "FrontB", "BackB"};
   const std::vector<std::string> & classes(registry["my_classes"]);
   for (size_t i(0); i < classes.size(); i++) {
      CPPUNIT_ASSERT(classes.at(i) == class_names[i]);
   }
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
