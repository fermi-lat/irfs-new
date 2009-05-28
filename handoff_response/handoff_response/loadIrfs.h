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
    void loadIrfs(const std::string& filename="", bool verbose=true);
}

#endif // handoff_loadIrfs_h
