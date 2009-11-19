/**
 * @file IEfficiencyFactor.h
 * @brief Function object that returns the IRF-dependent efficiency
 * corrections as a function of livetime fraction.  This interface is
 * based on ratios of the livetime fraction-dependent effective area
 * (P6_v6_diff) to the livetime averaged effective area
 * (P6_V3_DIFFUSE).  See
 * http://confluence.slac.stanford.edu/display/DC2/Efficiency+Correction+Parametrization+as+a+function+of+livetime+fraction+using+MC+(P6_v6_diff)
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_IEfficiencyFactor_h
#define irfInterface_IEfficiencyFactor_h

#include <string>
#include <vector>

namespace irfInterface {

/**
 * @class IEfficiencyFactor
 */

class IEfficiencyFactor {

public:

   virtual ~IEfficiencyFactor() throw() {}

   virtual double operator()(double energy, double met) const = 0;

   virtual double value(double energy, double livetimefrac, 
                        bool front) const = 0;

   virtual double value(double energy, double livetimefrac) const;

   virtual void getLivetimeFactors(double energy, double & factor1, 
                                   double & factor2) const = 0;

   virtual IEfficiencyFactor * clone() const = 0;

   void readFt2File(std::string ft2file);

   void clearFt2Data();


private:

   std::vector<double> m_start;
   std::vector<double> m_stop;
   std::vector<double> m_livetimefrac;

   void readPars(std::string parfile);

};

} // namespace irfInterface

#endif // irfInterface_IEfficiencyFactor_h
