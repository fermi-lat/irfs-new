/**
 * @file Bilinear.cxx
 * @brief Implementation for Bilinear 
 *
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "latResponse/Bilinear.h"

namespace latResponse {

Bilinear::Bilinear(const std::vector<float> & x, 
                   const std::vector<float> & y,
                   const std::vector<float> & values) 
   : m_x(x), m_y(y), m_values(values) {}

double Bilinear::operator()(float x, float y) const {
   typedef std::vector<float>::const_iterator const_iterator_t;

   const_iterator_t ix(std::upper_bound(m_x.begin(), m_x.end(), x));
   if (ix == m_x.end() && x != m_x.back()) {
      throw std::invalid_argument("Bilinear::operator: x out of range");
   }
   if (x == m_x.back()) {
      ix = m_x.end() - 1;
   }
   int i(ix - m_x.begin());
    
   const_iterator_t iy(std::upper_bound(m_y.begin(), m_y.end(), y));
   if (iy == m_y.end() && y != m_y.back()) {
      throw std::invalid_argument("Bilinear::operator: y out of range");
   }
   if (y == m_y.back()) {
      iy = m_y.end() - 1;
   }
   int j(iy - m_y.begin());

   double uu((x - m_x.at(i-1))/(m_x.at(i) - m_x.at(i-1)));
   double tt((y - m_y.at(j-1))/(m_y.at(j) - m_y.at(j-1)));

   size_t ysize(m_y.size());

   double y1(m_values.at(ysize*(i-1) + (j-1)));
   double y2(m_values.at(ysize*(i) + (j-1)));
   double y3(m_values.at(ysize*(i) + (j)));
   double y4(m_values.at(ysize*(i-1) + (j)));

   double value = ( (1. - tt)*(1. - uu)*y1 
                    + tt*(1. - uu)*y2  
                    + tt*uu*y3 
                    + (1. - tt)*uu*y4 ); 
   return value;
}

} // namespace latResponse
