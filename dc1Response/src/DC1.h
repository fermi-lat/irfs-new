/**
 * @file DC1.h
 * @brief Definition of base class for DC1 response functions.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc1Response_DC1_h
#define dc1Response_DC1_h

#include <vector>
#include <string>

namespace dc1Response {

/**
 * @class DC1
 *
 * @brief Base class for DC1 response functions.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class DC1 {

public:

   virtual ~DC1() {}

   /// Return the maximum allowed value of the source inclination wrt
   /// the instrument z-axis (in degrees).
   double incMax();

protected:

   /// There is no reason to create an object directly from this
   /// class, so make the constructors protected (but still available
   /// to the sub-classes).
   DC1() {}

   DC1(const std::string &filename, bool havePars=true);

   /// Use this constructor for psf and edisp FITS files.
   DC1(const std::string &filename, int hdu, int npars);

   /// Use this constructor for aeff FITS files.
   DC1(const std::string &filename, int hdu);

   DC1(const DC1 &rhs);

   std::string m_filename;

   int m_hdu;
   int m_npars;
   bool m_have_FITS_data;

   std::vector< std::vector<double> > m_pars;
   std::vector<double> m_theta;
   std::vector<double> m_energy;

   void readFitParams();
   void readGridBoundaries();

   const std::vector<double> &fitParams(double energy, 
                                        double inclination) const;
   int getParamsIndex(double energy, double inclination) const;

   /// Return the iterator pointing to the element just below or equal
   /// to the target value.
   std::vector<double>::const_iterator
   find_iterator(const std::vector<double> &gridValues, double target) const;

};

} // namespace dc1Response

#endif // dc1Response_DC1_h
