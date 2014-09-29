/** @file FisheyePlots.h
    @brief declare class FisheyePlots

    $Header$
*/
#ifndef IRF_FisheyePlots_h
#define IRF_FisheyePlots_h


class IrfAnalysis;

#include "IrfBinner.h"
#include "FisheyeHist.h"
#include "embed_python/Module.h"

#include <vector>

/** @class PSFPlots
 *  @brief manage the PSF plots
*/

class FisheyePlots {
public:
  FisheyePlots( IrfAnalysis& irf, std::ostream& log, embed_python::Module& py);
  ~FisheyePlots();

  void fill(double diff, double energy, double costheta, bool front);

  void fit();
  void summarize();

  void draw(const std::string &ps_filename) ;
  
  typedef std::vector<FisheyeHist> PSFlist;

  const PSFlist& hists(){return m_hists;} 
    
  // make a set of 2-d histograms with values of the fit parameters
  void fillParameterTables();

  const IrfBinner & binner()const{return m_binner;}

private:

    IrfAnalysis& m_irf;
    IrfBinner m_binner;
    PSFlist   m_hists;
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}
};

#endif
