/**
 * @file SkyDir.cxx
 * @brief Wrapper class for astro::SkyDir that does not expose
 * CLHEP-dependent parts of the interface.
 * @author J. Chiang
 *
 * $Header$
 */

#include "astro/SkyDir.h"

#include "SkyDir.h"

namespace rootIrfLoader {

SkyDir::SkyDir(double param1, double param2, CoordSystem inputType) 
   : m_dir(0) {
   m_dir = new astro::SkyDir(param1, param2,
                             static_cast<astro::SkyDir::CoordSystem>(inputType));
}

SkyDir::~SkyDir() {
   delete m_dir;
}

const astro::SkyDir & SkyDir::astroDir() const {
   return *m_dir;
}

double SkyDir::l() const {
   return m_dir->l();
}

double SkyDir::b() const {
   return m_dir->b();
}

double SkyDir::ra() const {
   return m_dir->ra();
}

double SkyDir::dec() const {
   return m_dir->dec();
}

double SkyDir::difference(const SkyDir & other) const {
   return m_dir->difference(other.astroDir());
}

} // namespace rootIrfLoader
