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

// The following is a snippet taken from the libApiExports.h file that 
// was authored by Matt Langston.
// The intent is to define for windows those classes we would like 
// to export (or import) from the IrfsFactory dll. 
#if (defined(_WIN32) && defined(_MSC_VER))
# ifdef IRFS_DLL_EXPORTS
#  undef  DLL_EXPORT_IRFS
#  define DLL_EXPORT_IRFS __declspec(dllexport)
# else
#  undef  DLL_EXPORT_IRFS
#  define DLL_EXPORT_IRFS __declspec(dllimport)
# endif
#else
// The gcc compiler (i.e. the Linux/Unix compiler) exports the Universe
// of symbols from a shared library, meaning that we can't control the
// EVT of our shared libraries. We therefore just define the Symbol
// Export Macro to expand to nothing.
# undef  DLL_EXPORT_IRFS
# define DLL_EXPORT_IRFS
#endif

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

class DLL_EXPORT_IRFS IrfsFactory {

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
