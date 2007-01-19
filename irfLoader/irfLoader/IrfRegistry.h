/**
 * @file IrfRegistry.h
 * @brief Singleton instance to manage concrete irfInterface implementations.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef irfLoader_IrfRegistry_h
#define irfLoader_IrfRegistry_h

#include <map>
#include <string>
#include <vector>

namespace irfLoader {

   class IrfLoader;

/**
 * @class IrfRegistry
 * @brief Singleton instance to manage concrete irfInterface
 * implementations.
 */

class IrfRegistry {

public:

   static IrfRegistry * instance() {
      if (s_instance == 0) {
         s_instance = new IrfRegistry();
      }
      return s_instance;
   }

   void registerLoader(IrfLoader * irfLoader);

   void registerEventClasses(const std::string & name,
                             const std::vector<std::string> & classNameList);

   void loadIrfs(const std::string & irfsName);

protected:

   IrfRegistry() {}

private:

   static IrfRegistry * s_instance;

   std::map<std::string, IrfLoader *> m_irfLoaders;

   std::map<std::string, std::vector<std::string> > m_irfIds; 

};


} // namespace irfLoader

#endif // irfLoader_IrfRegistry_h
