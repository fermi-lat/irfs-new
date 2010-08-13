/**
 * @file PsfBase.h
 * @brief PsfBase class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_PsfBase_h
#define latResponse_PsfBase_h

#include <string>

#include "irfInterface/IPsf.h"

namespace latResponse {

/**
 * @class PsfBase
 *
 */

class PsfBase : public irfInterface::IPsf {

public:

   PsfBase(const std::string & fitsfile, bool isFront,
           const std::string & extname="RPSF");

   PsfBase(const PsfBase & rhs);

   virtual ~PsfBase() {}

   virtual double scaleFactor(double energy, bool isFront) const;

   typedef std::vector<irfInterface::AcceptanceCone *> AcceptanceConeVector_t;

protected:

   /// Disable this.
   PsfBase & operator=(const PsfBase &) {
      return *this;
   }

   virtual double scaleFactor(double energy) const;

private:

   // PSF scaling parameters
   double m_par0;
   double m_par1;
   double m_index;

   // store all of the PSF parameters
   std::vector<double> m_psf_pars;

   void readScaling(const std::string & fitsfile, bool isFront,
                    const std::string & extname="PSF_SCALING_PARAMS");
};

} // namespace latResponse

#endif // latResponse_PsfBase_h
