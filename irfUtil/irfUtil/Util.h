/**
 * @file Util.h
 * @brief Utility functions used by response function classes and others.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfUtil_Util_h
#define irfUtil_Util_h

#include <string>
#include <vector>

namespace irfUtil {

/**
 * @class Util
 *
 * $Header$
 */

class Util {

public:

   virtual ~Util() {}

   /// A zeroth order bilinear interpolater.
   static double bilinear(const std::vector<double> &xx, double x,
                          const std::vector<double> &yy, double y, 
                          const std::vector<double> &z);

   /// Get a vector of values from the specified extension and column.
   static void getTableVector(const std::string & filename,
                              const std::string & extName,
                              const std::string & columnName,
                              std::vector<double> & branchVector);

   /// Get a vector from a given row of a record.
   static void getRecordVector(const std::string & filename,
                               const std::string & extName,
                               const std::string & columnName,
                               std::vector<double> & tableVector,
                               int recordNum = 0);

   /// Get the extension name of a FITS table HDU by extension number.
   static void getFitsHduName(const std::string & filename, int hdu,
                              std::string & hduName);

   /// Get the column names for a FITS table HDU.
   static void getFitsColNames(const std::string & filename, int hdu,
                               std::vector<std::string> & columnNames);

   static void getCaldbFile(const std::string &detName, 
                            const std::string &respName,
                            const std::string &version,
                            std::string &filename, long &extnum,
                            const std::string & telescope = "GLAST",
                            const std::string & instrument = "LAT");

   static bool expectedException(const std::exception & eObj,
                                 const std::string & targetMessage);

protected:

   Util() {}

};

} // namespace irfUtil

#endif // irfUtil_Util_h
