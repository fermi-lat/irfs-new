/**
 * @file IrfsFactory.h
 * @brief Generate Irf objects using Prototypes.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_IrfsFactory_h
#define irfInterface_IrfsFactory_h

#include <string>
#include <map>

#include <vector>
#include <string>

#include "st_facilities/libStApiExports.h"

#include "irfInterface/Irfs.h"

namespace irfInterface {

/**
 * @class IrfsFactory
 *
 * @brief Factory to supply Irf prototype objects.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class SCIENCETOOLS_API IrfsFactory {

public:

   Irfs * create(const std::string & name) const;

   void addIrfs(const std::string & name, Irfs * irfs, bool verbose=false);

   void getIrfsNames(std::vector<std::string> &names) const;

   static IrfsFactory * instance();

   static void delete_instance();

protected:

   IrfsFactory() {}

   ~IrfsFactory();

private:

   std::map<std::string, Irfs *> m_prototypes;

   static IrfsFactory * s_instance;

};

} // namespace irfInterface

#endif // irfInterface_IrfsFactory_h
