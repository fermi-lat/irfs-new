/** 
* @file Psf.cxx
* @brief Implementation for the  point-spread function class.
* @author 
* 
* $Header$
*/

#include <cmath>

#include <stdexcept>
#include <sstream>

#include "irfInterface/AcceptanceCone.h"

#include "Psf.h"

using namespace handoff_response;

Psf::Psf() : m_eval(0) {}

Psf::Psf(handoff_response::IrfEval* eval) : m_eval(eval) {}

Psf::~Psf() {}

double Psf::value(const astro::SkyDir & appDir, 
                  double energy, 
                  const astro::SkyDir & srcDir, 
                  const astro::SkyDir & scZAxis,
                  const astro::SkyDir &, double) const {
    // Angle between photon and source directions in radians.
    double separation = appDir.difference(srcDir);

    // Inclination wrt spacecraft z-axis in radians
    double inc = srcDir.difference(scZAxis);

    return value(separation*180./M_PI, energy, inc*180./M_PI, 0.);
}

double Psf::value(double separation, double energy, double theta,
                  double phi, double) const {
    if (theta < 0) {
        std::ostringstream message;
        message << "handoff::Psf::value(...):\n"
            << "theta cannot be less than zero. "
            << "Value passed: " << theta;
        throw std::invalid_argument(message.str());
    }
    // (back to radians)
    return m_eval->psf(separation*M_PI/180., energy, theta, phi);
}

double Psf::angularIntegral(double ,
                            const astro::SkyDir & ,
                            const astro::SkyDir & ,
                            const astro::SkyDir &,
                            const std::vector<irfInterface::AcceptanceCone *> 
                            & , double) {
    return 0;
}

double Psf::angularIntegral(double , 
                            const astro::SkyDir & ,
                            double , 
                            double , 
                            const std::vector<irfInterface::AcceptanceCone *> 
                            & , double) { 
    return 0;
}
