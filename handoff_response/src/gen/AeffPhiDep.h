/**
 * @file AeffPhiDep.h
 * @brief Class to manage fits of phi-dependence of the effective area.
 * @author J. Chiang
 *
 * $Header$
 */

#include <string>
#include <vector>

#include "PhiDepHist.h"

class IrfAnalysis;
class IrfBinner;

/**
 * @class AeffPhiDep
 */

class AeffPhiDep {

public:
   
   AeffPhiDep(IrfAnalysis & irf);

   ~AeffPhiDep();

   void fill(double mc_xdir, double mc_ydir, double energy, double costheta);

   void fit();

   void summarize();

   void draw(const std::string & psfile);

   void fillParameterTables();

private:

   IrfAnalysis & m_irf;
   
   const IrfBinner & m_binner;

   std::vector<PhiDepHist> m_hists;

};

