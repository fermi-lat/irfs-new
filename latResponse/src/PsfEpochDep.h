/**
 * @file PsfEpochDep.h
 * @brief PsfEpochDep class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_PsfEpochDep_h
#define latResponse_PsfEpochDep_h

#include <string>
#include <vector>

#include "irfInterface/IPsf.h"

#include "EpochDep.h"

namespace latResponse {

class PsfEpochDep : public irfInterface::IPsf, EpochDep {
      
public:

   PsfEpochDep();

   PsfEpochDep(const PsfEpochDep & other);

   PsfEpochDep & operator=(const PsfEpochDep & rhs);

   virtual ~PsfEpochDep();

   virtual double value(const astro::SkyDir & appDir, 
                        double energy, 
                        const astro::SkyDir & srcDir, 
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis, 
                        double time=0) const;

   virtual double value(double separation, double energy, double theta,
                        double phi, double time=0) const;

   typedef std::vector<irfInterface::AcceptanceCone *> AcceptanceConeVector_t;

   virtual double 
   angularIntegral(double energy,
                   const astro::SkyDir & srcDir,
                   double theta, 
                   double phi, 
                   const AcceptanceConeVector_t & acceptanceCones, 
                   double time=0);
                   

   virtual double angularIntegral(double energy, double theta, double phi,
                                  double radius, double time=0) const;

   virtual PsfEpochDep * clone() {
      return new PsfEpochDep(*this);
   }

   void addPsf(const irfInterface::IPsf & psf, double epoch_start);

private:

   std::vector<irfInterface::IPsf *> m_psfs;

};

} // namespace latResponse

#endif // latResponse_PsfEpochDep_h
