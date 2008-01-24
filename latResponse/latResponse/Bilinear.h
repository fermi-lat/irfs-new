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

   double operator()(float x, float y) const;

private:

   const std::vector<float> & m_x;

   const std::vector<float> & m_y;

   const std::vector<float> & m_values;
   
};

} // namespace latResponse

#endif // latResponse_Bilinear_h
