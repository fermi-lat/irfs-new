#include <iostream>
#include <string>
#include <vector>

#include "irfUtil/HdCaldb.h"
#include "irfUtil/Util.h"

int main() {
   std::string detname("FRONT");
   std::string respname("EFF_AREA");
//   std::string irfName("TESTIRFS");
   std::string irfName("P7REP_SOURCE_V10");
   try {
      irfUtil::HdCaldb hdcaldb;
      std::vector<std::string> files;
      std::vector<int> hdus;
      hdcaldb.getFiles(files, hdus, detname, respname, irfName);
      for (size_t i(0); i < files.size(); i++) {
         std::cout << i << "  "
                   << files[i] << std::endl;
      }
   } catch (std::exception & eObj) {
      std::cout << "Exception caught: " 
                << eObj.what() << std::endl;
   }
   std::cout << std::endl;

/// Exercise the operator() interface
//   irfName = "P7REP_SOURCE_V10";
   std::cout << "Testing HdCaldb::operator():" << std::endl;
   try {
      irfUtil::HdCaldb foo;
      std::pair<std::string, int> calfile;
      calfile = foo(detname, respname, "VERSION.eq." + irfName,
                    "NONE", "2008-08-04", "00:00:00");
      std::cout << calfile.first << "  "
                << calfile.second << std::endl;
   } catch (std::exception & eObj) {
      std::cout << "Exception caught: " 
                << eObj.what() << std::endl;
   }
   std::cout << std::endl;

/// Exercise the getCaldbFile function
   std::cout << "Testing getCaldbFile:" << std::endl;
   try {
      std::string irf_file;
      long hdu;
      irfUtil::Util::getCaldbFile(detname, respname, irfName,
                                  irf_file, hdu, "GLAST", "LAT", "NONE", 
                                  "2009-08-04", "00:00:00");
      std::cout << irf_file << "  " << hdu << std::endl;
   } catch (std::exception & eObj) {
      std::cout << "Exception caught: " 
                << eObj.what() << std::endl;
   }
      
   return 0;
}
