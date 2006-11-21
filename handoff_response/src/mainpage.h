/** @file mainpage.h
@brief doxygen mainpage

 $Header$
*/
/*! \mainpage package handoff_response

   \authors Toby Burnett, J. Chiang

  IRF analysis package, combining generation of the fit parameters, 
  and an interface of the resulting functions to irfInterface.

  - generation: See the folder gen, starting with the class IrfAnalysis, and the applications
  prune and makeirf.

  The applications expect to find a path to a folder containing a file "setup.txt". It can
  either be an argument in the command line, or  the environment variable "output_file_root". It is parsed by
  the class Setup, which then returns the three parameters needed: the input file, the list of
  cuts (formatted as a cut string), and a list of variable names for the copy.

  The output file is "goodEvents.root", in the same folder.
  

  - functions

  A global function loadirfs() examines a ROOT file 
  (default is $HANDOFF_RESPONSEROOT/data/parameters.root).
  In this file, each top level TDirectory object has the name of an event class. Below  it we expect 
  to find TDirectory entries for "front" and "back". At this lowest level, 
  there are TH2F tables of  parameters for the functions themselves, 
  and one for the effective area.

  The ROOT dependence is encapsulated in a RootEventClassList class for extracting the
  class names, and a RootEval subclass of the abstract IrfEval class.


    <hr>
  \section notes release notes
  release.notes
  \section requirements requirements
  \include requirements

*/




















*/