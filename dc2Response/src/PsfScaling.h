/**
 * @file PsfScaling.h
 * @brief Energy scaling of angular deviation for fitting Psf.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc2Response_PsfScaling_h
#define dc2Response_PsfScaling_h

#include <vector>

namespace dc2Response {

/**
 * @class PsfScaling
 *
 * @brief Energy scaling of angular deviation for fitting Psf using
 * Toby's form for the "Atwood Function".  There are apparently at
 * least 13(!) parameters empirically determined for this scaling
 * function for both front and back.  The independent variables are
 * McEnergy and McZDir.
 *
 * @author J. Chiang
 *
 */

class PsfScaling {

public:

   /// @param params The 13 parameters describing the functional
   /// dependence of the angular deviation scaling on McEnergy and
   /// McZDir.
   PsfScaling(const std::vector<double> & pars, bool useFront=true);

   /// @param psfFile FITS file containing the parameters in
   /// PSF_SCALING_PARAMS binary table extension.
   PsfScaling(const std::string & psfFile);

   PsfScaling(const PsfScaling & rhs) {
      m_pars = rhs.m_pars;
   }

   PsfScaling & operator=(const PsfScaling & rhs) {
      if (&rhs != this) {
         m_pars = rhs.m_pars;
      }
      return *this;
   }

   /// @return The energy scaling for the angular deviation in degrees
   /// @param McEnergy True photon energy
   /// @param McZDir Negative of the projection of true photon direction 
   ///        unit vector along the instrument axis.
   double operator()(double McEnergy, double McZDir) const;

private:

   /// @brief The 13 "parameters".
   std::vector<double> m_pars;

   bool m_useFront;

   double powerLawScaling(double McEnergy) const;
   double zfactor(double McZDir) const;
   double trendline(double McEnergy) const;

};

} // namespace dc2Response

#endif // dc2Response_PsfScaling_h
