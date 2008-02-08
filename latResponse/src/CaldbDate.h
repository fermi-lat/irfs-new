/**
 * @file CaldbDate.h
 * @brief Small class to allow for easy comparisons of dates in the
 * caldb.indx file.
 * 
 * @author J. Chiang
 *
 * $Header$
 */

#include "astro/JulianDate.h"

#ifndef latResponse_CaldbDate_h
#define latResponse_CaldbDate_h

namespace latResponse {

/**
 * @class CaldbDate
 *
 */

class CaldbDate {

public:

   CaldbDate(const std::string & date) : m_jd(0) {
      std::vector<std::string> tokens;
      facilities::Util::stringTokenize(date, "-", tokens);
      int year(std::atoi(tokens.at(0).c_str()));
      int month(std::atoi(tokens.at(1).c_str()));
      int day(std::atoi(tokens.at(2).c_str()));

      m_jd = new astro::JulianDate(year, month, day, 0.);
   }

   CaldbDate(const CaldbDate & other) 
      : m_jd(new astro::JulianDate(*(other.m_jd))) {}

   ~CaldbDate() {
      delete m_jd;
   }

   CaldbDate & operator=(const CaldbDate & rhs) {
      if (this == &rhs) {
         return *this;
      }
      delete m_jd;
      m_jd = new astro::JulianDate(*(rhs.m_jd));
      return *this;
   }

   bool operator<(const CaldbDate & rhs) const {
      return *m_jd < *(rhs.m_jd);
   }

   bool operator==(const CaldbDate & rhs) const {
      return *m_jd == *(rhs.m_jd);
   }

   bool operator>(const CaldbDate & rhs) const {
      return !(operator<(rhs) || operator==(rhs));
   }

private:

   astro::JulianDate * m_jd;

};

} // namespace latResponse

#endif // latResponse_CaldbDate_h
