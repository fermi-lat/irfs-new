/** @file Dispersion.h
@brief declaration of class Dispersion

$Header$

*/

#ifndef IRF_Dispersion_h
#define IRF_Dispersion_h

#include "embed_python/Module.h"

#include "TF1.h"
#include <string>
#include <iostream>
#include <vector>
class TH1F;

class Dispersion {

public:

    Dispersion(std::string histname,
        std::string title, embed_python::Module &);

 Dispersion():m_count(-1),m_edisp_version(1){} // default 
    ~Dispersion();

    /// add a point with a scaled angular difference delta
    void fill(double scaled_delta, double weight=1.0);

    /// add a summary line to a table, with 68%, 95%, and fit parameters.
    void summarize(std::ostream & out);

    /// draw to the current pad
    void draw(double ymin=1e-6, double ymax=1.0, bool ylog=true);

    /// make a fit, using standard PSF function
    void fit(std::string opts = "RQ");

    /// get vector of fit parameters (all zero if not fit)
    void getFitPars(std::vector<double> & pars)const;

    inline int getFitParSize(){return m_parmap.size();}
    
    std::vector<std::string> getFitParNames();

   /// Set the scale factor parameters
   void setScaleFactorParameters(const std::vector<double> & scaling_pars);

   /// Get the scale factor parameters
   void getScaleFactorParameters(std::vector<double> & scaling_pars);

   /// access to the function itself
   static double function(double* delta, double* par);

    /// call operator for object, turning Dispersion into a functor
    double operator()(double* delta, double* par);

    double entries()const{return m_count;}

    ///! list of names
    static std::vector<std::string> pnames;
    static const char* parname(int i);
    static int npars();
    static double scaleFactor(double energy, double zdir, std::vector<double> edisp_scaling_pars);

    static void summary_title(std::ostream & out);

private:

    TH1F& hist(){return *m_hist;}
    TH1F* m_hist;  ///< managed histogram
    TH1F* m_cumhist; ///< a cumulative histogram, generated before fit
    TF1 m_fitfunc; ///< the fit function

    std::map<std::string,std::vector<double> > m_parmap;

    int m_count; ///< number of entries
    unsigned int m_edisp_version;

    double m_quant[2]; // for 68,95% quantiles
    double m_tail;     // fraction in tail beyond fit range

   std::vector<double> m_scaling_pars;

   double biaslimit;
   double bias2limit;

    void reorder_parameters();
};

#endif
