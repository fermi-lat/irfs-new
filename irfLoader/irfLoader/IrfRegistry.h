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

   static IrfRegistry * instance();

   void registerLoader(IrfLoader * irfLoader);

   void registerEventClasses(const std::string & name,
                             const std::vector<std::string> & classList);

   void registerEventClasses(const std::string & name,
                             const char ** class_list); 

   void loadIrfs(const std::string & irfsName);

   std::vector<std::string> irfNames() const;

   const std::vector<std::string> & 
   operator[](const std::string & respName) const;
   
   const std::map<std::string, std::vector<std::string> > & respIds() const {
      return m_respIds;
   }
   
protected:

   IrfRegistry() {}

private:

   static IrfRegistry * s_instance;

   std::map<std::string, IrfLoader *> m_irfLoaders;

   std::map<std::string, std::vector<std::string> > m_respIds; 

};


} // namespace irfLoader

#endif // irfLoader_IrfRegistry_h
