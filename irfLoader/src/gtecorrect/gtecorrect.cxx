/**
 * @file gtecorrect.cxx
 * @brief Apply event by event energy correction
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "facilities/Util.h"

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "irfInterface/IrfsFactory.h"

#include "irfLoader/Loader.h"

class EnergyCorrect : public st_app::StApp {

public:

   EnergyCorrect() : st_app::StApp(),
                     m_pars(st_app::StApp::getParGroup("gtecorrect")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in EnergyCorrect constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~EnergyCorrect() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }  
   }

   virtual void run();
   virtual void banner() const;

private:

   st_app::AppParGroup & m_pars;

   std::string par(const std::string & key) const;

   std::string m_inputFile;
   std::string m_outputFile;

   std::vector<irfInterface::Irfs *> m_irfs;

   static std::string s_cvs_id;

   void copyTable(const std::string & extension, 
                  bool applyCorrection=false) const;
   void correctEnergy(tip::Table::Record & row) const;

};

std::string EnergyCorrect::s_cvs_id("$Name$");

st_app::StAppFactory<EnergyCorrect> myAppFactory("gtecorrect");

void EnergyCorrect::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

std::string EnergyCorrect::par(const std::string & key) const {
   std::string value = m_pars[key.c_str()];
   return value;
}

void EnergyCorrect::run() {
   m_pars.Prompt();
   m_pars.Save();

   m_inputFile = par("infile");
   facilities::Util::expandEnvVar(&m_inputFile);
   st_facilities::Util::file_ok(m_inputFile);

   m_outputFile = par("outfile");
   facilities::Util::expandEnvVar(&m_outputFile);

   bool clobber = m_pars["clobber"];
   st_stream::StreamFormatter formatter("EnergyCorrect", "run", 2);
   if (!clobber && st_facilities::Util::fileExists(m_outputFile)) {
      formatter.err() << "Output file, " << m_outputFile 
                      << ", already exists,\n"
                      << "and you have specified 'clobber' as 'no'.\n"
                      << "Please provide a different file name." 
                      << std::endl;
      std::exit(1);
   }
   
   irfInterface::IrfsFactory & factory(*irfInterface::IrfsFactory::instance());

   irfLoader::Loader::go();
   std::map<std::string, std::vector<std::string> > respIds
      = irfLoader::Loader::respIds();
   const std::vector<std::string> & irfNames(respIds[par("irfs")]);

   for (size_t i = 0; i < irfNames.size(); i++) {
      m_irfs.push_back(factory.create(irfNames.at(i)));
   }

   tip::IFileSvc::instance().createFile(m_outputFile, m_inputFile);

   bool applyCorrection(true);
   copyTable(par("evtable"), applyCorrection);
   copyTable("gti");
   formatter.info() << "Done." << std::endl;

   st_facilities::FitsUtil::writeChecksums(m_outputFile);
}

void EnergyCorrect::copyTable(const std::string & extension,
                              bool applyCorrection) const {
   const tip::Table * inputTable 
      = tip::IFileSvc::instance().readTable(m_inputFile, extension);
   
   tip::Table * outputTable 
      = tip::IFileSvc::instance().editTable(m_outputFile, extension);

   outputTable->setNumRecords(inputTable->getNumRecords());

   tip::Table::ConstIterator inputIt = inputTable->begin();
   tip::Table::Iterator outputIt = outputTable->begin();

   tip::ConstTableRecord & input = *inputIt;
   tip::Table::Record & output = *outputIt;

   long npts(0);

   for (; inputIt != inputTable->end(); ++inputIt) {
      output = input;
      if (applyCorrection) {
         correctEnergy(output);
      }
      ++outputIt;
      npts++;
   }
// Resize output table to account for filtered rows.
   outputTable->setNumRecords(npts);

   delete inputTable;
   delete outputTable;
}

void EnergyCorrect::correctEnergy(tip::Table::Record & row) const {
   double energy;
   row["ENERGY"].get(energy);
   double theta;
   row["THETA"].get(theta);
   double phi;
   row["PHI"].get(phi);
   double time;
   row["TIME"].get(time);
   int eventClass;
   row["EVENT_CLASS"].get(eventClass);

   double trueEnergy = 
      m_irfs.at(eventClass)->edisp()->meanTrueEnergy(energy, theta, phi, time);

   row["ENERGY"].set(trueEnergy);
}
