/** @file Bilinear.h
*    @brief Definition of interpolating class  Bilinear .
*
* $Header$
*/

#ifndef handoff_response_Bilinear_h
#define handoff_response_Bilinear_h

#include <vector>

namespace handoff_response {

    /**  @class Bilinear
          @brief  Bilinear interpolator
         @author T. Burnett ( some code from J. Chiang)
    */
    class Bilinear {
    public:
        typedef std::vector<float>::const_iterator const_iterator;
        /**
        @param x define x-axis 
        @param y define y-axis 
        @param vals 2-d array of x vs y. Stored as rows in x, first row at y[0] from x[0] to x[max], etc.
        */
        Bilinear(const std::vector<float> &x, const std::vector<float> &y, const std::vector<float> &vals);

        Bilinear( const_iterator xbegin, const_iterator xend, const_iterator ybegin, const_iterator yend , const_iterator vals_begin );


        /** @brief the interpolating function */
        double operator()(double x, double y)const;

    private:
        const_iterator m_xbegin, m_xend, m_ybegin, m_yend, m_valsbegin;

    };

} // namespace

#endif // l

