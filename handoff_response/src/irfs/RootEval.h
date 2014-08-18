/** @file RootEval.h
    @brief declare class RootEval

    $Header$
*/

#ifndef handoff_response_RootEval_h
#define handoff_response_RootEval_h

#include "handoff_response/IrfEval.h"

#include <string>
#include <map>
#include <vector>

class TFile;

namespace handoff_response {

   class Bilinear;
   class Table;

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

   RootEval(const std::string & eventClass) 
      : IrfEval(eventClass), m_f(0) {}

    virtual ~RootEval();

    virtual double aeff(double energy, double theta=0, double phi=0);

    virtual double aeffmax();

    virtual double psf(double delta, double energy, double theta=0, double phi=0);
    virtual double psf_integral(double delta, double energy, double theta, double phi=0);


    virtual double dispersion(double emeas, double energy, double theta=0, double phi=0);

    static void createMap(std::string filename, std::map<std::string,handoff_response::IrfEval*>& evals);

   /// @brief Expose parameters for caching PSF integrals in
   /// handoff_response::Psf class.
   /// @param True energy (MeV)
   /// @param True incident photon inclination (wrt z-axis) (degrees)
   /// @param params map of parameter values with parameter name / value pairs
   void getPsfPars(double energy, double inclination,
                   std::map<std::string, double> & params);

protected:
   
   TFile * m_f;

   Table * m_aeff;

   std::vector<Table *> m_dispTables;

   std::vector<Table *> m_psfTables;

private:

    double * psf_par(double energy, double costh);

    double * disp_par(double energy, double costh);

    Table* setupHist( std::string name);
    void setupParameterTables(const std::string& name, 
                              std::vector<Table*>&tables);

   double m_loge_last;
   double m_costh_last;

   double m_loge_last_edisp;
   double m_costh_last_edisp;

};

} // namespace handoff_response
#endif
