/** @file RootEval.h
    @brief declare class RootEval

    $Header$
*/

#ifndef handoff_response_RootEval_h
#define handoff_response_RootEval_h

#include "handoff_response/IrfEval.h"

#include <string>
#include <map>

class TFile;

namespace handoff_response {

/** @class RootEval
    @brief Subclass of IrfEval -- Evaluate the functions from a ROOT file


*/
class RootEval : public IrfEval {
public:

    /** @brief ctor
        @param file  ROOT file
        @param eventclass name of the event class

    */
    RootEval(TFile* file, std::string eventclass);

    virtual ~RootEval();

    virtual double aeff(double energy, double theta=0, double phi=0);

    virtual double aeffmax();

    virtual double psf(double delta, double energy, double theta=0, double phi=0);
    virtual double psf_integral(double delta, double energy, double theta, double phi=0);


    virtual double dispersion(double emeas, double energy, double theta=0, double phi=0);

    static void RootEval::createMap(std::string filename, std::map<std::string,handoff_response::IrfEval*>& evals);


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
