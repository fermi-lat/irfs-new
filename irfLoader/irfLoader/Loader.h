/**
 * @file Loader.h
 * @brief This class provides a set of static functions for loading 
 * the various irf implementations into the IrfsFactory instance.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfLoader_Loader_h
#define irfLoader_Loader_h

#include <map>
#include <string>
#include <vector>

#include "st_facilities/libStApiExports.h"

namespace irfLoader {

/**
 * @class Loader
 *
 */

#ifndef SWIG
class SCIENCETOOLS_API Loader {
#else
class Loader {
#endif

public:

   /// This method loads all of the available irfs.
   static void go();

   /// This method loads only those requested.
   /// @param irfsNames A vector of irfs names, e.g., "DC1", "GLAST25"
   static void go(const std::vector<std::string> & irfsNames);

   /// Load a single set of irfs by name.
   /// @param irfsName The name of the desired irfs
   static void go(const std::string & irfsName);

   /// Access to the names of the available irfs.
   static const std::vector<std::string> & irfsNames() {
      return s_irfsNames;
   }

   /// @return A reference to a map for common combinations of 
   ///         response functions for use with a PIL entry.
   static const std::map<std::string, std::vector<std::string> > & respIds() {
      return s_respIds;
   }

   static void resetIrfs();

protected:

   Loader() {}

   ~Loader() {}

private:

   static std::vector<std::string> s_irfsNames;

   static std::map<std::string, std::vector<std::string> > s_respIds;

};

#ifndef SWIG
// Opaque wrappers for class static functions since linkage on windows for
// symbols exported from dlls is all fouled up.
void Loader_go();

const std::map<std::string, std::vector<std::string> > & Loader_respIds();
#endif

} // namespace irfLoader

#endif // irfLoader_Loader_h
