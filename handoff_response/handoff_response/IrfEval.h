/** @file IrfEval.h
    @brief declare class IrfEval

    $Header$
*/

#ifndef handoff_response_IrfEval_h
#define handoff_response_IrfEval_h

#include <string>

class TFile;
class TH2F;

namespace handoff_response {

class IrfEval {
public:
    IrfEval(std::string filename, std::string eventtype);
    ~IrfEval();

    double aeff(double energy, double theta=0, double phi=0);

    double aeffmax();

    double psf(double delta, double energy, double theta=0, double phi=0);

    double dispersion(double emeas, double energy, double theta=0, double phi=0);

    std::string name()const{return m_type;}

private:

    TH2F* setupHist( std::string name);
    double * psf_par(double energy, double costh);

    double * disp_par(double energy, double costh);

    TFile* m_f;
    bool m_front;
    std::string m_type;
    TH2F* m_aeff;
    TH2F* m_sigma, *m_gcore, *m_gtail; // psf parameters
    TH2F* m_dnorm, *m_rwidth, *m_ltail;// dispersion parameters

};

} // namespace handoff_response
#endif
