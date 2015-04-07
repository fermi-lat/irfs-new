/**
 * @file Irfs.h
 * @brief Response function container.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_Irfs_h
#define latResponse_Irfs_h

#include <string>

#include "irfInterface/Irfs.h"

namespace latResponse {

/**
 * @class Irfs
 *
 * @brief Subclass of irfInterface::Irfs that uses lazy evaluation
 * when accessing the IRF files in CALDB in order to minimize
 * concurrency issues and speed up execution time.
 *
 */

class Irfs : public irfInterface::Irfs {

public:

   Irfs(const std::string & irf_name, const std::string & event_type);

   Irfs(const Irfs & rhs);

   virtual ~Irfs() {}
   
   virtual irfInterface::IAeff * aeff();

   virtual irfInterface::IPsf * psf();

   virtual irfInterface::IEdisp * edisp();

   virtual const irfInterface::IEfficiencyFactor * efficiencyFactor() const;

   virtual irfInterface::Irfs * clone() {
      return new Irfs(*this);
   }

private:

   std::string m_irfName;

   std::string m_eventType;

};

} // namespace latResponse

#endif // latResponse_Irfs_h
