/** @file EffectiveArea.h
  @brief create and manage a plot of the effective area

  $Header$
*/
#ifndef IRF_EffectiveArea_h
#define IRF_EffectiveArea_h

#include <vector>
#include <iostream>
#include <string>
class IrfAnalysis;
class TH2F;

/** @class EffectiveArea
 *  @brief manage the effective area plot
*/

class EffectiveArea {
public:
    EffectiveArea( IrfAnalysis& irf, std::ostream& log=std::cout);
    ~EffectiveArea();

    void fill(double energy, double costheta, bool front, int count=0);

    void summarize();

    void draw(const std::string &ps_filename) ;

    void writeFitParameters(std::string outputFile, std::string treename);

    void fillParameterTables();

    
private:
    class Bins {
    public:
        Bins();
        const std::vector<double>& energy_bins()const{return m_energy_bin_edges;}
        const std::vector<double>& angle_bins()const{return m_angle_bin_edges;}
    private:
        std::vector<double> m_energy_bin_edges;
        std::vector<double> m_angle_bin_edges;
    }m_bins;

    IrfAnalysis& m_irf;
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

    TH2F* m_hist;  ///< the 2-d histogram that we manage
};

#endif
