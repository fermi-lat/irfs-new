/**
 * @file AcceptanceCone.h
 * @brief Class to describe acceptance cones on the sky for LAT events.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef irfInterface_AcceptanceCone_h
#define irfInterface_AcceptanceCone_h

#include <cmath>
#include "astro/SkyDir.h"

namespace irfInterface {

/** 
 * @class AcceptanceCone
 *
 * @brief This class encapsulates acceptance cone information, the
 * center of the cone and the half opening angle.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class AcceptanceCone {
    
public:

   AcceptanceCone() {}

   /// @param center The center of the acceptance cone.
   /// @param radius The radius (half opening angle) of the acceptance 
   ///               cone in degrees.
   AcceptanceCone(const astro::SkyDir &center, double radius) :
      m_center(center), m_radius(radius) {}

//   ~AcceptanceCone() {}

   const astro::SkyDir &center() const {return m_center;}

   double radius() const {return m_radius;}

   bool operator==(const AcceptanceCone &rhs) const {
      bool value = true;
      if (fabs(m_radius - rhs.radius()) > 1e-5 ||
          m_center.difference(rhs.center()) > 1e-3) value = false;
      return value;
   }

   bool operator!=(const AcceptanceCone &rhs) const {
      bool value = true;
      if (fabs(m_radius - rhs.radius()) < 1e-5 &&
          m_center.difference(rhs.center()) < 1e-3) value = false;
      return value;
   }

   /// Returns true if dir lies inside the acceptance cone.
   bool inside(astro::SkyDir &dir) {
      return m_center.difference(dir)*180./M_PI < m_radius;
   }

   AcceptanceCone * clone() {
      return new AcceptanceCone(*this);
   }

private:

   astro::SkyDir m_center;

   double m_radius;

};

} // namespace irfInterface

#endif // irfInterface_AcceptanceCone_h

