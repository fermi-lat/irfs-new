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

   Psf() {}

   virtual ~Psf() {}

   virtual double value(const astro::SkyDir &, 
                        double, 
                        const astro::SkyDir &,
                        const astro::SkyDir &,
                        const astro::SkyDir &) const {return 0;}

   virtual double value(double, double, double, 
                        double) const {return 0;}

   virtual double angularIntegral(double,
                                  const astro::SkyDir &,
                                  const astro::SkyDir &,
                                  const astro::SkyDir &,
                                  const std::vector<irfInterface::AcceptanceCone *> 
                                  &) {return 0;}

   virtual double angularIntegral(double,
                                  const astro::SkyDir &,
                                  double, double,
                                  const std::vector<irfInterface::AcceptanceCone *> 
                                  &) {return 0;}
   
   virtual double angularIntegral(double, double, double,
                                  double) const {return 0;}

   virtual astro::SkyDir appDir(double,
                                const astro::SkyDir &,
                                const astro::SkyDir &,
                                const astro::SkyDir &) const {
      return astro::SkyDir(0, 0);
   }

   virtual IPsf * clone() {return new Psf(*this);}
};

#endif // irfInterface_Psf_h
