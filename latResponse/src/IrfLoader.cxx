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

#include "irfInterface/IrfRegistry.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"

#include "irfUtil/Util.h"

#include "latResponse/IrfLoader.h"

#include "Aeff.h"
#include "Edisp.h"
#include "Psf.h"

namespace {
   class CaldbDate {
   public:
      CaldbDate(const std::string & date) {
         std::vector<std::string> tokens;
         facilities::Util::stringTokenize(date, "-", tokens);
         m_year = std::atoi(tokens.at(0).c_str());
         m_month = std::atoi(tokens.at(1).c_str());
         m_day = std::atoi(tokens.at(2).c_str());
      }
      bool operator<(const CaldbDate & rhs) const {
         if (m_year < rhs.m_year) {
            return true;
         }
         if (m_year > rhs.m_year) {
            return false;
         }
         // years are equal
         if (m_month < rhs.m_month) {
            return true;
         }
         if (m_month > rhs.m_month) {
            return false;
         }
         // months are equal
         if (m_day < rhs.m_day) {
            return true;
         }
         // m_day >= rhs.m_day
         return false;
      }
      bool operator==(const CaldbDate & rhs) const {
         return (m_year == rhs.m_year && 
                 m_month == rhs.m_month && 
                 m_day == rhs.m_day);
      }
      bool operator>(const CaldbDate & rhs) const {
         return !(operator<(rhs) || operator==(rhs));
      }
   private:
      int m_year;
      int m_month;
      int m_day;
   };
}

namespace latResponse {

IrfLoader::IrfLoader() {
   read_caldb_indx();
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
   
   ::CaldbDate cutoff_date("2007-01-01");

   tip::Table::ConstIterator it(table->begin());
   tip::ConstTableRecord & row(*it);
   for ( ; it != table->end(); ++it) {
      std::string cal_date;
      row["cal_date"].get(cal_date);
      ::CaldbDate caldbDate(cal_date);
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
