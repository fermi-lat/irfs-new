/**
 * @brief Implementation for post-handoff review IRFs
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>
#include <vector>

#include "facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Env.h"

#include "irfInterface/IrfRegistry.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"

#include "irfUtil/Util.h"

#include "latResponse/IrfLoader.h"

#include "Aeff.h"
#include "CaldbDate.h"
#include "Edisp.h"
#include "Psf.h"

namespace latResponse {

IrfLoader::IrfLoader() {
   read_caldb_indx();
   readCustomIrfNames();
}

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   std::vector<std::string> classNames(2);
   for (size_t i(0); i < m_caldbNames.size(); i++) {
      classNames.at(0) = m_caldbNames.at(i) + "::FRONT";
      classNames.at(1) = m_caldbNames.at(i) + "::BACK";
      registry.registerEventClasses(m_caldbNames.at(i), classNames);
      registry.registerEventClass(classNames.at(0), classNames.at(0));
      registry.registerEventClass(classNames.at(1), classNames.at(1));
   }
   for (size_t i(0); i < m_customIrfNames.size(); i++) {
      classNames.at(0) = m_customIrfNames.at(i) + "::front";
      classNames.at(1) = m_customIrfNames.at(i) + "::back";
      registry.registerEventClasses(m_customIrfNames.at(i), classNames);
   }
}

void IrfLoader::addIrfs(const std::string & version, 
                        const std::string & detector,
                        int irfID,
                        std::string irfName,
                        const std::string & date) {
   if (irfName == "") {
      // Build the standard name composed of the detector and version.
      irfName = version + "::" + detector;
   }

   irfInterface::IrfsFactory * myFactory(irfInterface::IrfsFactory::instance());
   const std::vector<std::string> & irfNames(myFactory->irfNames());

// Check if this set of IRFs already exists.
   if (std::count(irfNames.begin(), irfNames.end(), irfName)) {
      return;
   }
   std::string aeff_file;
   std::string psf_file;
   std::string edisp_file;
   long hdu;
   irfUtil::Util::getCaldbFile(detector, "EFF_AREA", version,
                               aeff_file, hdu, "GLAST", "LAT",
                               "NONE", date, "00:00:00");
   irfUtil::Util::getCaldbFile(detector, "RPSF", version,
                               psf_file, hdu, "GLAST", "LAT",
                               "NONE", date, "00:00:00");
   irfUtil::Util::getCaldbFile(detector, "EDISP", version,
                               edisp_file, hdu, "GLAST", "LAT",
                               "NONE", date, "00:00:00");
   irfInterface::IAeff * aeff = new Aeff(aeff_file);
   irfInterface::IPsf * psf = new Psf(psf_file);
   irfInterface::IEdisp * edisp = new Edisp(edisp_file);
   
   myFactory->addIrfs(irfName, new irfInterface::Irfs(aeff, psf, edisp, irfID));
}

void IrfLoader::loadIrfs() const {
   int irfID;
   for (size_t i(0); i < m_caldbNames.size(); i++) {
      addIrfs(m_caldbNames.at(i), "FRONT", irfID=0);
      addIrfs(m_caldbNames.at(i), "BACK", irfID=1);
   }
   loadCustomIrfs();
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
      std::cout << m_customIrfNames.at(i) << std::endl;
   }
}

void IrfLoader::loadCustomIrfs() const {
   const std::string & irfDir(m_customIrfDir);

   irfInterface::IrfsFactory * myFactory(irfInterface::IrfsFactory::instance());
   const std::vector<std::string> & irfNames(myFactory->irfNames());

   for (size_t i(0); i < m_customIrfNames.size(); i++) {
      std::string section("front");
      std::string irfName(m_customIrfNames.at(i) + "::" + section);
      if (!std::count(irfNames.begin(), irfNames.end(), irfName)) {
         std::string aeff_file = st_facilities::Env
            ::appendFileName(irfDir, "aeff_"+irfName+"_"+section+".fits");
         std::string psf_file = st_facilities::Env
            ::appendFileName(irfDir, "psf_"+irfName+"_"+section+".fits");
         std::string edisp_file = st_facilities::Env
            ::appendFileName(irfDir,"edisp_"+irfName+"_"+section+".fits");
         myFactory->addIrfs(irfName, 
                            new irfInterface::Irfs(new Aeff(aeff_file),
                                                   new Psf(psf_file),
                                                   new Edisp(edisp_file), 0));
      }
      section = "back";
      irfName = m_customIrfNames.at(i) + "::" + section;
      if (!std::count(irfNames.begin(), irfNames.end(), irfName)) {
         std::string aeff_file = st_facilities::Env
            ::appendFileName(irfDir, "aeff_"+irfName+"_"+section+".fits");
         std::string psf_file = st_facilities::Env
            ::appendFileName(irfDir, "psf_"+irfName+"_"+section+".fits");
         std::string edisp_file = st_facilities::Env
            ::appendFileName(irfDir,"edisp_"+irfName+"_"+section+".fits");
         myFactory->addIrfs(irfName,
                            new irfInterface::Irfs(new Aeff(aeff_file),
                                                   new Psf(psf_file),
                                                   new Edisp(edisp_file), 1));
      }
   }
}

void IrfLoader::read_caldb_indx() {
   m_caldbNames.clear();
   char * caldb_path = ::getenv("CALDB");
   if (!caldb_path) {
      throw std::runtime_error("CALDB env var not set");
   }
/// @todo generalize to use CALDBCONFIG file.   
   std::string caldb_indx(caldb_path + std::string("/caldb.indx"));
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
}

} // namespace latResponse
