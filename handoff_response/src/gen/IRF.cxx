/** @file IRF.cxx
    @brief implementation of class IRF

  $Header$
  */
  
#include "IRF.h"
#include <iostream>
#include "TPaveLabel.h"
#include "TDatime.h"
#include "TROOT.h"
#include "TCanvas.h"

#include <cmath>

double IRF::s_generated_area(6.0);

// defines binning 
double IRF::logemin   = 1.25;  // low side of first bin
#if 1
double IRF::logedelta = 0.5;  // 2 per decade
int    IRF::energy_bins = 9;  // number of bins  
int    IRF::angle_bins = 8;
double IRF::deltaCostheta = 0.1; // angular bin size
#else
double IRF::logedelta = 0.25;  // 4 per decade
int    IRF::energy_bins = 16;  // number of bins
int    IRF::angle_bins = 12;
double IRF::deltaCostheta = 0.05; // angular bin size
#endif


IRF::IRF(std::string output_folder, std::string summary_root_filename)
: m_output_file_root(output_folder)
, m_summary_filename( std::string( !output_folder.empty()? output_folder+   "/": "")+summary_root_filename)
, m_ymin(0)
, m_ymax(1)
{
    m_ebins = energy_bins;
    m_abins = angle_bins;
    for( int i=0; i<=angle_bins; ++i){ angles.push_back( int(acos(1.-deltaCostheta*i)*180/M_PI+0.5)); }
}

// copy code from TCanvas::Divide, but leave room for a label at top
void IRF::divideCanvas(TCanvas & c, int nx, int ny, std::string top_title)   {
        int color=10;
        double xmargin=0, ymargin=0, top_margin=0.08; 
        c.SetFillColor(color);
        if (nx <= 0) nx = 1;
        if (ny <= 0) ny = 1;
        std::string temp(top_title+" " + TDatime().AsString());

        TPaveLabel*  title = new TPaveLabel( 0.1,0.95, 0.9,0.99, temp.c_str());
         title->SetFillColor(10);
         title->SetBorderSize(0);
         title->Draw();

        Int_t ix,iy;
        Double_t x1,y1,x2,y2;
        Double_t dy = 1/Double_t(ny);
        Double_t dx = 1/Double_t(nx);
        TPad *pad;
        char *tname = new char [strlen(c.GetName())+6];
        Int_t n = 0;
        if (color == 0) color = c.GetFillColor();
        for (iy=0;iy<ny;iy++) {
            y2 = 1 - iy*dy - ymargin;
            y1 = y2 - dy + 2*ymargin;
            if (y1 < 0) y1 = 0;
            if (y1 > y2) continue;
            for (ix=0;ix<nx;ix++) {
                x1 = ix*dx + xmargin;
                x2 = x1 +dx -2*xmargin;
                if (x1 > x2) continue;
                n++;
                sprintf(tname,"%s_%d",c.GetName(),n);
                pad = new TPad(tname,tname,x1,y1*(1-top_margin),x2,y2*(1-top_margin),color);
                pad->SetNumber(n);
                pad->Draw();
            }
        }
        delete [] tname;
        c.Modified();
    }
