/**
 * @file Table.h
 * @brief Table abstraction to manage IRF data which are assumed to be a 
 * function of log10(E/MeV) and cos(theta).
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef handoff_response_Table_h
#define handoff_response_Table_h

#include <vector>

class TAxis;
class TH2F;

namespace handoff_response {

class Bilinear;

class Table {

public:

   Table(TH2F * hist);

   ~Table();
      
   /// @brief lookup a value from the table
   /// @param logenergy log10(energy)
   /// @param costh cos(theta)
   /// @param interpolate [true] if true, make linear
   /// interpolation. Otherwise take value for given cell
   double value(double logenergy, double costh, bool interpolate=true);
    
   double maximum();

   float minCosTheta() const {
      return m_minCosTheta;
   }

private:

   TH2F * m_hist;

   std::vector<float> m_energy_axis; 
   std::vector<float> m_angle_axis;
   std::vector<float> m_data_array;

   std::vector<float> m_ebounds;
   std::vector<float> m_tbounds;

   float m_minCosTheta;

   float m_maxValue;

   Bilinear * m_interpolator;

   /// Fill vector array with the bin edges in a ROOT TAxis, with
   /// extra ones for the overflow bins
   void binArray(double low_limit, double high_limit, 
                 TAxis * axis, std::vector<float> & array,
                 std::vector<float> & bounds);

}; // class Table

} // namespace handoff_response

#endif // handoff_response_Table_h

