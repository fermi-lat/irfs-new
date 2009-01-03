/** @file IrfAnalysis.cxx
@brief Analysis of PSf

$Header$
*/


#include "IrfAnalysis.h"
#include "PsfPlots.h"
#include "DispPlots.h"
#include "EffectiveArea.h"
#include "AeffPhiDep.h"
#include "TreeWrapper.h"
#include "embed_python/Module.h"

#include "CLHEP/Geometry/Vector3D.h"

#include "TFile.h"
#include "TTree.h"

#include <iomanip>
#include <fstream>
#include <ios>
#include <stdexcept>
namespace{
    inline static double sqr(double x){return x*x;}

}


//__________________________________________________________________________
IrfAnalysis::IrfAnalysis(std::string output_folder,int set, embed_python::Module& py) 
: MyAnalysis(py)
, m_binner(py)  // initilize the binner
, m_filename_root(output_folder+"/")
, m_set(set)
, m_setname( m_set==1? "front":"back")
, m_outputfile(std::string(set==1? "front":"back")+".root")
{

    std::string logfile;

    py.getValue("className", m_classname);    
    py.getValue("logFile", logfile);
    m_log = logfile.empty()? &std::cout : new std::ofstream(logfile.c_str(),std::ios_base::app);


    py.getValue("parameterFile", m_parameterFile);

    py.getValue("Data.generate_area", m_generate_area);
    std::vector<double> generated, logemins, logemaxes;
    py.getList("Data.generated", generated);
    py.getList("Data.logemin", logemins);
    py.getList("Data.logemax", logemaxes);
    for( size_t i=0; i<generated.size(); ++i){
      normalization().push_back(Normalization(static_cast<int>(generated[i]), logemins[i], logemaxes[i]));
    }

    setName(m_classname+"/"+m_setname); // for access by the individual guys
    current_time(out());
    out() << "Event class is " << m_classname << std::endl;

    project();
}

