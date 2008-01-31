/** @file reformat.cxx  
@brief Analysis of PSf

$Header$
*/

#include "../TreeWrapper.h"
#include "../Setup.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <cmath>
#include <sstream>
#include <iomanip>

/**@class Reformat
@brief create text table for convenience of determining scanning-mode 

Want a table that looks like this, compiled for the 0-66 deg fits.

@verbatim
Original file: F:/glast/data/DC2/allGamma-rep-GR-v7r3p4_concat_CTnew.root
Cuts:          CTBCORE>0.35&& CTBBestEnergyProb>0.35&& CTBGAM>0.50&& (GltWord&10)>0 && (GltWord!=35)&& (FilterStatus_HI==0)&& (CalEnergyRaw>5&&CalCsIRLn>4) && ! ( ( (( (AcdCornerDoca>-5&&AcdCornerDoca<50&&CTBTkrLATEdge<100) || ((AcdActiveDist3D>0 || AcdRibbonActDist>0) &&Tkr1SSDVeto<2))        ||(((min(abs(Tkr1XDir),abs(Tkr1YDir)) < .01 && Tkr1DieEdge < 10 && AcdActiveDist3D > 0 && AcdActDistTileEnergy > .2) || (Tkr1SSDVeto < 7 && AcdActiveDist3D > -3 && AcdActDistTileEnergy > .15) || ( AcdActiveDist3D >(-30 + 30*(Tkr1FirstLayer-2)))) && CTBBestEnergy > 5000))||((Tkr1FirstLayer - Tkr2FirstLayer) < 0 && Tkr2FirstLayer > 4 && Tkr2TkrHDoca>100))||(CTBBestEnergy>5000 && (((CalTransRms-1.5)*Tkr1ToTTrAve)<5)      )   )
	front			back		
logE	sigma	gamma	accept	sigma	gamma	accept
1.5	0.283	1.89	0.000588	0.356	1.88	0.0164	
2	0.438	2.48	0.0867	0.388	1.83	0.209	
2.5	0.369	2.02	0.376	0.345	1.65	0.281	
3	0.373	2.41	0.85	0.3	1.59	0.718	
3.5	0.421	2.81	0.932	0.336	2.04	0.843	
4	0.477	2.77	0.912	0.382	2.23	0.856	
4.5	0.529	2.79	0.931	0.398	2.24	0.891	
5	0.496	2.36	0.901	0.414	2.69	0.885	

@endverbatim
*/
class Reformat {
public:
    ///@param filepath path to  root file
    ///@param out output stream
    Reformat(std::string filepath, std::ostream& out)
        :m_filepath(filepath)
        , m_filename("../parameters.root")
        , m_out(&out)
    {
        set_logE();  
        
        // infer class name from last element of full path
        unsigned int k = filepath.find_last_of('\\');
        if( k == std::string::npos )  k = filepath.find_last_of('/');
        std::string classname = filepath.substr(k+1);

        add(classname+"/front");
        add(classname+"/back");
        dump();

    }
    void set_logE(){
        // setup table, 8 rows <===== should be dynamic!
        m_title2 << "logE";
        for( int i=0; i<8; ++i){
            double logE(1.5+0.5*i);
            std::vector<double>row;
            row.push_back(logE);
            m_table.push_back(row);
        }
    }
    void add(std::string setname)
    {
        TreeWrapper tree(m_filename, setname+ "/parameters");
        assert (tree.size()==72); // expect 72 = 9*8 entries, use last 8
        static int first(64);     // first entry corresponding to 0-66 degrees.

        m_title1 << "\t" << setname <<"\t\t";
        m_title2 << "\tsigma\tgamma\taccept";
  //      out() << "parameters for set "<< setname << std::endl;
        
        TreeWrapper::Leaf //energy("energy")
             sigma("sigma"), aeff("aeff"), gamma("gcore");
       
        int j(0);
        for(TreeWrapper::Iterator it=tree.begin(first); it!=tree.end(); ++it,++j){
            //int i( it.index());
    //        out() << i<< "\t" << sigma << "\t" << gamma << "\t" << aeff<< std::endl;
          //  double loge = m_table[j][0], checklog(log10(energy));
            m_table[j].push_back(sigma);
            m_table[j].push_back(gamma);
            static double delta_costheta(0.1);
            m_table[j].push_back(aeff * delta_costheta * 2*M_PI);// convert from summed effective areas to acceptance
        }

    }
    void dump()
    {

        out() << m_title1.str() << std::endl; 
        out() << m_title2.str() << std::endl; 
        out() << std::setprecision(3);
        for(int i=0; i<8; ++i){
            const std::vector<double>&row = m_table[i];
            for( std::vector<double>::const_iterator irow=row.begin(); irow!=row.end();++irow){
                out() << (*irow) << "\t";
            }
            out() << std::endl;
        }
    }

private:
    std::stringstream m_title1, m_title2;
    std::vector<std::vector<double> >m_table;
    std::ostream& out(){return *m_out;}
    std::string m_filepath;
    std::string m_filename;
    std::ostream* m_out;
};

//_____________________________________________________________________________

int main(int argc, char* argv[]){
    int ret=0;
    try {

        Setup s(argc, argv);  // will cd to current folder.
#if 1
        std::string summary("summary.prn");
        std::cout << "Reformating output to file "<< summary << std::endl;
        std::ofstream out(summary.c_str());
#else
        std::ostream& out = std::cout;
#endif
        out << "Original file: " << s[0] << std::endl;
        out << "Cuts:          " << s[1] << std::endl;


        Reformat(s.root(), out);

    }catch( const std::exception& e){
        std::cerr << "Caught exception "<< e.what() << std::endl;
        ret=1;
    }
    return ret;
}
