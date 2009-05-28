/** @file MyAnalysis.cxx
*/
#include "MyAnalysis.h"
#include "TChain.h"
#include "TFile.h"

#include <cmath>
#include <sstream>
#include <iostream>

std::string MyAnalysis::s_input_filename="";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::MyAnalysis(std::string input_file)
{
    open_input_file(input_file);
    current_time();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::~MyAnalysis()
{
    current_time();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string MyAnalysis::input_filename()
{ 
    return s_input_filename;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::open_input_file(std::string input_file)
{
    if(! input_file.empty() )  s_input_filename = input_file;
    static std::string tree_name("MeritTuple");
    std::cout << "Creating TChain(\"MeritTuple\") and adding files matching "
        << input_filename() << std::endl;

    m_input_tree = new TChain("MeritTuple");
    m_input_tree->Add(input_filename().c_str());
    if( m_input_tree==0 || m_input_tree->GetEntries()==0) {
        std::cerr << "Did not find tree \"" << tree_name << "\" in the input file" << std::endl;
        throw "did not find the TTree";
    }
    std::cout << "Tree " << m_input_tree->GetTitle() 
        << " has " << m_input_tree->GetEntries() << " entries." << std::endl;

    m_tree= m_input_tree;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::makeCutTree(const std::string& cuts, const std::vector<std::string>& branchnames, std::string filename)
{

    std::cout << "Copying cut tree, using cuts "<< cuts << std::endl;
    TFile f( (filename).c_str(), "recreate");
    m_input_tree->SetBranchStatus("*", 0);
    for(std::vector<std::string>::const_iterator i=branchnames.begin(); i!=branchnames.end(); ++i){
        m_input_tree->SetBranchStatus((*i).c_str(), 1);
    }
    m_tree = m_input_tree->CopyTree(cuts.c_str());
    m_tree->Write(); // save it
    std::cout << "Wrote " << m_tree->GetEntries() << " events to file " << filename << std::endl;
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
