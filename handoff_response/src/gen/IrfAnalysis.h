/** 
 * @file IrfAnalysis.h
 * @brief Analysis of PSf
 * $Header$
 */

#include "MyAnalysis.h"
#include "IrfBinner.h"
#include <string>

class PsfPlots;
class DispPlots;
class EffectiveArea;
class AeffPhiDep;

namespace embed_python {
   class Module;
}

/** 
 * @class IrfAnalysis
 * @brief Manage fits to binned data, both PSF and dispersion
 */

class IrfAnalysis : public MyAnalysis {
public:
   /// @param folder where to find data file, to put analysis files
   IrfAnalysis(std::string folder, embed_python::Module & py); 

   /// do the fits, optionally make plots and write out parameters to
   /// a root file
   void fit(bool make_plots=false, std::string output_type="ps");

   std::string name() const{
      return m_name;
   }
   void setName(std::string name) {
      m_name = name;
   }

   /// @brief write out fit parameters to the ROOT file, tabular TTree
   /// do both psf and dispersion 
   void writeFitParameters(std::string outputFile);

   const IrfBinner & binner()const{return m_binner;}

    /** 
     * @class IrfAnalysis::Normalization
     * @brief information allowing normalization for effective area
     */
    class Normalization {
    public:
       /// @param generated number of events initially generated
       /// @param logemin minimum log10(McEnergy)
       /// @param logemax maxiumum log10(McEnergy)
       
       Normalization(int generated, double logemin, double logemax)
          :m_events(generated)
          ,m_low(logemin)
          ,m_high(logemax)
          {}
       bool in_range(double loge, double costh) const {
          return loge>m_low && loge<=m_high && costh>0 && costh<=1;
       } 
       int generated()const{return m_events;}
       double logemin()const{return m_low;}
       double logemax()const{return m_high;}
       double value(double loge, double costh) const {
          return in_range(loge, costh) ? m_events/(m_high-m_low) : 0;
       } 
    private:
       int m_events;
       double  m_low, m_high;
    };

   const std::vector<Normalization> & normalization() const {
      return m_norm;
   }
   std::vector<Normalization> & normalization() {
      return m_norm;
   }

   // the root file where we save all the histograms
   std::string summary_filename() const {
      return m_output_dir + "/" + m_outputfile;
   }

   double generate_area()const {
      return m_generate_area;
   }

protected:

   void project();

private:

   IrfBinner m_binner;
   std::vector<Normalization> m_norm;
   double m_generate_area;
   std::string m_name;
   std::string m_output_dir;
   std::string m_filename_root;
   
   /// Data members to allow configurable selection of merit variables
   /// for reconstructed direction cosines and energy.
   std::string m_bestXDir;
   std::string m_bestYDir;
   std::string m_bestZDir;
   std::string m_bestEnergy;
    
   PsfPlots* m_psf;
   DispPlots* m_disp;
   EffectiveArea* m_aeff;
   AeffPhiDep * m_phi_dep;

   std::ostream * m_log;
   /// event class, derived from folder name
   std::string m_classname; 
   std::ostream & out() {return *m_log;}

   std::string m_parameterFile;
   std::string m_outputfile;
};

