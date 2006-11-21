/** @file MyAnalysis.cxx
*/
#include "MyAnalysis.h"
#include "TreeWrapper.h"

#include "TChain.h"
#include "TFile.h"

#include "Setup.h"

#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>

std::string MyAnalysis::s_input_filename="";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::MyAnalysis(std::string input_file, std::string cut_filename)
: m_summary_filename(cut_filename)
, m_out(0)
{
    open_input_file(input_file);
    current_time();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::~MyAnalysis()
{
    current_time();
    delete m_out;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string MyAnalysis::input_filename()
{ 
    return s_input_filename;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::open_input_file(std::string input_files)
{
    if(! input_files.empty() )  s_input_filename = input_files;
    static std::string tree_name("MeritTuple");
    std::cout << "Creating TChain(\"MeritTuple\") and adding files matching "
        << input_filename() << std::endl;

    m_input_tree = new TChain("MeritTuple");

    if( !input_files.empty() ){
    std::vector<std::string> filelist;
    Setup::parse_list(input_files, filelist);
    int count(0), events_generated,  entries, previous(0), current;
    double logemin, logemax;

    for(std::vector<std::string>::const_iterator it = filelist.begin(); it!=filelist.end(); ++it,++count){
        const std::string& entry = *it;
        switch (count%4){
            case 0:
                // assume, if more than one entry, that there are numbers for size
                std::cout << "Adding file(s) " << entry ;        
                m_input_tree->Add(entry.c_str());
                current =static_cast<int>(m_input_tree->GetEntries());
                std::cout << " total entries now " << current << std::endl;
                entries = current-previous;
                previous = current;
                break;
            case 1:  //should be number of events
                std::cout << "\t events generated: " << entry;
                events_generated = static_cast<int>(::atof(entry.c_str()));
                break;
            case 2:
                std::cout << "\tloge limits: from " << entry; 
                logemin = ::atof(entry.c_str());
                break;
            case 3:
                std::cout << "to " << entry << std::endl;
                logemax = ::atof(entry.c_str());
                m_norm.push_back(Normalization(events_generated, logemin, logemax, current));
                break;

        }
    }
    }else{
        // reprocessing the original file 
        m_input_tree->Add(s_input_filename.c_str());
        TChain norms("norm");
        norms.Add(s_input_filename.c_str());
        if( norms.GetEntries()==0) throw std::invalid_argument("MyAnalysis::open_input_file -- did not find the normaliztion data");
        TreeWrapper tnorm(&norms);
        TreeWrapper::Leaf generated("generated"), logemin("logemin"), logemax("logemax"), entries("entries");
        for( TreeWrapper::Iterator it = tnorm.begin(); it!=tnorm.end(); ++it){
            m_norm.push_back(Normalization(generated, logemin, logemax, entries));
        }
    }


    if( m_input_tree==0 || m_input_tree->GetEntries()==0) {
        std::cerr << "Did not find tree \"" << tree_name << "\" in the input file" << std::endl;
        throw "did not find the TTree";
    }
    std::cout << "Tree " << m_input_tree->GetTitle() 
        << " has " << m_input_tree->GetEntries() << " entries." << std::endl;

    m_tree= m_input_tree;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::makeCutTree(const std::string& cuts, const std::vector<std::string>& branchnames)
{

    std::cout << "Copying cut tree, using cuts "<< cuts << std::endl;
    if( !m_summary_filename.empty() ){
        m_out = new TFile(m_summary_filename.c_str(), "recreate");
    }

    m_input_tree->SetBranchStatus("*", 0);
    for(std::vector<std::string>::const_iterator i=branchnames.begin(); i!=branchnames.end(); ++i){
        m_input_tree->SetBranchStatus((*i).c_str(), 1);
    }
    m_tree = m_input_tree->CopyTree(cuts.c_str());
    m_tree->Write(); // save it
    std::cout << "Wrote " << m_tree->GetEntries() << " events to file " << m_summary_filename << std::endl;

    // now create and save the normalization tuple

    if( m_norm.size()>0) {
        TTree*  norm = new TTree("norm", "normalization");
        int generated, entries; 
        double logemin,logemax;

        norm->Branch("generated",&generated, "generated/I");
        norm->Branch("logemin", &logemin, "logemin/D");
        norm->Branch("logemax", &logemax, "logemax/D");
        norm->Branch("entries", &entries, "entries/I");
        for( std::vector<Normalization>::const_iterator it = m_norm.begin(); it!=m_norm.end(); ++it){
            generated = it->generated();
            logemin   = it->logemin();
            logemax   = it->logemax();
            entries   = it->entries();
            norm->Fill();
        }
        norm->Write();

        std::cout << "Wrote " << m_norm.size() << " entries to the normalization tuple" << std::endl;
    }else{
        std::cout << "No normalization info: tuple not created"<< std::endl;
    }
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
