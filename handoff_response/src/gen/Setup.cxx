/** @file Setup.cxx
@brief define class Setup

$Header$

*/

#include "Setup.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#ifdef WIN32
 #include <direct.h> // for chdir
#else
# include <unistd.h>
 int _chdir( const char*d){return chdir(d);}
 char *_ getcwd(  char *buffer,   int maxlen ){return getcwd(buffer, maxlen);}
#endif

namespace{
    std::string 
        envvar("output_file_root") //here to look if no command-ine arg
        , setupfile("setup.txt"); // file name to read and parse
}

Setup::Setup(int argc, char* argv[])
:m_root( std::string(argc>1? argv[1] : ::getenv(envvar.c_str())) )
{
    char oldcwd[128], newcwd[128];
    ::_getcwd(oldcwd, sizeof(oldcwd));

    std::cout << "Current working directory: " << oldcwd << std::endl;

    if( ::_chdir(m_root.c_str()) !=0 ){
        throw std::runtime_error("Setup: could not find folder " +m_root);
    }
    // save current working directory.
    ::_getcwd(newcwd, sizeof(newcwd));
    std::cout << "switched to " << newcwd << std::endl;
    m_root = newcwd; 
    std::string filename( setupfile );

    readnames(filename);
}
void Setup::readnames(std::string filename)
{
    std::ifstream setupfile( filename.c_str() );
    if( ! setupfile.is_open() ){
        throw std::runtime_error("Setup: could not find file \""+filename +"\" in folder "+m_root);
    }
    std::string line, current_entry;
    while( setupfile ){
        std::getline(setupfile, line);
        //std::cout << "line: " << line << std::endl;
        if( line.empty()) { // flag to swithch
            if( !current_entry.empty() ){
                push_back(current_entry);
            }
            current_entry = "";
        }else{
           if( line[0]=='@'){
                // indirection: expect first char to be @
                std::stringstream str(line.substr(1));
                std::string name;
                str >> name;
                readnames(name);
                continue;
           }else if( line[0]=='+') {
               // first char=+, add to previous (assume no current entry)
               current_entry = back(); pop_back();
               line = line.substr(1);
           }
           current_entry += strip_comment(line);
        }

    }
    if( !current_entry.empty() ) push_back(current_entry);

}

// parse a comma-delimeted list
void Setup::parse_list(const std::string& input, std::vector<std::string>& output)
{
    size_t i=0 ;
    for(; i != std::string::npos ; ){
        size_t j = input.substr(i).find_first_of(",");
        if( j==std::string::npos ){
            std::string temp( strip_blanks(input.substr(i) ) );
            if( !temp.empty() ) output.push_back(temp);
            break;
        } else{
            std::string temp( strip_blanks(input.substr(i, j)) );
            if( !temp.empty() ) output.push_back(temp);
            i += j+1;
        }
    }
}


void Setup::dump(std::ostream& log)
{
    std::copy(begin(), end(), std::ostream_iterator<std::string>(log, "\n"));
}


/// remove  anything following a #
std::string Setup::strip_comment(std::string input)
{
    if( input.empty() ) return input;
    size_t j = input.find_first_of("#");

    return j==std::string::npos? input : input.substr(0, j);
}

std::string Setup::strip_blanks(std::string input)
{
    if( input.empty() || input==" ") return "";
    size_t i = input.find_first_not_of(" "),
        j = input.substr(i).find_first_of(" ");
    return j==std::string::npos? input.substr(i) : input.substr(i, j-i+1);
}
