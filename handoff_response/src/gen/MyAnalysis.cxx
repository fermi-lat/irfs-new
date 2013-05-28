/** @file MyAnalysis.cxx
*/
#include "MyAnalysis.h"
#include "TreeWrapper.h"

#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "embed_python/Module.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::MyAnalysis(embed_python::Module& py)
:m_out(0)
{
    // get file information from input description 
    // first, file list


    py.getList("Data.files", m_files);
    std::cout << "Reading from " << m_files.size() << " filelists" << std::endl;

    // then set of info
    py.getValue("Prune.cuts", m_cuts);
    py.getValue("Prune.fileName", m_summary_filename);
    py.getList("Prune.branchNames", m_branchNames);

    current_time();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::~MyAnalysis()
{
    current_time();
    delete m_out;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::open_input_file()
{
    static std::string tree_name("MeritTuple");
    std::cout << "Creating TChain(\"MeritTuple\") and adding files matching "
        << m_summary_filename << std::endl;

    m_input_tree = new TChain("MeritTuple");
    m_input_tree->SetMaxTreeSize(500000000000LL); // 500 gigs?

    // reprocessing the original file 
    m_input_tree->Add(m_summary_filename.c_str());


    if( m_input_tree==0 || m_input_tree->GetEntries()==0) {
        std::cerr << "Did not find tree \"" << tree_name << "\" in the input file" << std::endl;
        throw "did not find the TTree";
    }
    std::cout << "Tree " << m_input_tree->GetTitle() 
        << " has " << m_input_tree->GetEntries() << " entries." << std::endl;

    m_tree= m_input_tree;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::makeCutTree()
{
    static std::string tree_name("MeritTuple");
    std::cout << "Creating TChain(\"MeritTuple\") from files" << std::endl;

    m_input_tree = new TChain(tree_name.c_str());
    m_input_tree->SetMaxTreeSize(500000000000LL); // 500 gigs?

    for( std::vector<std::string>::const_iterator sit = m_files.begin(); sit!=m_files.end(); ++sit){
        std::cout << "\t" << *sit << std::endl;
        m_input_tree->Add((*sit).c_str());
    }
    std::cout << "Copying cut tree, using cuts "<< m_cuts << std::endl;
    if( !m_summary_filename.empty() ){
        m_out = new TFile(m_summary_filename.c_str(), "recreate");
    }

    m_input_tree->SetBranchStatus("*", 0);
    for(std::vector<std::string>::const_iterator i=m_branchNames.begin(); i!=m_branchNames.end(); ++i){
        m_input_tree->SetBranchStatus((*i).c_str(), 1);
    }
    m_tree = m_input_tree->CopyTree(m_cuts.c_str());
    m_tree->Write(); // save it
    std::cout << "Wrote " << m_tree->GetEntries() << " events to file " << m_summary_filename << std::endl;

}

#include <time.h>

void MyAnalysis::current_time(std::ostream& out)
{   
    static bool first=true;
    static time_t start;
    if(first){ first=false; ::time(&start);}
    time_t aclock;
    ::time( &aclock );   
    char tbuf[25]; ::strncpy(tbuf, asctime( localtime( &aclock ) ),24);
    tbuf[24]=0;
    double deltat ( ::difftime( aclock, start) );
    out<<  "Current time: " << tbuf
        << " ( "<< deltat <<" s elapsed)" << std::endl;
}

// copy code from TCanvas::Divide, but leave room for a label at top
void MyAnalysis::divideCanvas(TCanvas & c, int nx, int ny, std::string top_title)   {
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
