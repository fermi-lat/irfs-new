/**
 * @file IrfBase.h
 * @brief loader for irfs
 *
 * $Header$
 */
#ifndef handoff_loadIrfs_h
#define handoff_loadIrfs_h

#include <string>

namespace handoff_response {
   std::string load_irfs(const std::string& filename="", bool verbose=false);
}

#endif // handoff_loadIrfs_h
