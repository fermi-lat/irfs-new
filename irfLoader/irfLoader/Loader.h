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

#include <string>
#include <vector>

namespace irfLoader {

/**
 * @class Loader
 * @brief This class has Singleton-like behavior, but since it
 * comprises only static functions, the Singleton instance is never
 * explicitly accessed.
 *
 * $Header$
 */

class Loader {

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

protected:

   Loader();

   ~Loader() {}

   static void create_instance() {
      if (s_instance == 0) {
         s_instance = new Loader();
      }
   }

private:

   static Loader * s_instance;

   static std::vector<std::string> s_irfsNames;
};

} // namespace irfLoader

#endif // irfLoader_Loader_h
