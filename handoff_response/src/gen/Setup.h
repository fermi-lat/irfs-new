/** @file Setup.h
    @brief declare class Setup

    $Header$

*/
#ifndef Setup_h
#define Setup_h

#include <vector>
#include <string>
#include <iostream>

/** @class Setup
    @brief read in and parse a setup file

    A setup file is a set of strings, delimited by blank line(s)
    A line starting with a # character is ignored
    If a # character appears anywhere else, it and the characters following are ignored
*/
class Setup : public std::vector<std::string> {
public:

    Setup(int argc, char* argv[], bool verbose=false);
    /// @return the path to the folder with the setup file
    const std::string& root()const{return m_root;}

    /// @brief parse a comma-delimited list
    static void parse_list(const std::string& input, std::vector<std::string>& output);


    void dump(std::ostream& log=std::cout);
private:
    void readnames(std::string filename);

    /// remove  anything following a #
    static std::string strip_comment(std::string input);
  
    static std::string strip_blanks(std::string input);
  
    std::string m_root;
    bool m_verbose;
};

#endif
