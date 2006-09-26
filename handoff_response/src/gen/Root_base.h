/** @file Root_base.h"
@brief declare class Root_base

$Header$

*/
#ifndef ROOT_BASE_H
#define ROOT_BASE_H


// ROOT
#include "TStyle.h"
#include "TROOT.h"
#include "TH1.h"

#include <sstream>

/** @class Root_base

A constructor that redefines various ROOT defaults,
and a place to put useful utilities. Useful to be a base class for 
classes that implement root plots

*/
class Root_base{
public:
    Root_base(){

        // redefine some defaults
        gStyle->SetPadColor(kWhite); // out with the gray
        gStyle->SetCanvasColor(kWhite);
        gStyle->SetStatColor(kWhite);

        gStyle->SetTitleBorderSize(0);
        gStyle->SetPadBorderSize(0);
        gStyle->SetPadBorderMode(0); 
        gStyle->SetOptStat("e"); // events only by default
        gStyle->SetStatBorderSize(0); // simple box for stats if present

        // this applies it to all new plots
        gROOT->ForceStyle();

        // the rainbow palette
        gStyle->SetPalette(1);
    }
    // center all titles for any plot
    void centerTitles(TH1* h)
    {
        TAxis * a;
        a = h->GetXaxis(); if(a!=0)a->CenterTitle();
        a = h->GetYaxis(); if(a!=0)a->CenterTitle();
        a = h->GetZaxis(); if(a!=0)a->CenterTitle();
    }
    
    // nice unique name -- todo: make number depend on root
    const char * unique_name(std::string rootname){
        static int n=0;
        std::stringstream t; t << rootname << (++n);
        static char buf[10];
        ::strncpy(buf, t.str().c_str(), sizeof(buf));
        return buf;
    }

};

#endif
