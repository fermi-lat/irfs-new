/**
 * @file PhiDepHist.h
 * @brief Class to fit parameterization of phi-dependence of effective area.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_gen_PhiDepHist_h
#define handoff_response_gen_PhiDepHist_h

#include <string>
#include <vector>

class TF1;
class TH1F;

class PhiDepHist {

public:

   PhiDepHist();

   PhiDepHist(const std::string & histname, const std::string & title,
              size_t nbins=10, double xmin=0, double xmax=1);

   ~PhiDepHist();

   void fill(double tangent);
   
   void fit();

   void draw();
   
   void getFitPars(std::vector<double> & pars) const;

   double entries() const;

   const std::vector<double> & pars() const;

private:

   TH1F * m_hist;

   // Don't trust the introspection mechanism for TH1F, so
   // store the histogram info in these variables.
   size_t m_nbins;
   double m_xmin;
   double m_xmax;

   static TF1 * s_fit_func;

   int m_count;

   bool m_fitted;

   std::vector<double> m_pars;
};

#endif // handoff_response_gen_PhiDepHist_h
