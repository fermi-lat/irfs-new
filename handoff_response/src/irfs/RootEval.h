/** @file RootEval.h
    @brief declare class RootEval

    $Header$
*/

#ifndef handoff_response_RootEval_h
#define handoff_response_RootEval_h

#include "handoff_response/IrfEval.h"

#include <string>

class TFile;

namespace handoff_response {

/** @class RootEval
    @brief Subclass of IrfEval -- Evaluate the functions from a ROOT file


*/
class RootEval : public IrfEval {
public:

    /** @brief ctor
        @param filename name of the root file to open
        @param eventclass name of the event class

    */
    RootEval(std::string filename, std::string eventclass);
    virtual ~RootEval();

    virtual double aeff(double energy, double theta=0, double phi=0);

    virtual double aeffmax();

    virtual double psf(double delta, double energy, double theta=0, double phi=0);

    virtual double dispersion(double emeas, double energy, double theta=0, double phi=0);


private:

    double * psf_par(double energy, double costh);

    double * disp_par(double energy, double costh);

    TFile* m_f;
    class Table; ///< nested class manages table lookup
    Table* setupHist( std::string name);
    Table* m_aeff;
    Table* m_sigma, *m_gcore, *m_gtail; // psf parameters
    Table* m_dnorm, *m_rwidth, *m_ltail;// dispersion parameters

};

} // namespace handoff_response
#endif
