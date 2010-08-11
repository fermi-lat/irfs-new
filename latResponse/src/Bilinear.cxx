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
      Array(const std::vector<double> & values, size_t nx) 
         : m_values(values), m_nx(nx) {}
      double operator()(size_t iy, size_t ix) const {
         return m_values.at(iy*m_nx + ix);
      }
   private:
      const std::vector<double> & m_values;
      size_t m_nx;
   };
}

namespace latResponse {

Bilinear::Bilinear(const std::vector<double> & x, 
                   const std::vector<double> & y,
                   const std::vector<double> & values,
                   double xlo, double xhi, double ylo, double yhi) {
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

double Bilinear::operator()(double x, double y) const {
   double tt, uu;
   std::vector<double> xvals;
   std::vector<double> yvals;
   std::vector<double> zvals;
   getCorners(x, y, tt, uu, xvals, yvals, zvals);
   return evaluate(tt, uu, zvals);
}

double Bilinear::evaluate(double tt, double uu, 
                          const std::vector<double> & zvals) {
   double value = ( (1. - tt)*(1. - uu)*zvals.at(0)
                    + tt*(1. - uu)*zvals.at(1)
                    + tt*uu*zvals.at(2)
                    + (1. - tt)*uu*zvals.at(3) );
   return value;
}

void Bilinear::getCorners(double x, double y, 
                          double & tt, double & uu,
                          std::vector<double> & corner_xvals,
                          std::vector<double> & corner_yvals,
                          std::vector<double> & zvals) const {
   typedef std::vector<double>::const_iterator const_iterator_t;

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

   tt = (x - m_x.at(i-1))/(m_x.at(i) - m_x.at(i-1));
   uu = (y - m_y.at(j-1))/(m_y.at(j) - m_y.at(j-1));

   corner_xvals.clear();
   corner_xvals.push_back(m_x.at(i-1));
   corner_xvals.push_back(m_x.at(i));
   corner_xvals.push_back(m_x.at(i));
   corner_xvals.push_back(m_x.at(i-1));

   corner_yvals.clear();
   corner_yvals.push_back(m_y.at(j-1));
   corner_yvals.push_back(m_y.at(j-1));
   corner_yvals.push_back(m_y.at(j));
   corner_yvals.push_back(m_y.at(j));

   size_t xsize(m_x.size());
   zvals.clear();
   zvals.push_back(m_values.at(xsize*(j-1) + (i-1)));
   zvals.push_back(m_values.at(xsize*(j-1) + (i)));
   zvals.push_back(m_values.at(xsize*(j) + (i)));
   zvals.push_back(m_values.at(xsize*(j) + (i-1)));
}

double Bilinear::getPar(size_t i, size_t j) const {
   Array array(m_values, m_x.size());
   return array(j+1, i+1);
}

void Bilinear::setPar(size_t i, size_t j, double value) {
   m_values.at((j+1)*m_x.size() + i+1) = value;
}

} // namespace latResponse
