/**
 * @file Glast25.h
 * @brief Definition of base class for Glast25 response functions.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef g25Response_Glast25_h
#define g25Response_Glast25_h

#include <string>
#include <vector>

namespace g25Response {

/**
 * @class Glast25
 *
 * @brief Base class for Glast25 response functions.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Glast25 {

public:

   virtual ~Glast25() {}

   /// Type-fields for specifying response file HDUs.
   enum HDU {Front = 2, Back, Combined};

   /// Return the maximum allowed value of the source inclination wrt
   /// the instrument z-axis (in degrees).
   static double incMax() {return s_incMax;}

   /// An identifier for the IRFs that are being used.  Here we use
   /// m_hdu.
   int irfId() const {return m_hdu;}

protected:

   /// There is no reason to create an object directly from this
   /// class, so make the constructors protected and therefore
   /// available to the sub-classes.
   Glast25() {}

   Glast25(const std::string &filename, int hdu) 
      : m_filename(filename), m_hdu(hdu) {}

   std::string m_filename;
   int m_hdu;

private:

   static double s_incMax;

};

} // namespace g25Response

#endif // g25Response_Glast25_h
