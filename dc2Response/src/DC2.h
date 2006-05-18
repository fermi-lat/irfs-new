/**
 * @file DC2.h
 * @brief Definition of base class for DC2 response functions.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc2Response_DC2_h
#define dc2Response_DC2_h

#include <string>
#include <vector>

namespace dc2Response {

/**
 * @class DC2
 *
 * @brief Base class for DC2 response functions.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class DC2 {

protected:

   /// There is no reason to create an object directly from this
   /// class, so make the constructors protected (but still available
   /// to the sub-classes).
   DC2() {}

   DC2(const std::string & filename, const std::string & extname);

   DC2(const DC2 &rhs);

   std::string m_filename;
   std::string m_extname;

   /// Return the iterator pointing to the element just below or equal
   /// to the target value.
   static std::vector<double>::const_iterator
   find_iterator(const std::vector<double> & gridValues, double target);

};

} // namespace dc2Response

#endif // dc2Response_DC2_h
