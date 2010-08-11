/** 
 * @file Bilinear.h
 * @brief Definition of interpolating class Bilinear.
 *
 * $Header$
 */

#ifndef latResponse_Bilinear_h
#define latResponse_Bilinear_h

#include <vector>

namespace latResponse {

/**  
 * @class Bilinear
 *
 * @brief Bilinear interpolator.  Hold references to the vectors so
 * that introspection and greater transparency is available during
 * debugging.
 *
 */

class Bilinear {

public:

   Bilinear(const std::vector<float> & x, const std::vector<float> & y, 
            const std::vector<float> & values);

   Bilinear(const std::vector<float> & x, const std::vector<float> & y, 
            const std::vector<float> & values, 
            float xlo, float xhi, float ylo, float yhi);

   double operator()(float x, float y) const;

   void getCorners(float x, float y, 
                   double & tt, double & uu,
                   std::vector<double> & yvals) const;

   static double evaluate(double tt, double uu, 
                          const std::vector<double> & yvals);

private:

   std::vector<float> m_x;
   std::vector<float> m_y;
   std::vector<float> m_values;
   
};

} // namespace latResponse

#endif // latResponse_Bilinear_h
