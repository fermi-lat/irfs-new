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

public:

   virtual ~DC2() {}

   /// Return the maximum allowed value of the source inclination wrt
   /// the instrument z-axis (in degrees).
   double incMax();

protected:

   /// There is no reason to create an object directly from this
   /// class, so make the constructors protected (but still available
   /// to the sub-classes).
   DC2() {}

   DC2(const std::string &filename, bool havePars=true);

   /// Use this constructor for psf and edisp FITS files.
   DC2(const std::string &filename, int hdu, int npars);

   /// Use this constructor for aeff FITS files.
   DC2(const std::string &filename, int hdu);

   DC2(const DC2 &rhs);

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

} // namespace dc2Response

#endif // dc2Response_DC2_h
