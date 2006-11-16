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

        std::string cut_filename( "goodEvent.root" );

        MyAnalysis copier(s[0], cut_filename); // makes a copy of the good gammas

        copier.makeCutTree( s[1], names);

    }catch( const std::exception& e){
        std::cerr << "Caught exception "<< e.what() << std::endl;
        ret=1;
    }
    return ret;
}

/** @page prune The prune application

Use the Setup class to process arguments, which are assumed to be just a path to a folder containing a 
file "setup.txt". 

Expect this to define three parameters:

- the full path to the original ROOT file
- a list of cuts to apply
- a list of tuple names to enable

The created file is "goodEvent.root" in the same folder.

*/
