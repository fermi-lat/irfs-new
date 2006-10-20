/**  @file Bilinear.cxx
*     @brief Implementation for Bilinear 
* 
*     $Header$
*/

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "Bilinear.h"

using namespace handoff_response;

Bilinear::Bilinear(const std::vector<float> &x, const std::vector<float> &y, const std::vector<float> &vals)
: m_xbegin(x.begin())
, m_xend(x.end())
, m_ybegin(y.begin())
, m_yend(y.end())
, m_valsbegin(vals.begin())
{}
Bilinear::Bilinear(const_iterator xbegin, const_iterator xend, const_iterator ybegin, const_iterator yend , const_iterator vals_begin)
: m_xbegin(xbegin)
, m_xend(xend)
, m_ybegin(ybegin)
, m_yend(yend)
, m_valsbegin(vals_begin)
 {
 }

double Bilinear::operator()( double x,  double y)const
{

    const_iterator ix = std::upper_bound(m_xbegin , m_xend, x);
    if( ix==m_xend) {
     //mabye ok?   throw std::invalid_argument("Bilinear::operator -- x out of range");
    }
    int j = ix - m_xbegin;
    
    const_iterator iy= std::upper_bound(m_ybegin, m_yend, y);
    if( iy==m_yend) {
   //     throw std::invalid_argument("Bilinear::operator -- y out of range");
    }
    int i = iy - m_ybegin;

    double uu = (x - *(ix-1))/(*(ix) - *(ix-1));
    double tt = (y - *(iy-1))/(*(iy) - *(iy-1));
    size_t size = m_xend - m_xbegin;;

    // get values for the corners of the grid element
    double y1 = *(m_valsbegin + size*(i-1) + (j-1) ) ;
    double y2 = *(m_valsbegin + size*(i)   + (j-1) );
    double y3 = *(m_valsbegin + size*(i)   + (j) );
    double y4 = *(m_valsbegin + size*(i-1) + (j) );

    double value 
        =  (1. - tt)*(1. - uu)*y1 
        + tt*(1. - uu)*y2  
        + tt*uu*y3 
        + (1. - tt)*uu*y4; 

    return value;
}

