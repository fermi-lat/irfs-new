/** 
 * @file Util.cxx
 * @brief Implementation for utility class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cassert>
#include <cmath>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "fitsio.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "irfUtil/Util.h"
#include "irfUtil/HdCaldb.h"

namespace irfUtil {

double Util::bilinear(const std::vector<double> &xx, double x, 
                      const std::vector<double> &yy, double y, 
                      const std::vector<double> &z) {

   std::vector<double>::const_iterator ix;
   if (x < *(xx.begin())) {
      ix = xx.begin() + 1;
   } else if (x >= *(xx.end()-1)) {
      ix = xx.end() - 1;
   } else {
      ix = std::upper_bound(xx.begin(), xx.end(), x);
   }
   int i = ix - xx.begin();

   std::vector<double>::const_iterator iy;
   if (y < *(yy.begin())) {
      iy = yy.begin() + 1;
   } else if (y >= *(yy.end()-1)) {
      iy = yy.end() - 1;
   } else {
      iy = std::upper_bound(yy.begin(), yy.end(), y);
   }
   int j = iy - yy.begin();

   double tt = (x - *(ix-1))/(*(ix) - *(ix-1));
   double uu = (y - *(iy-1))/(*(iy) - *(iy-1));

   double y1 = z[yy.size()*(i-1) + (j-1)];
   double y2 = z[yy.size()*(i) + (j-1)];
   double y3 = z[yy.size()*(i) + (j)];
   double y4 = z[yy.size()*(i-1) + (j)];

   double value = (1. - tt)*(1. - uu)*y1 + tt*(1. - uu)*y2 
      + tt*uu*y3 + (1. - tt)*uu*y4; 
   if (value < 0.) {
      std::ostringstream message;
      message << "irfUtil::Util::bilinear:\n"
              << "value = " << value << " < 0\n";
      message << xx[i-1] << "  " << *(ix-1) << "  " 
              << x << "  " << *ix << "\n";
      message << yy[j-1] << "  " << *(iy-1) << "  " 
              << y << "  " << *iy << "\n";
      message << tt << "  " << uu << "  " 
              << y1 << "  " << y2 << "  "
              << y3 << "  " << y4;
      throw std::runtime_error(message.str());
   }
   return value;
}

void Util::getTableVector(const std::string & filename,
                          const std::string & extName,
                          const std::string & columnName, 
                          std::vector<double> & branchVector) {
   tip::Table * my_tree 
      = tip::IFileSvc::instance().editTable(filename, extName);
   tip::Table::Iterator it = my_tree->begin();
   tip::Table::Record & row = *it;
   int nrows = my_tree->getNumRecords();
   branchVector.resize(nrows);
   for (int i = 0; it != my_tree->end() && i < nrows; ++it, ++i) {
      row[columnName].get(branchVector[i]);
   }
   delete my_tree;
}

void Util::getRecordVector(const std::string & filename,
                           const std::string & extName,
                           const std::string & columnName,
                           std::vector<double> & tableVector,
                           int recordNum) {
   tip::Table * my_table 
      = tip::IFileSvc::instance().editTable(filename, extName);
   tip::Table::Iterator it = my_table->begin();
   for (int i = 0; i < recordNum && it != my_table->end(); ++i, ++it);
   (*it)[columnName].get(tableVector);
   delete my_table;
}

void Util::getFitsHduName(const std::string &filename, int hdu,
                          std::string &hduName) {
   int status(0);
   fitsfile * fptr = 0;

   fits_open_file(&fptr, filename.c_str(), READONLY, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("Util::getFitsHduName:\n cfitsio error.");
   }

   int hdutype = 0;
   fits_movabs_hdu(fptr, hdu, &hdutype, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("Util::getFitsHduName:\n cfitsio error.");
   }
   
   char extname[20];
   char comment[72];
   fits_read_key_str(fptr, "EXTNAME", extname, comment, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("Util::getFitsHduName:\n cfitsio error.");
   }

   hduName = extname;
   fits_close_file(fptr, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("Util::getFitsHduName:\n cfitsio error.");
   }
}

void Util::getFitsColNames(const std::string & filename, int hdu,
                           std::vector<std::string> &columnNames) {
   std::string extName;
   getFitsHduName(filename, hdu, extName);
   const tip::Table * my_table =
      tip::IFileSvc::instance().readTable(filename, extName);
   columnNames = my_table->getValidFields();
}

void Util::getCaldbFile(const std::string &detName, 
                        const std::string &respName,
                        const std::string &version,
                        std::string &filename, long &extnum,
                        const std::string & telescope,
                        const std::string & instrument) {
   std::string filter("NONE");
   std::string date("2003-01-01");
   std::string time("00:00:00");
   try {
      std::string expression = "VERSION.eq." + version;
      std::pair<std::string, int> calfile;
      HdCaldb caldb_obj(telescope, instrument);
      calfile = caldb_obj(detName, respName, expression);
      filename = calfile.first;
      extnum = calfile.second + 1;
   } catch (...) {
      std::cout << "irfUtil::Util::getCaldbFile: \n"
                << "Error trying to find filename and extension for \n"
                << "   detName: " << detName
                << "   respName: " << respName
                << "   version: " << version << "\n"
                << "Try checking your CALDB environment variable."
                << std::endl;
      throw;
   }
}

} // namespace irfUtil
