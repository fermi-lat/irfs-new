/**
 * @file EfficiencyFactor.h
 * @brief Static function that returns the IRF-dependent efficiency
 * corrections as a function of livetime fraction based on fits to
 * Vela, Crab, Geminga data (D. Paneque).
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_EfficiencyFactor_h
#define irfInterface_EfficiencyFactor_h

#include <string>
#include <vector>

namespace irfInterface {

/**
 * @class EfficiencyFactor
 */

class EfficiencyFactor {

public:

   EfficiencyFactor();

   EfficiencyFactor(const std::string & parfile);

   double operator()(double energy, double met) const;

   double value(double energy, double livetimefrac) const;

   void getLivetimeFactors(double energy, double & factor1, 
                           double & factor2) const;

   void readFt2File(std::string ft2file);

   void clearFt2Data();

private:
   
   bool m_havePars;
   double m_offset_p0;
   double m_offset_p1;
   double m_slope_p0;
   double m_slope_p1;
   double m_rate_p0;
   double m_rate_p1;

   double m_dt;
   std::vector<double> m_start;
   std::vector<double> m_stop;
   std::vector<double> m_livetimefrac;

   void readPars(std::string parfile);

};

} // namespace irfInterface

#endif // irfInterface_EfficiencyFactor_h
