/**
 * @file Psf.h
 * @brief Concrete IPsf sub-class for testing purposes.
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef irfInterface_Psf_h
#define irfInterface_Psf_h

#include "astro/SkyDir.h"

#include "irfInterface/IPsf.h"

class Psf : public irfInterface::IPsf {

public:

   Psf(double maxSep=10);

   virtual ~Psf() {}

   virtual double value(const astro::SkyDir & appDir, 
                        double energy, 
                        const astro::SkyDir & srcDir,
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time) const;

   virtual double value(double sep, double energy, double theta, 
                        double phi, double time=0) const;

   virtual IPsf * clone() {return new Psf(*this);}

private:
   
   double m_maxSep;
   double m_value;
};

#endif // irfInterface_Psf_h
