/**
 * @file EdispInterpolator
 * @brief Class to perform bilinear interpolation of IRF distributions
 * on the log(energy)-cos(theta) grid on which IRF parameters are fit.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef latResponse_EdispInterpolator_h
#define latResponse_EdispInterpolator_h

#include <cmath>

#include <stdexcept>
#include <string>
#include <vector>

#include "latResponse/Bilinear.h"

namespace latResponse {

class EdispInterpolator {

public:

   EdispInterpolator(const std::string & fitsfile,
                   const std::string & extname,
                   size_t nrow);

   ~EdispInterpolator() throw();

#ifndef SWIG
   template<class IrfClass>
   double evaluate(const IrfClass & irfClass, double emeas, 
                   double energy, double theta, double phi,
                   double time=0) const {
      if (!m_renormalized) {
         // We need to do this here explicitly since the renormalize
         // function belongs to irfClass.
         size_t ipars(0);
         for (size_t j(0); j < m_cosths.size(); j++) {
            for (size_t k(0); k < m_logEs.size(); k++, ipars++) {
               irfClass.renormalize(m_logEs[k], m_cosths[j], 
                                    const_cast<double *>(&m_parVectors[ipars][0]));
            }
         }
         m_renormalized = true;
      }
      double tt, uu;
      std::vector<double> cornerEnergies(4);
      std::vector<double> cornerThetas(4);
      std::vector<size_t> index(4);
      getCornerPars(energy, theta, phi, time, tt, uu, 
                    cornerEnergies, cornerThetas, index);
      std::vector<double> yvals(4);
      double scaled_energy((emeas - energy)/energy);
      for (size_t i(0); i < 4; i++) {
         double my_emeas(cornerEnergies[i]*scaled_energy + cornerEnergies[i]);
         yvals[i] = irfClass.evaluate(my_emeas, cornerEnergies[i],
                                      cornerThetas[i], phi, time,
                                      const_cast<double *>(&m_parVectors[index[i]][0]));
         /// By using my_emeas, we are effectively rescaling the x-axis
         /// by the ratio of true energies. This extra factor is the
         /// Jacobian of that transformation.
         yvals[i] *= cornerEnergies[i]/energy;
      }
      double my_value(Bilinear::evaluate(tt, uu, &yvals[0]));
      return my_value;
   }
#endif // SWIG

   const std::string & fitsfile() const {
      return m_fitsfile;
   }
   const std::string & extname() const {
      return m_extname;
   }
   size_t nrow() const {
      return m_nrow;
   }

   void getCornerPars(double energy, double theta, double phi, double time,
                      double & tt, double & uu,
                      std::vector<double> & cornerEnergies,
                      std::vector<double> & cornerThetas,
                      std::vector<size_t> & index) const;

private:

   std::string m_fitsfile;
   std::string m_extname;
   size_t m_nrow;
   mutable bool m_renormalized;

   std::vector<double> m_logEs;
   std::vector<double> m_energies;
   std::vector<double> m_cosths;
   std::vector<double> m_thetas;
   std::vector<std::vector<double> > m_parVectors;

   void readFits();

   static int findIndex(const std::vector<double> & xx, double x);

   static void generateBoundaries(const std::vector<double> & x,
                                  const std::vector<double> & y,
                                  const std::vector<double> & values,
                                  std::vector<double> & xout,
                                  std::vector<double> & yout,
                                  std::vector<double> & values_out, 
                                  double xlo=0, double xhi=10., 
                                  double ylo=-1., double yhi=1.);

};

} // namespace latResponse

#endif // latResponse_EdispInterpolator_h
