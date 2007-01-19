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

   CPPUNIT_TEST(initialization);
   CPPUNIT_TEST(load_single_irfs);
   CPPUNIT_TEST(use_IrfsFactory);
   CPPUNIT_TEST_EXCEPTION(access_missing_irfs, std::runtime_error);

   CPPUNIT_TEST(test_IrfRegistry);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void initialization();
   void load_single_irfs();
   void use_IrfsFactory();
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

void irfLoaderTests::initialization() {
   CPPUNIT_ASSERT(Loader::irfsNames().size() == 4);
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
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("DC2::FrontA")) == names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("DC2::BackA")) == names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("DC2::FrontB")) == names.end());
   CPPUNIT_ASSERT(std::find(names.begin(), names.end(), 
                            std::string("DC2::BackB")) == names.end());
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
   my_irfs = myFactory->create("Glast25::Back");
   delete my_irfs;
   my_irfs = myFactory->create("DC2::FrontA");
   delete my_irfs;
   my_irfs = myFactory->create("DC2::BackA");
   delete my_irfs;
   my_irfs = myFactory->create("DC2::FrontB");
   delete my_irfs;
   my_irfs = myFactory->create("DC2::BackB");
   delete my_irfs;
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
