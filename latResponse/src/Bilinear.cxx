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

namespace {
   class Array {
   public:
      Array(const std::vector<float> & values, size_t nx) 
         : m_values(values), m_nx(nx) {}
      float operator()(size_t iy, size_t ix) const {
         return m_values.at(iy*m_nx + ix);
      }
   private:
      const std::vector<float> & m_values;
      size_t m_nx;
   };
}

namespace latResponse {

Bilinear::Bilinear(const std::vector<float> & x, 
                   const std::vector<float> & y,
                   const std::vector<float> & values) 
   : m_x(x), m_y(y), m_values(values) {}

Bilinear::Bilinear(const std::vector<float> & x, 
                   const std::vector<float> & y,
                   const std::vector<float> & values,
                   float xlo, float xhi, float ylo, float yhi) {
   m_x.resize(x.size() + 2);
   std::copy(x.begin(), x.end(), m_x.begin() + 1);
   m_x.front() = xlo;
   m_x.back() = xhi;

   m_y.resize(y.size() + 2);
   std::copy(y.begin(), y.end(), m_y.begin() + 1);
   m_y.front() = ylo;
   m_y.back() = yhi;

   Array array(values, x.size());
   m_values.push_back(array(0, 0));
   for (size_t i(0); i < x.size(); i++) {
      m_values.push_back(array(0, i));
   }
   m_values.push_back(array(0, x.size()-1));
   for (size_t j(0); j < y.size(); j++) {
      m_values.push_back(array(j, 0));
      for (size_t i(0); i < x.size(); i++) {
         m_values.push_back(array(j, i));
      }
      m_values.push_back(array(j, x.size()-1));
   }
   m_values.push_back(array(y.size()-1, 0));
   for (size_t i(0); i < x.size(); i++) {
      m_values.push_back(array(y.size()-1, i));
   }
   m_values.push_back(array(y.size()-1, x.size()-1));
}

double Bilinear::operator()(float x, float y) const {
   typedef std::vector<float>::const_iterator const_iterator_t;

   const_iterator_t ix(std::upper_bound(m_x.begin(), m_x.end(), x));
   if (ix == m_x.end() && x != m_x.back()) {
      throw std::invalid_argument("Bilinear::operator: x out of range");
   }
   if (x == m_x.back()) {
      ix = m_x.end() - 1;
   } else if (x <= m_x.front()) {
      ix = m_x.begin() + 1;
   }
   int i(ix - m_x.begin());
    
   const_iterator_t iy(std::upper_bound(m_y.begin(), m_y.end(), y));
   if (iy == m_y.end() && y != m_y.back()) {
      throw std::invalid_argument("Bilinear::operator: y out of range");
   }
   if (y == m_y.back()) {
      iy = m_y.end() - 1;
   } else if (y <= m_y.front()) {
      iy = m_y.begin() + 1;
   }
   int j(iy - m_y.begin());

   double tt((x - m_x.at(i-1))/(m_x.at(i) - m_x.at(i-1)));
   double uu((y - m_y.at(j-1))/(m_y.at(j) - m_y.at(j-1)));

   size_t xsize(m_x.size());

   double y1(m_values.at(xsize*(j-1) + (i-1)));
   double y2(m_values.at(xsize*(j-1) + (i)));
   double y3(m_values.at(xsize*(j) + (i)));
   double y4(m_values.at(xsize*(j) + (i-1)));

//    std::cout << x << "  " << tt << "  "
//              << y << "  " << uu << "  "
//              << y1 << "  " << y2 << "  "
//              << y3 << "  " << y4 << "  latResponse" << std::endl;

   double value = ( (1. - tt)*(1. - uu)*y1 
                    + tt*(1. - uu)*y2  
                    + tt*uu*y3 
                    + (1. - tt)*uu*y4 ); 
   return value;
}

} // namespace latResponse
