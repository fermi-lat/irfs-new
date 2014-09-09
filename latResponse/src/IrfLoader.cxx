/**
 * @brief Implementation for post-handoff review IRFs
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "facilities/commonUtilities.h"
#include "facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_stream/StreamFormatter.h"

#include "st_facilities/Env.h"
#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "irfInterface/IrfRegistry.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"

#include "irfUtil/HdCaldb.h"

#include "latResponse/IrfLoader.h"

#include "Aeff.h"
#include "AeffEpochDep.h"
#include "CaldbDate.h"
#include "Edisp.h"
#include "Edisp2.h"
#include "Edisp3.h"
#include "EdispEpochDep.h"
#include "EfficiencyFactor.h"
#include "EfficiencyFactorEpochDep.h"
#include "Psf.h"
#include "Psf2.h"
#include "Psf3.h"
#include "PsfEpochDep.h"
namespace latResponse {

bool IrfLoader::s_interpolate_edisp(true);

IrfLoader::IrfLoader() 
   : m_hdcaldb(new irfUtil::HdCaldb("GLAST", "LAT")) {
   read_caldb_indx();
   readCustomIrfNames();
}

IrfLoader::~IrfLoader() {
   delete m_hdcaldb;
}

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   for (size_t i(0); i < m_caldbNames.size(); i++) {
      const std::string & irfName(m_caldbNames.at(i));
      const std::vector<std::string> & sub_classes(subclasses(irfName));
      std::vector<std::string> classNames;
      for (size_t j(0); j < sub_classes.size(); j++) {
         classNames.push_back(sub_classes.at(j) + "::FRONT");
         registry.registerEventClass(classNames.back(), classNames.back());
         classNames.push_back(sub_classes.at(j) + "::BACK");
         registry.registerEventClass(classNames.back(), classNames.back());
      }
      registry.registerEventClasses(irfName, classNames);
   }
   for (size_t i(0); i < m_customIrfNames.size(); i++) {
      const std::string & irfName(m_customIrfNames.at(i));
      const std::vector<std::string> & sub_classes(subclasses(irfName));
      std::vector<std::string> classNames;
      for (size_t j(0); j < sub_classes.size(); j++) {
         classNames.push_back(sub_classes.at(j) + "::FRONT");
         registry.registerEventClass(classNames.back(), classNames.back());
         classNames.push_back(sub_classes.at(j) + "::BACK");
         registry.registerEventClass(classNames.back(), classNames.back());
      }
      registry.registerEventClasses(m_customIrfNames.at(i), classNames);
   }
// kluge to allow for HANDOFF irfs for backwards compatibility with old
// handoff_response implementation
   if (registry.respIds().find("PASS4") != registry.respIds().end()) {
      std::vector<std::string> classNames;
      classNames.push_back("PASS4::FRONT");
      classNames.push_back("PASS4::BACK");
      registry.registerEventClasses("HANDOFF", classNames);
      registry.registerEventClass("HANDOFF::FRONT", "PASS4::FRONT");
      registry.registerEventClass("HANDOFF::BACK", "PASS4::BACK");
   }
}

void IrfLoader::loadIrfs() const {
   int convType;
   for (size_t i(0); i < m_caldbNames.size(); i++) {
      addIrfs(m_caldbNames.at(i), "FRONT", convType=0);
      addIrfs(m_caldbNames.at(i), "BACK", convType=1);
   }
   loadCustomIrfs();
}

void IrfLoader::addIrfs(const std::string & version, 
                        const std::string & detector,
                        int convType,
                        const std::string & date) const {
   std::string irfName(version);

   irfInterface::IrfsFactory * myFactory(irfInterface::IrfsFactory::instance());
   const std::vector<std::string> & irfNames(myFactory->irfNames());

// Check if this set of IRFs already exists.
   if (std::count(irfNames.begin(), irfNames.end(), irfName+"::"+detector)) {
      return;
   }
   std::vector<std::string> aeff_files;
   std::vector<std::string> psf_files;
   std::vector<std::string> edisp_files;
   std::vector<int> hdus;

   m_hdcaldb->getFiles(aeff_files, hdus, detector, "EFF_AREA", irfName);
   m_hdcaldb->getFiles(psf_files, hdus, detector, "RPSF", irfName);
   m_hdcaldb->getFiles(edisp_files, hdus, detector, "EDISP", irfName);

   if (aeff_files.size() == 1 && psf_files.size() == 1 
       && edisp_files.size() == 1) {
      addIrfs(aeff_files[0], psf_files[0], edisp_files[0], convType, irfName);
   } else {
      addIrfs(aeff_files, psf_files, edisp_files, hdus, convType, irfName);
      st_stream::StreamFormatter formatter("latResponse::IrfLoader",
                                           "addIrfs", 4);
      formatter.info() << "multiple epoch IRF: " << irfName << std::endl;
   }
}

void IrfLoader::addIrfs(const std::string & aeff_file, 
                        const std::string & psf_file,
                        const std::string & edisp_file,
                        int convType,
                        const std::string & irfName) const {
   irfInterface::IrfsFactory * myFactory(irfInterface::IrfsFactory::instance());
   for (size_t i(0); i < subclasses(irfName).size(); i++) {
      std::string class_name(subclasses(irfName).at(i));
      bool front;
      if (convType == 0) {
         class_name += "::FRONT";
         front = true;
      } else {
         class_name += "::BACK";
         front = false;
      }

      size_t irfID(i*2 + convType);

      irfInterface::Irfs * 
         irfs = new irfInterface::Irfs(aeff(aeff_file, i),
                                       psf(psf_file, front, i),
                                       edisp(edisp_file, i),
                                       irfID);
      irfInterface::IEfficiencyFactor * eff(efficiency_factor(aeff_file));
      if (eff) {
         irfs->setEfficiencyFactor(eff);
         delete eff;
      }

      myFactory->addIrfs(class_name, irfs);
   }
}

void IrfLoader::
addIrfs(const std::vector<std::string> & aeff_files,
        const std::vector<std::string> & psf_files,
        const std::vector<std::string> & edisp_files,
        const std::vector<int> & hdus,
        int convType,
        const std::string & irfName) const {
   irfInterface::IrfsFactory *myFactory(irfInterface::IrfsFactory::instance());
   for (size_t i(0); i < subclasses(irfName).size(); i++) {
      std::string class_name(subclasses(irfName).at(i));
      bool front;
      if (convType == 0) {
         class_name += "::FRONT";
         front = true;
      } else {
         class_name += "::BACK";
         front = false;
      }

      size_t irfID(i*2 + convType);
      AeffEpochDep * my_aeff(new AeffEpochDep());
      PsfEpochDep * my_psf(new PsfEpochDep());
      EdispEpochDep * my_edisp(new EdispEpochDep());
      for (size_t j(0); j < aeff_files.size(); j++) {
         double epoch_start(EpochDep::epochStart(aeff_files[j], 
                                                 "EFFECTIVE AREA"));
         irfInterface::IAeff * aeff_component(aeff(aeff_files[j], i));
         my_aeff->addAeff(*aeff_component, epoch_start);
         delete aeff_component;
      }
      for (size_t j(0); j < psf_files.size(); j++) {
         double epoch_start(EpochDep::epochStart(psf_files[j], "RPSF"));
         irfInterface::IPsf * psf_component(psf(psf_files[j], front, i));
         my_psf->addPsf(*psf_component, epoch_start);
         delete psf_component;
      }
      for (size_t j(0); j < edisp_files.size(); j++) {
         double epoch_start(EpochDep::epochStart(edisp_files[j], 
                                                 "ENERGY DISPERSION"));
         irfInterface::IEdisp * edisp_component(edisp(edisp_files[j], i));
         my_edisp->addEdisp(*edisp_component, epoch_start);
         delete edisp_component;
      }
      irfInterface::Irfs * 
         irfs = new irfInterface::Irfs(my_aeff, my_psf, my_edisp, irfID);

      /// Add efficiency factors.  This needs to be a separate loop
      /// for backwards compatibility for IRFs that don't have
      /// efficiency factor extensions.
      irfInterface::IEfficiencyFactor * eff_component = 
         efficiency_factor(aeff_files[0]);
      if (eff_component) {
         // If first file has an efficiency factor extension, then
         // proceed as if they all do.
         EfficiencyFactorEpochDep * my_eff(new EfficiencyFactorEpochDep());
         my_eff->add(*eff_component,
                     EpochDep::epochStart(aeff_files[0], "EFFECTIVE AREA"));
         delete eff_component;
         for (size_t j(1); j < aeff_files.size(); j++) {
            eff_component = efficiency_factor(aeff_files[j]);
            if (!eff_component) {
               throw std::runtime_error("Missing EFFICIENCY_PARAMS extension");
            }
            my_eff->add(*eff_component, 
                        EpochDep::epochStart(aeff_files[j], "EFFECTIVE AREA"));
            delete eff_component;
         }
         irfs->setEfficiencyFactor(my_eff);
         delete my_eff;
      }
      myFactory->addIrfs(class_name, irfs);
   }
}

irfInterface::IAeff * 
IrfLoader::aeff(const std::string & aeff_file, size_t nrow) const {
   return new Aeff(aeff_file, "EFFECTIVE AREA", nrow);
}

irfInterface::IPsf * 
IrfLoader::psf(const std::string & psf_file, bool front, size_t nrow) const {
   switch (psfVersion(psf_file)) {
   case 1:
      return new Psf(psf_file, front, "RPSF", nrow);
      break;
   case 2:
      return new Psf2(psf_file, front, "RPSF", nrow);
      break;
   case 3:
      return new Psf3(psf_file, front, "RPSF", nrow);
      break;
   default:
      throw std::runtime_error("PSF version not found.");
   }
}

irfInterface::IEdisp *
IrfLoader::edisp(const std::string & edisp_file, size_t nrow) const {
   if (edispVersion(edisp_file) == 2) {
      return new Edisp2(edisp_file, "ENERGY DISPERSION", nrow);
   } else if (edispVersion(edisp_file) == 3) {
      return new Edisp3(edisp_file, "ENERGY DISPERSION", nrow);
   }
   return new Edisp(edisp_file, "ENERGY DISPERSION", nrow);
}

irfInterface::IEfficiencyFactor *
IrfLoader::efficiency_factor(const std::string & aeff_file) const {
   try {
      return new EfficiencyFactor(aeff_file);
   } catch (tip::TipException &) {
      return 0;
   }
}

void IrfLoader::readCustomIrfNames() {
   char * custom_irf_dir(::getenv("CUSTOM_IRF_DIR"));
   if (!custom_irf_dir) {
      return;
   }
   m_customIrfDir = custom_irf_dir;

   char * custom_irf_names(::getenv("CUSTOM_IRF_NAMES"));

   if (!custom_irf_names) {
      throw std::runtime_error("CUSTOM_IRF_NAMES env var not set." );
   }

   facilities::Util::stringTokenize(custom_irf_names, ", ", m_customIrfNames);

   for (size_t i(0); i < m_customIrfNames.size(); i++) {
      std::string basename("aeff_" + m_customIrfNames.at(i) + "_front.fits");
      std::string aeff_file = 
         facilities::commonUtilities::joinPath(m_customIrfDir, basename);
      m_subclasses[m_customIrfNames.at(i)] = std::vector<std::string>();
      buildClassNames(aeff_file, m_customIrfNames.at(i),
                      m_subclasses[m_customIrfNames.at(i)]);
   }
//    std::cout << "Adding custom IRFs: " << std::endl;
//    for (size_t i(0); i < m_customIrfNames.size(); i++) {
//       std::cout << m_customIrfNames.at(i) << std::endl;
//    }
}

void IrfLoader::loadCustomIrfs() const {
   const std::string & irfDir(m_customIrfDir);

   irfInterface::IrfsFactory * myFactory(irfInterface::IrfsFactory::instance());
   const std::vector<std::string> & irfNames(myFactory->irfNames());

   for (size_t i(0); i < m_customIrfNames.size(); i++) {
      const std::string & irfName(m_customIrfNames.at(i));
      if (!std::count(irfNames.begin(), irfNames.end(), irfName + "::FRONT")) {
         std::string section("front");
         std::string aeff_file = st_facilities::Env
            ::appendFileName(irfDir, "aeff_"+irfName+"_"+section+".fits");
         std::string psf_file = st_facilities::Env
            ::appendFileName(irfDir, "psf_"+irfName+"_"+section+".fits");
         std::string edisp_file = st_facilities::Env
            ::appendFileName(irfDir,"edisp_"+irfName+"_"+section+".fits");
         addIrfs(aeff_file, psf_file, edisp_file, 0, irfName);

         section = "back";
         aeff_file = st_facilities::Env
            ::appendFileName(irfDir, "aeff_"+irfName+"_"+section+".fits");
         psf_file = st_facilities::Env
            ::appendFileName(irfDir, "psf_"+irfName+"_"+section+".fits");
         edisp_file = st_facilities::Env
            ::appendFileName(irfDir,"edisp_"+irfName+"_"+section+".fits");
         addIrfs(aeff_file, psf_file, edisp_file, 1, irfName);
      }
   }
}

void IrfLoader::read_caldb_indx() {
   m_caldbNames.clear();
   char * caldb_path = ::getenv("CALDB");
   if (!caldb_path) {
      throw std::runtime_error("CALDB env var not set");
   }
   std::string caldb_indx;
   find_cif(caldb_indx);
   caldb_indx = facilities::commonUtilities::joinPath(caldb_path, caldb_indx);

   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table = fileSvc.readTable(caldb_indx, "CIF");
   
   CaldbDate cutoff_date("2007-01-01");

   tip::Table::ConstIterator it(table->begin());
   tip::ConstTableRecord & row(*it);
   for ( ; it != table->end(); ++it) {
      std::string cal_date;
      row["cal_date"].get(cal_date);
      CaldbDate caldbDate(cal_date);
      if (caldbDate > cutoff_date) {
         std::vector<std::string> cal_cbd;
         row["cal_cbd"].get(cal_cbd);
         std::vector<std::string> tokens;
         facilities::Util::stringTokenize(cal_cbd.front(), "()", tokens);
         const std::string & caldbName(tokens.at(1));
         if (!std::count(m_caldbNames.begin(), m_caldbNames.end(), caldbName)) {
            m_caldbNames.push_back(caldbName);
         }
      }
   }
   for (size_t i(0); i < m_caldbNames.size(); i++) {
      getCaldbClassNames(m_caldbNames.at(i));
   }
   delete table;
}

const std::vector<std::string> & 
IrfLoader::subclasses(const std::string & irfName) const {
   std::map<std::string, std::vector<std::string> >::const_iterator
      it = m_subclasses.find(irfName);
   if (it == m_subclasses.end()) {
      throw std::runtime_error("IRF named " + irfName + " not found.");
   }
   return it->second;
}

void IrfLoader::getCaldbClassNames(const std::string & irfName,
                                   const std::string & date) {
   std::vector<std::string> fitsfiles;
   std::vector<int> hdus;
   m_hdcaldb->getFiles(fitsfiles, hdus, "FRONT", "EFF_AREA", irfName);
   m_subclasses[irfName] = std::vector<std::string>();
   buildClassNames(fitsfiles[0], irfName, m_subclasses[irfName]);
}

void IrfLoader::buildClassNames(const std::string & fitsfile,
                                const std::string & irfName,
                                std::vector<std::string> & classNames) const {
   size_t numClasses(getNumRows(fitsfile));
   if (numClasses == 1) {
      classNames.push_back(irfName);
      return;
   }
   for (size_t i(0); i < numClasses; i++) {
      std::ostringstream subclass;
//      subclass << irfName << "_" << std::setw(2) << std::setfill('0') << i;
      subclass << irfName << "_" << i;
      classNames.push_back(subclass.str());
   }
}

size_t IrfLoader::getNumRows(const std::string & fitsfile) {
   std::string extname;
   st_facilities::FitsUtil::getFitsHduName(fitsfile, 2, extname);
   const tip::Table * table = 
      tip::IFileSvc::instance().readTable(fitsfile, extname);
   size_t numRows(table->getNumRecords());
   delete table;
   return numRows;
}

void IrfLoader::find_cif(std::string & caldb_indx) const {
   char * caldb_config = ::getenv("CALDBCONFIG");
   if (!caldb_config) {
      throw std::runtime_error("CALDBCONFIG env var not set");
   }
   std::vector<std::string> lines;
   st_facilities::Util::readLines(caldb_config, lines);
   for (size_t i(0); i < lines.size(); i++) {
      std::vector<std::string> tokens;
      facilities::Util::stringTokenize(lines.at(i), " \t", tokens);
      if (tokens.at(0) == "GLAST" && tokens.at(1) == "LAT") {
         caldb_indx = facilities::commonUtilities::joinPath(tokens.at(3),
                                                            tokens.at(4));
         return;
      }
   }
   throw std::runtime_error("GLAST LAT not found in caldb.config file");
}

int IrfLoader::edispVersion(const std::string & fitsfile) const {
   std::string extname;
   st_facilities::FitsUtil::getFitsHduName(fitsfile, 2, extname);
   const tip::Table * table = 
      tip::IFileSvc::instance().readTable(fitsfile, extname);
   int version(1);
   try {
      table->getHeader()["EDISPVER"].get(version);
   } catch (tip::TipException & eObj) {
      /// EDISPVER keyword is (probably) missing, so assume default version
   }
   delete table;
   return version;
}

int IrfLoader::psfVersion(const std::string & fitsfile) const {
   std::string extname;
   st_facilities::FitsUtil::getFitsHduName(fitsfile, 2, extname);
   const tip::Table * table = 
      tip::IFileSvc::instance().readTable(fitsfile, extname);
   int version(1);
   try {
      table->getHeader()["PSFVER"].get(version);
   } catch (tip::TipException & eObj) {
      /// PSFVER keyword is (probably) missing, so assume default version
   }
   delete table;
   return version;
}

} // namespace latResponse
