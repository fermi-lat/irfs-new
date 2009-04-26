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

   EfficiencyFactor(std::string python_dir, std::string parfile);

   double operator()(double energy, double met) const;

   double value(double energy, double livetimefrac) const;

   static void readFt2File(const std::string & ft2file);

   static void clearFt2Data();

private:
   
   double m_offset_p0;
   double m_offset_p1;
   double m_slope_p0;
   double m_slope_p1;
   double m_rate_p0;
   double m_rate_p1;

   static double s_dt;
   static std::vector<double> s_start;
   static std::vector<double> s_stop;
   static std::vector<double> s_livetimefrac;

};

} // namespace irfInterface

#endif // irfInterface_EfficiencyFactor_h
