/**
 * @file Irfs.h
 * @brief Response function container.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_Irfs_h
#define irfInterface_Irfs_h

#include "irfInterface/IAeff.h"
#include "irfInterface/IPsf.h"
#include "irfInterface/IEdisp.h"
#include "irfInterface/IEfficiencyFactor.h"

namespace irfInterface {

/**
 * @class Irfs
 *
 * @brief Container of instrument response functions that are defined
 * using the irfInterface abstract interface.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Irfs {

public:

   Irfs() : m_aeff(0), m_psf(0), m_edisp(0), m_efficiencyFactor(0),
            m_irfID(0) {}

   Irfs(IAeff *aeff, IPsf *psf, IEdisp *edisp, int irfID) 
      : m_aeff(aeff), m_psf(psf), m_edisp(edisp), 
        m_efficiencyFactor(0), m_irfID(irfID) {}

   ~Irfs() {
      delete m_aeff;
      delete m_psf;
      delete m_edisp;
      delete m_efficiencyFactor;
   }

   Irfs(const Irfs &rhs) {
      m_psf = rhs.m_psf->clone();
      m_aeff = rhs.m_aeff->clone();
      m_edisp = rhs.m_edisp->clone();
      if (rhs.m_efficiencyFactor != 0) {
         m_efficiencyFactor = rhs.m_efficiencyFactor->clone();
      } else {
         m_efficiencyFactor = 0;
      }
      m_irfID = rhs.m_irfID;
   }

   IAeff *aeff() {return m_aeff;}
   IPsf *psf() {return m_psf;}
   IEdisp *edisp() {return m_edisp;}

   const IEfficiencyFactor & efficiencyFactor() const {
      return *m_efficiencyFactor;
   }

   void setEfficiencyFactor(const IEfficiencyFactor & eff) {
      m_efficiencyFactor = eff.clone();
   }

   Irfs * clone() {
      return new Irfs(*this);
   }

   /// Return the ID number of the IRFs being used.
   int irfID() const {return m_irfID;}

private:

   IAeff *m_aeff;
   IPsf *m_psf;
   IEdisp *m_edisp;

   IEfficiencyFactor * m_efficiencyFactor;
   
   int m_irfID;

};

} // namespace irfInterface

#endif // irfInterface_Irfs_h
