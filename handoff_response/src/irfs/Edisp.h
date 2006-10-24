/**
 * @file Edisp.h
 * @brief Edisp class definition.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_Edisp_h
#define handoff_Edisp_h

#include "irfInterface/IEdisp.h"

namespace handoff_response {

   class IrfEval;

/**
 * @class Edisp
 *
 * @brief declare IEdisp subclass for handoff energy dispersion
 *
 *
 */

class Edisp : public irfInterface::IEdisp {

public:

   Edisp(handoff_response::IrfEval* eval);

   virtual ~Edisp() {}

   /// A member function returning the energy dispersion function.
   /// @param appEnergy measured photon energy in MeV.
   /// @param energy True photon energy in MeV.
   /// @param srcDir True photon direction.
   /// @param scZAxis Spacecraft z-axis.
   /// @param scXAxis Spacecraft x-axis.
   /// @param time   MET
   virtual double value(double appEnergy, 
                        double energy,
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis,
                        double time=0) const;

   virtual double value(double appEnergy, double energy,
                        double theta, double phi,
                        double time=0) const;

   virtual Edisp * clone() {return new Edisp(*this);}

private:

   handoff_response::IrfEval* m_eval;

};

}

#endif // handoff_Edisp_h

