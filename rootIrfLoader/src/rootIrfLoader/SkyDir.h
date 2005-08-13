/** 
 * @file SkyDir.h
 * @brief Wrapper class for astro::SkyDir that does not expose 
 * CLHEP-dependent parts of the interface.
 *
 *  $Header$
 *
 */

#ifndef rootIrfLoader_SkyDir_H
#define rootIrfLoader_SkyDir_H

#include <cmath>

#include <string>

namespace astro {
   class SkyDir;
}

namespace rootIrfLoader {

/** 
 * @class SkyDir
 * @brief Describe an absolute direction
 *
 * Note that units associated with sky coordinates (ra, dec, l, b)
 * are in degrees
 */

class SkyDir {

public:

   typedef enum  { 
      /// fixed direction with respect to the galactic coordinate
      /// system (l,b)
      GALACTIC=0,  
      /// fixed direction with respect to the equatorial coordinate
      /// system (ra,dec) in the J2000 epoch.
      EQUATORIAL=1
   } CoordSystem ;

   ///Constructors
   ///(l,b) or (Ra, Dec) or projection instanciation
   SkyDir(double param1=0, double param2=0, 
          CoordSystem inputType=EQUATORIAL);

   ~SkyDir();
   
   /// @return The underlying astro::SkyDir object
   const astro::SkyDir & astroDir() const;
      
   /// galactic l in degrees
   double l () const;
      
   /// galactic b in degrees
   double b () const;
      
   /// equatorial ra in degrees
   double ra () const;
      
   /// equatorial dec in degrees
   double dec () const;
      
   /// @return the opening angle (in radians) between two objects:
   double difference(const SkyDir & other) const;

private:

   astro::SkyDir * m_dir;
   
};

} // namespace rootIrfLoader

#endif
