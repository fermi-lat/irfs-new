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
 */

class Irfs {

public:

   Irfs() : m_aeff(0), m_psf(0), m_edisp(0), m_efficiencyFactor(0),
            m_irfID(0) {}

   Irfs(IAeff * aeff, IPsf * psf, IEdisp * edisp, int irfID) 
      : m_aeff(aeff), m_psf(psf), m_edisp(edisp), 
        m_efficiencyFactor(0), m_irfID(irfID) {}

   Irfs(const Irfs & rhs) {
      if (rhs.m_psf != 0) {
         m_psf = rhs.m_psf->clone();
      } else {
         m_psf = 0;
      }
      if (rhs.m_aeff != 0) {
         m_aeff = rhs.m_aeff->clone();
      } else {
         m_aeff = 0;
      }
      if (rhs.m_edisp != 0) {
         m_edisp = rhs.m_edisp->clone();
      } else {
         m_edisp = 0;
      }
      if (rhs.m_efficiencyFactor != 0) {
         m_efficiencyFactor = rhs.m_efficiencyFactor->clone();
      } else {
         m_efficiencyFactor = 0;
      }
      m_irfID = rhs.m_irfID;
   }

   virtual ~Irfs() {
      delete m_aeff;
      delete m_psf;
      delete m_edisp;
      delete m_efficiencyFactor;
   }

   virtual IAeff * aeff() {
      return m_aeff;
   }

   virtual IPsf * psf() {
      return m_psf;
   }
   
   virtual IEdisp * edisp() {
      return m_edisp;
   }

   virtual const IEfficiencyFactor * efficiencyFactor() const {
      return m_efficiencyFactor;
   }

   void setEfficiencyFactor(const IEfficiencyFactor * eff) {
      m_efficiencyFactor = eff->clone();
   }

   void setAeff(IAeff * aeff) {
      m_aeff = aeff;
   }

   void setPsf(IPsf * psf) {
      m_psf = psf;
   }

   void setEdisp(IEdisp * edisp) {
      m_edisp = edisp;
   }

   virtual Irfs * clone() {
      return new Irfs(*this);
   }

   /// Return the ID number of the IRFs being used.
   int irfID() const {
      return m_irfID;
   }

   void setIrfID(int irfID) {
      m_irfID = irfID;
   }

private:

   IAeff * m_aeff;
   IPsf * m_psf;
   IEdisp * m_edisp;

   IEfficiencyFactor * m_efficiencyFactor;
   
   int m_irfID;

};

} // namespace irfInterface

#endif // irfInterface_Irfs_h