//__________________________________________________________________________
void IrfAnalysis::project() 
{

    open_input_file();
    TFile*   m_hist_file= new TFile(summary_filename().c_str(), "recreate"); // for the histograms
    std::cout << " writing irf summary plots to " << summary_filename() << std::endl;
    out() << " writing irf summary plots to " << summary_filename() << std::endl;
    out() << " selecting " ;
    switch (m_set) {
case 0: out() << "all events"; break;
case 1: out() << "front events"; break;
case 2: out() << "back events"; break;
    }
    std::cout << std::endl;
    //---- declare the PSF plots------
    //---------------------------

    m_psf = new PsfPlots(*this, out());
    m_disp = new DispPlots(*this, out());
    m_aeff = new EffectiveArea(*this, out());
    m_phi_dep = new AeffPhiDep(*this);

    std::cout << "Selecting columns in tree " << tree().GetName() << std::endl;
    TreeWrapper mytree(&tree()); // sets current TTree
    TreeWrapper::Leaf // create TLeaf-wrappers from the current TTree 
        McEnergy("McEnergy")
        , CTBBestEnergy("CTBBestEnergy")

        , McXDir("McXDir")
        , McYDir("McYDir")
        , McZDir("McZDir")

        , fitxdir("CTBBestXDir")
        , fitydir("CTBBestYDir")
        , fitzdir("CTBBestZDir")

        , Tkr1FirstLayer("Tkr1FirstLayer")
        , EvtRun("EvtRun" ) // to count runs
        ;
    int lastrun(0), selected(0), total(0), nruns(0);
    double minlogE(1e6), maxlogE(0);
    double minzdir(1), maxzdir(-1);

    for( TreeWrapper::Iterator it = mytree.begin(); it!=mytree.end(); ++it, ++total) {
        if( EvtRun != lastrun ) { ++nruns; lastrun = EvtRun;}
        double logE( log10(McEnergy) );
        if( logE< minlogE) minlogE = logE;
        if( logE > maxlogE) maxlogE = logE;
        if( McZDir< minzdir) minzdir = McZDir;
        if( McZDir> maxzdir) maxzdir = McZDir;

        bool front =Tkr1FirstLayer>5;
        // make event selection: all, front, or back
        if(  m_set==1&& !front || m_set==2 && front ) continue; 

        ++selected;

        // calculate theta, phi components of the error
        HepVector3D 
            mc_dir(McXDir, McYDir, McZDir), 
            fit_dir(fitxdir, fitydir, fitzdir),
            mc_error(mc_dir-fit_dir),
            zhat(0,0,1),
            phi_hat=zhat.cross(mc_dir).unit(),
            theta_hat = phi_hat.cross(mc_dir).unit();

        double phi_err = mc_error*phi_hat,
            theta_err = mc_error*theta_hat,
            diff =sqrt(sqr(theta_err)+ sqr(phi_err)) ;

        // choose one of the following
        double measured_energy=CTBBestEnergy, 
            mc_energy = McEnergy,
            //ratio =measured_energy/mc_energy,
            dsp = measured_energy/mc_energy-1;
#if 1
        m_psf->fill(diff, McEnergy, McZDir, front);
#else
        m_psf->fill(diff, CTBBestEnergy, McZDir, front);
#endif

        m_disp->fill(dsp, McEnergy, McZDir, front);

        m_aeff->fill( mc_energy, McZDir, front, total);

        m_phi_dep->fill(McXDir, McYDir, McEnergy, McZDir);
    }
    out() << "\nFound " << nruns <<" run numbers" 
        << " and " << selected<< "/" <<  mytree.size() << " events" <<  std::endl;
    out() << "Log energy range: " << minlogE << " to " << maxlogE << std::endl;
    out() << "McZDir range: " << minzdir << " to " << maxzdir << std::endl;

    m_hist_file->Write();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IrfAnalysis::fit(bool make_plots,  std::string output_type)
{
    m_psf->fit(); 
    m_disp->fit();
    m_phi_dep->fit();

    m_psf->summarize();
    m_disp->summarize();
    m_aeff->summarize();
    if(make_plots) m_psf->draw(std::string(output_file_root()+m_setname+"_psf."+output_type));
    if(make_plots) m_disp->draw(std::string(output_file_root()+m_setname+"_disp."+output_type));
    if(make_plots) m_aeff->draw(std::string(output_file_root()+m_setname+"_aeff."+output_type));
    if (make_plots) {
       m_phi_dep->draw(output_file_root() + m_setname + 
                       "_phi_dep." + output_type);
    }
    if( !parfile().empty()) {
        writeFitParameters( output_file_root() +parfile());
        //  tabulate(output_file_root() +parfile,  m_setname);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IrfAnalysis::writeFitParameters(std::string outputFile)
{
    // Create or update the new Root file.
    TFile* file = new TFile(outputFile.c_str(), "UPDATE");

    // add directory if not already there
    TDirectory* td = file->mkdir((m_classname).c_str());
    if( td!=0){  // know we'll need both
        td->mkdir("front");
        td->mkdir("back");
    }

    bool check = file->cd((std::string("/")+m_classname+"/"+m_setname).c_str());
    if( ! check) throw("could not cd to /"+m_classname+"/"+m_setname);

    out() << "Writing summaries to " << outputFile << "/"<< m_classname<<"/"<<m_setname << std::endl;

#if 0 // make this optional, to study the parameter values
    // Create the TTree.
    makeParameterTuple();

#endif
    // now make 2-D histograms of the values
    m_psf->fillParameterTables();
    m_disp->fillParameterTables();

    m_aeff->fillParameterTables();

    m_phi_dep->fillParameterTables();

    delete file;
    current_time(out());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IrfAnalysis::makeParameterTuple()
{
#if 0 ///@todo clean up this mess
    TTree* tree = new TTree("parameters", "table of parameter values");

    double entries;
    //double  aeff;
    double energy, anglebin;

    tree->Branch("energy",   &energy,  "energy/D");
    tree->Branch("anglebin", &anglebin,"anglebin/D");
    tree->Branch("entries",  &entries, "entries/D"); 
    //tree->Branch("aeff",     &aeff,    "aeff/D");

    // make branches to store psf fit parameters
    int psf_npars=PointSpreadFunction::npars();
    std::vector<double> psf_params(psf_npars);
    for( int i=0; i< psf_npars; ++i){
        tree->Branch(PointSpreadFunction::parname(i),
            &psf_params[i], (std::string(PointSpreadFunction::parname(i))+"/D").c_str());
    }

    // make branches to store dispersion fit parameters (except for normalization)
    int disp_npars=Dispersion::npars();
    std::vector<double> disp_params(disp_npars);

    for( int i=0; i< disp_npars; ++i){
        tree->Branch(Dispersion::parname(i),
            &disp_params[i], (std::string(Dispersion::parname(i))+"/D").c_str());
    }

    // loop through the two sets of histograms, extracting info from each
    int index(0);
    DispPlots::DispList::const_iterator disp_it = m_disp->hists().begin();
    PsfPlots::PSFlist::const_iterator psf_it = m_psf->hists().begin();
    for( ; psf_it!=m_psf->hists().end();  ++psf_it, ++disp_it)   {

        (*psf_it).getFitPars(psf_params);
        (*disp_it).getFitPars(disp_params);
        entries = (*psf_it).entries();
        //  aeff = entries* aeff_per_event();
        anglebin = index/IrfAnalysis::angle_bins;
        energy = IrfAnalysis::eCenter(index++);

        tree->Fill();
    }
    tree->Write();
#endif

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IrfAnalysis::tabulate(std::string filename)
{
    std::string setname(m_classname);

    out() << "\nFit results, from file \"" 
        << filename << "\" data set \"" << setname << "\"" <<std::endl;

    TreeWrapper mytree(filename, setname.c_str());

    std::vector<TreeWrapper::Leaf> fitpar;

    std::vector<std::string > names; // names to label groups
    fitpar.push_back(mytree.leaf("entries"));
    names.push_back("effective area (m^2)");

    // skip norm, since not saved
    for( int j=1; j<PointSpreadFunction::npars(); ++j){
        fitpar.push_back(mytree.leaf(PointSpreadFunction::parname(j)));
        names.push_back(PointSpreadFunction::parname(j));
    }
    for( int j=1; j< Dispersion::npars(); ++j) {
        fitpar.push_back(mytree.leaf(Dispersion::parname(j)));
        names.push_back(Dispersion::parname(j));
    }

    // define matrix that will be filled with selected columns and transposed below
    std::vector< std::vector<double> > p(names.size());

    int i=0;
    for( TreeWrapper::Iterator it = mytree.begin(); it!=mytree.end(); ++it, ++i){
        //   double energy = IrfAnalysis::eCenter(it.index()%8);
#if 0// obsolete--fix if use this again!        
        p[0].push_back(fitpar[0]* aeff_per_event() );
#else
        throw std::runtime_error("need to implement aeff per event");
#endif
        for(unsigned int j=1; j<fitpar.size(); ++j) p[j].push_back(fitpar[j]);

#if 0 // just columns here, check input data
        out() << (i) << "\t";
        std::copy(par.begin(),par.end(), std::ostream_iterator<double>(out,"\t"));
        out() << std::endl;
#endif
    }

    // now rearrage: expect 54 entries if 6 energy bins, or 
    int n(mytree.size());
    int energy_bins = n==54? 6 : 8;
    int angle_bins = 9;  // 0-7 are the 8 costheta bins, 8 is integrated fit

    const char * enames[]={"32 MeV", "100 MeV", "316 MeV", "1 GeV", "3.2 GeV", "10 GeV", "32 GeV", "100 GeV"};

    out() << std::setprecision(4);

    for(unsigned int j = 0; j< names.size(); ++j){
        out() << "\n" << names[j] << std::endl;
        for( int k=0; k<energy_bins; ++k){ // energy bins as columns
            out() << enames[k];
            for (int l=0; l<angle_bins; ++l){ // angular bins as rows
                out() << "\t" << p[j][k+energy_bins*l];
            }
            out() << "\n";
        }
    }
    out() << std::endl;
}

