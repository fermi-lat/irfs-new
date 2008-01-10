/** @file prune.cxx
@brief create pruned output file

$Header$
*/

#include "../MyAnalysis.h"
#include "../Setup.h"

#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <iterator>


//_____________________________________________________________________________

int main(int argc, char* argv[]){
    int ret=0;
    try {

        Setup s(argc, argv);

        //std::cout << "\nstrings: " << std::endl;   s.dump(std::cout);  std::cout << "\nNames: \n\t";
        std::vector<std::string> names;
        s.parse_list(s[2], names);
        //std::copy(names.begin(), names.end(), std::ostream_iterator<std::string>(std::cout, "\n\t"));  std::cout << std::endl;

        MyAnalysis copier(s[0]); // makes a copy of the good gammas
        std::string cut_filename( "goodEvent.root" );

        copier.makeCutTree( s[1], names, cut_filename);

    }catch( const std::exception& e){
        std::cerr << "Caught exception "<< e.what() << std::endl;
        ret=1;
    }
    return ret;
}
