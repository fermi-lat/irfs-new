/**
 * @file PsfScaling.cxx
 * @brief Energy scaling of angular deviation for fitting Psf.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include "PsfScaling.h"

namespace {
   double sqr(double x) {
      return x*x;
   }
}

namespace dc2Response {

PsfScaling::PsfScaling(const std::vector<double> & pars) 
   : m_pars(pars) {}

double PsfScaling::operator()(double McEnergy, double McZDir,
                              bool front) const {
   double t(powerLawScaling(McEnergy));
   double x(trendline(McEnergy));
   double scaleFactor(1.);
   if (front) {
      scaleFactor *= (m_pars[4]*x*x - m_pars[5]*x + m_pars[6]);
      scaleFactor *= zfactor(McZDir)*std::sqrt(::sqr(m_pars[7]*t) +
                                               ::sqr(m_pars[8]));
   } else {
      scaleFactor *= (m_pars[9]*x + m_pars[10]);
      scaleFactor *= zfactor(McZDir)*std::sqrt(::sqr(m_pars[11]*t) +
                                               ::sqr(m_pars[12]));
   }
   return scaleFactor;
}

double PsfScaling::powerLawScaling(double McEnergy) const {
   return std::pow(McEnergy/100., m_pars[0]);
}

double PsfScaling::zfactor(double McZDir) const {
   return 1. + m_pars[1]*(1. - std::fabs(McZDir));
}

double PsfScaling::trendline(double McEnergy) const {
   return m_pars[2]*std::log10(McEnergy/100.) + m_pars[3];
}


} //namespace dc2Response
