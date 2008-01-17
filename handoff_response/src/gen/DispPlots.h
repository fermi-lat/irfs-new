/** @file DispPlots.h
    @brief declare class DispPlots
*/
#ifndef Disp_PsfPlots_h
#define Disp_PsfPlots_h


class IrfAnalysis;
class IrfBinner;

#include "Dispersion.h"
#include <vector>

/** @class DispPlots
 *  @brief manage the Dispersion plots
*/

class DispPlots {
public:
    DispPlots( IrfAnalysis& irf, std::ostream& log=std::cout);
    ~DispPlots();

    void fill(double deviat, double energy, double costheta, bool front);

    void fit();
    void summarize();

    void draw(const std::string &ps_filename) ;


    typedef std::vector<Dispersion> Displist;

    const Displist& hists(){return m_hists;} 
    
    // make a set of 2-d histograms with values of the fit parameters
    void fillParameterTables();

    const IrfBinner & binner()const{return m_binner;}

protected:
    IrfAnalysis& m_irf;
    const IrfBinner& m_binner;
    Displist m_hists;
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

};

#endif
