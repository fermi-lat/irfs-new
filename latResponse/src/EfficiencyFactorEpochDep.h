/**
 * @file EfficiencyFactorEpochDep.h
 * @brief Epoch-dependent efficiency factor class
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_EfficiencyFactorEpochDep_h
#define latResponse_EfficiencyFactorEpochDep_h

#include <string>
#include <vector>

#include "irfInterface/IEfficiencyFactor.h"
#include "EpochDep.h"

namespace latResponse {

/**
 * @class EfficiencyFactorEpochDep
 */

class EfficiencyFactorEpochDep 
   : public irfInterface::IEfficiencyFactor, EpochDep {

public:

   EfficiencyFactorEpochDep();

   EfficiencyFactorEpochDep(const EfficiencyFactorEpochDep & other);
   
   EfficiencyFactorEpochDep & operator=(const EfficiencyFactorEpochDep & rhs);

   virtual ~EfficiencyFactorEpochDep() throw();

   virtual double operator()(double energy, double met) const;

   virtual double value(double energy, double livetimefrac, bool front,
                        double met) const;

   virtual void getLivetimeFactors(double energy, double & factor1, 
                                   double & factor2, double met) const;

   virtual irfInterface::IEfficiencyFactor * clone() const {
      return new EfficiencyFactorEpochDep(*this);
   }

   void add(irfInterface::IEfficiencyFactor & eff, double epoch_start);

private:
   
   std::vector<irfInterface::IEfficiencyFactor *> m_effs;

};

} // namespace latResponse

#endif // latResponse_EfficiencyFactorEpochDep_h
