#include <iostream>
#include <sstream>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TTree.h"
#include "TVirtualPad.h"

int main() {
   TFile root_file("goodEvent.root");
   TTree * merit = (TTree*)(root_file.Get("MeritTuple"));
   
   size_t astepno(6);
   double amin(0.2), amax(1.0), astep((amax - amin)/astepno);
   
   size_t lestepno(12);
   double lemin(1.25), lemax(5.75), lestep((lemax - lemin)/lestepno);

   TF1 fitf("fitf", "1 + [0]*x^[1]", 0, 0.9);
   fitf.SetParameter(0, 1.2);
   fitf.SetParameter(1, 2);

   TH2F par1map("par1map", "par1map", lestepno, lemin, lemax, 
                astepno, amin, amax);
   TH2F par2map("par2map", "par2map", lestepno, lemin, lemax, 
                astepno, amin, amax);

   TCanvas canvas("canvas", "aeff", 600, 800);
   canvas.Divide(3, 4);

   std::string psfile("foo.ps");
   
   for (size_t ai(0); ai < astepno; ai++) {
      double as0(ai*astep + amin);
      double as1(as0 + astep);
      for (size_t lei(0); lei < lestepno; lei++) {
         size_t i(ai*lestepno + lei + 1);
         size_t ipad(lei + 1);
         canvas.cd(ipad);
         double les0(lei*lestep + lemin);
         double les1(les0 + lestep);
         std::ostringstream cuts;
         cuts << "log10(McEnergy)>" << les0
              << " && log10(McEnergy)<" << les1
              << " && McZDir>" << -as1
              << " && McZDir<" << -as0;
         std::cout << cuts.str() << std::endl;
         std::string my_var;
         if (i == 1) {
            my_var = "abs(abs(atan(McYDir/McXDir))*2./pi-0.5)*2>>htemp(10,0,1)";
         } else {
            my_var = "abs(abs(atan(McYDir/McXDir))*2./pi-0.5)*2";
         }
         merit->Draw(my_var.c_str(), cuts.str().c_str());
         long nevents(merit->GetSelectedRows());
         if (nevents > 10) {
            TVirtualPad * pad(canvas.GetPad(ipad));
            TH1F * hist = (TH1F*)(pad->GetPrimitive("htemp"));
            double lowval = (hist->GetBinContent(1)+hist->GetBinContent(2) +
                             hist->GetBinContent(3)+hist->GetBinContent(4))/4.;
            if (lowval > 0) {
               for (size_t jb(1); jb < 11; jb++) {
                  hist->SetBinContent(jb, hist->GetBinContent(jb)/lowval);
               }
            }
            hist->Draw();
            fitf.SetParameter(1, 2);
            fitf.SetParameter(2, 1);
            hist->Fit(&fitf);
            double psq(fitf.GetParameter(0));
            double psx(fitf.GetParameter(1));
            par1map.Fill(les0 + lestep/2, as0 + astep/2, psq);
            par2map.Fill(les0 + lestep/2, as0 + astep/2, psx);
         }
      }
      if (ai == 0) {
         canvas.Print((psfile + "(").c_str());
      } else {
         canvas.Print(psfile.c_str());
      }         
   }

   TCanvas minicvas1("minicvas1", "PAR1", 500, 350);
   par1map.Draw("COLZ");
   par1map.SetXTitle("log10(McEnergy)");
   par1map.SetXTitle("McZDir");
   minicvas1.Print(psfile.c_str());

   TCanvas minicvas2("minicvas2", "PAR2", 500, 350);
   par2map.Draw("COLZ");
   par2map.SetXTitle("log10(McEnergy)");
   par2map.SetXTitle("McZDir");
   minicvas2.Print((psfile + ")").c_str());

   TFile file("phi_params.root", "recreate");
   par1map.Write();
   par2map.Write();
}
