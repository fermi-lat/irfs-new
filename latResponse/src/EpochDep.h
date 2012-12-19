/**
 * @file EpochDep.h
 * @brief Mix-in class to provide functionality needed for
 * epoch-dependent IRFs
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef _latResponse_EpochDep_h
#define _latResponse_EpochDep_h

#include <string>
#include <vector>

namespace latResponse {

class EpochDep {

public:

   /// @return Validity start time from CVSD0001 keyword in MET.
   static double epochStart(const std::string & fitsfile,
                            const std::string & extname);

protected:

   EpochDep();

   std::vector<double> m_epochStart;
   
   mutable size_t m_curr_index;

   void appendEpoch(double epoch_start);

   size_t index(double met) const;

};

} // namespace latResponse

#endif // _latResponse_EpochDep_h
