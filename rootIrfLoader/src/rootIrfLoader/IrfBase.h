/**
 * @file IrfBase.h
 * @brief Base classes for irfInterface wrapper classes.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef rootIrfLoader_IrfBase_h
#define rootIrfLoader_IrfBase_h

#include <string>

namespace irfInterface {
   class Irfs;
}

namespace rootIrfLoader {

/**
 * @class IrfBase
 *
 * @brief Base class for IRF components.  This class is responsible
 * for loading the IRFs and uses Template Method design pattern (GOF)
 * to delegate pointer acquisition by subclasses in setIrfs(...) via
 * the getRef(...) pure virtual method.
 *
 * @author J. Chiang
 *
 */

class IrfBase {

public:

   IrfBase(const std::string & irfs);

   virtual ~IrfBase() {}

   void setIrfs(const std::string & irfsName);

   const std::string & currentIrfs() const {
      return m_currentIrfs;
   }

protected:

   std::string m_currentIrfs;

   virtual void getRef(irfInterface::Irfs *) = 0;

private:

   static bool s_irfsLoaded;

};

} // namespace rootIrfLoader

#endif // rootIrfLoader_IrfBase_h

