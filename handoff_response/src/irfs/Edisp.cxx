/**
* @file Edisp.cxx
* @brief Implementation class Edisp.
* @author J. Chiang
*
* $Header$
*/

#include <cmath>

#include "astro/SkyDir.h"

#include "handoff_response/IrfEval.h"

#include "Edisp.h"

using namespace handoff_response;

Edisp::Edisp(handoff_response::IrfEval * eval)
: m_eval(eval)
{}

double Edisp::value(double appEnergy,
                    double energy, 
                    const astro::SkyDir &srcDir,
                    const astro::SkyDir &scZAxis,
                    const astro::SkyDir &, double) const {
    // Inclination wrt spacecraft z-axis in degrees.
    double theta = srcDir.difference(scZAxis)*180./M_PI;

    // The azimuthal angle is not used by the IrfBase irfs.
    static double phi(0);
    return value(appEnergy, energy, theta, phi);
}

double Edisp::value(double appEnergy, double energy,
                    double theta, double phi, double) const {
  return m_eval->dispersion(appEnergy, energy, theta, phi);
//    double sigma(energy/10.);
//    double de(appEnergy - energy);
//    return std::exp(-de*de/2./sigma/sigma)/std::sqrt(2.*M_PI)/sigma;
}
