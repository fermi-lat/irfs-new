/** @file PointSpreadFunction.h
@brief declaration of class PointSpreadFunction

$Header$

*/

#ifndef IRF_PointSpreadFunction_h
#define IRF_PointSpreadFunction_h


#include "TF1.h"
#include <string>
#include <iostream>
#include <vector>
class TH1F;

class PointSpreadFunction {

public:

    PointSpreadFunction(std::string histname,
        std::string title
        , std::ostream& log=std::cout);

    PointSpreadFunction():m_count(-1){} // default 
    ~PointSpreadFunction();

    /// add a point with a scaled angular difference delta
    void fill(double scaled_delta, double weight=1.0);

    /// add a summary line to a table, with 68%, 95%, and fit parameters.
    void summarize();

    /// draw to the current pad
    void draw(double ymin=1e-6, double ymax=1.0, bool ylog=true);

    /// make a fit, using standard PSF function
    void fit(std::string opts = "RQ");

    /// get vector of fit parameters (all zero if not fit)
    void getFitPars(std::vector<double> & pars)const;

    /// scale factor to apply to data
    static double scaleFactor(double energy, double zdir, bool front);

    /// access to the function itself
    static double function(double* delta, double* par);

    /// the integral of the function
    static double integral(double* delta, double* par);
    double entries()const{return m_count;}

    ///! list of names
    static std::vector<std::string> pnames;

    static const char* parname(int i);
    static int npars();
private:

    TH1F& hist(){return *m_hist;}
    TH1F* m_hist;  ///< managed histogram
    TH1F* m_cumhist; ///< a cumulative histogram, generated before fit
    TF1 m_fitfunc; ///< the fit function

    int m_count; ///< number of entries
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

    double m_quant[2]; // for 68,95% quantiles
    double m_tail;     // fraction in tail beyond fit range

    void reorder_parameters();
    void setFitPars(double * pars, double * pmin, double * pmax);
};

#endif
