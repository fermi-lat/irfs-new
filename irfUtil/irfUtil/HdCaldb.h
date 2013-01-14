/**
 * @file HdCaldb.h
 * @brief Interface declaration to wrapper class for HEASARC CALDB routines.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfUtil_HdCaldb_h
#define irfUtil_HdCaldb_h

#include <string>
#include <utility>
#include <vector>

namespace irfUtil {

/**
 * @class HdCaldb
 * @brief Provides an OO wrapper interface to the HEASARC routines that
 * find calibration files in CALDB.
 *
 * @author J. Chiang
 *
 */

class HdCaldb {

public:

   HdCaldb(const std::string & telescope = "GLAST",
           const std::string & instrument = "LAT");

   ~HdCaldb();

   std::pair<std::string, int> 
   operator()(const std::string & detName,  
              const std::string & respName,
              const std::string & expression, 
              const std::string & filter = "NONE",
              const std::string & date = "2003-01-01", 
              const std::string & time = "00:00:00");


   /// Find all IRF files and extensions regardless of validity time
   /// @param files Found filenames.
   /// @param extnums Corresponding HDU numbers.
   /// @param detName "FRONT" or "BACK" for the LAT
   /// @param respName "EFF_AREA", "RPSF", or "EDISP"
   /// @param irfName IRF name, e.g., "P7_SOURCE_V6"
   void getFiles(std::vector<std::string> & files,
                 std::vector<int> & extnums,
                 const std::string & detName,
                 const std::string & respName,
                 const std::string & irfName);

private:

   std::string m_telescope;
   std::string m_instrument;

   static const int s_maxret = 100;
   int m_filenamesize;

   char * m_filenames[s_maxret];
   char * m_online[s_maxret];
   long m_extnums[s_maxret];

};

} // namespace irfUtil

#endif // irfUtil_HdCaldb_h
