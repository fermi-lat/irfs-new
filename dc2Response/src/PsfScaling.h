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
   PsfScaling(const std::vector<double> & pars);

   double operator()(double McEnergy, double McZDir, bool front) const;

private:

   /// @brief The 13 "parameters".
   std::vector<double> m_pars;  

   double powerLawScaling(double McEnergy) const;
   double zfactor(double McZDir) const;
   double trendline(double McEnergy) const;

};

} // namespace dc2Response

#endif // dc2Response_PsfScaling_h
