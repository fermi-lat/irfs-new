/**
 * @file EfficiencyFactor.h
 * @brief Function that returns the IRF-dependent efficiency
 * corrections as a function of livetime fraction.  This is based on 
 * ratios of the livetime fraction-dependent effective area (P6_v6_diff) to
 * the livetime averaged effective area (P6_V3_DIFFUSE).  See
 * http://confluence.slac.stanford.edu/display/DC2/Efficiency+Correction+Parametrization+as+a+function+of+livetime+fraction+using+MC+(P6_v6_diff)
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_EfficiencyFactor_h
#define latResponse_EfficiencyFactor_h

#include <string>
#include <vector>

#include "latResponse/IEfficiencyFactor.h"

namespace latResponse {

/**
 * @class EfficiencyFactor
 */

class EfficiencyFactor : public latResponse::IEfficiencyFactor {

public:

   EfficiencyFactor();

   EfficiencyFactor(const std::string & parfile);

   virtual ~EfficiencyFactor() throw() {}

   virtual double operator()(double energy, double met) const;

   virtual double value(double energy, double livetimefrac, bool front) const;

   virtual void getLivetimeFactors(double energy, double & factor1, 
                                   double & factor2) const;

   virtual IEfficiencyFactor * clone() const {
      return new EfficiencyFactor(*this);
   }

private:
   
   bool m_havePars;

   /**
    * @class EfficiencyParameter
    *
    * @brief "Meta-parameter" that is used in the parameterization of
    * the loss of efficiency due to accidental coincidences.  This is
    * a functor that implements the piecewise linear fits to the
    * parameters in the expression $\xi = p_0 log10(E/MeV) + p_1$
    * where $\xi$ is the ratio of effective areas for the livetime
    * fraction-dependent case to the livetime-averaged value.
    */
   class EfficiencyParameter {
   public:
      EfficiencyParameter() {}
      EfficiencyParameter(const std::vector<double> & pars) {
         m_a0 = pars.at(0);
         m_b0 = pars.at(1);
         m_a1 = pars.at(2);
         m_logEb1 = pars.at(3);
         m_a2 = pars.at(4);
         m_logEb2 = pars.at(5);
      }
      double operator()(double logE) const {
         if (logE < m_logEb1) {
            return m_a0*logE + m_b0;
         }
         double b1 = (m_a0 - m_a1)*m_logEb1 + m_b0;
         if (logE < m_logEb2) {
            return m_a1*logE + b1;
         }
         double b2 = (m_a1 - m_a2)*m_logEb2 + b1;
         return m_a2*logE + b2;
      }
   private:
      double m_a0;
      double m_b0;
      double m_a1;
      double m_logEb1;
      double m_a2;
      double m_logEb2;
   } m_p0_front, m_p1_front, m_p0_back, m_p1_back;

   double m_dt;
   std::vector<double> m_start;
   std::vector<double> m_stop;
   std::vector<double> m_livetimefrac;

   void readPars(std::string parfile);

};

} // namespace latResponse

#endif // latResponse_EfficiencyFactor_h
