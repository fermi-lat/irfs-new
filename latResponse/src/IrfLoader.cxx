/**
 * @brief Implementation for post-handoff review IRFs
 * @author J. Chiang
 *
 * $Header$
 */

#include <vector>

#include "irfInterface/IrfRegistry.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"

#include "irfUtil/Util.h"

#include "latResponse/IrfLoader.h"

#include "Aeff.h"
#include "Edisp.h"
#include "Psf.h"

namespace latResponse {

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());

/// @todo Replace all these hardwired IRF names with code that reads
/// caldb.indx for this information.  This will require standardizing
/// the class names.
   std::vector<std::string> classNames;
   classNames.push_back(m_className + "/front");
   classNames.push_back(m_className + "/back");

   registry.registerEventClasses("HANDOFF", classNames);
   registry.registerEventClass("HANDOFF_front", classNames.at(0));
   registry.registerEventClass("HANDOFF_back", classNames.at(1));

   registry.registerEventClasses("PASS4", classNames);
   registry.registerEventClass("PASS4::FRONT", classNames.at(0));
   registry.registerEventClass("PASS4::BACK", classNames.at(1));

   classNames.at(0) = "Pass4_v2/front";
   classNames.at(1) = "Pass4_v2/back";
   registry.registerEventClasses("Pass4_v2", classNames);
   registry.registerEventClass("Pass4_v2_front", classNames.at(0));
   registry.registerEventClass("Pass4_v2_back", classNames.at(1));

   classNames.at(0) = "P5_v0_transient/front";
   classNames.at(1) = "P5_v0_transient/back";
   registry.registerEventClasses("P5_v0_transient", classNames);
   registry.registerEventClass("P5_v0_transient_front", classNames.at(0));
   registry.registerEventClass("P5_v0_transient_back", classNames.at(1));

   classNames.at(0) = "P5_v0_source/front";
   classNames.at(1) = "P5_v0_source/back";
   registry.registerEventClasses("P5_v0_source", classNames);
   registry.registerEventClasses("PASS5_source", classNames);
   registry.registerEventClass("P5_v0_source_front", classNames.at(0));
   registry.registerEventClass("P5_v0_source_back", classNames.at(1));

   classNames.at(0) = "P5_v0_diffuse/front";
   classNames.at(1) = "P5_v0_diffuse/back";
   registry.registerEventClasses("P5_v0_diffuse", classNames);
   registry.registerEventClass("P5_v0_diffuse_front", classNames.at(0));
   registry.registerEventClass("P5_v0_diffuse_back", classNames.at(1));
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
   addIrfs("PASS4", "FRONT", irfID=0, m_className + "/front");
   addIrfs("PASS4", "BACK", irfID=1, m_className + "/back");

// Use standard class names instead of non-compliant versions from
// handoff_response.
   addIrfs("PASS4", "FRONT", irfID=0);
   addIrfs("PASS4", "BACK", irfID=1);

   addIrfs("PASS4_v2", "FRONT", irfID=0);
   addIrfs("PASS4_v2", "BACK", irfID=1);

   addIrfs("PASS5_v0_TRANSIENT", "FRONT", irfID=0);
   addIrfs("PASS5_v0_TRANSIENT", "BACK", irfID=1);

   addIrfs("PASS5_v0", "FRONT", irfID=0);
   addIrfs("PASS5_v0", "BACK", irfID=1);

   addIrfs("PASS5_v0_DIFFUSE", "FRONT", irfID=0);
   addIrfs("PASS5_v0_DIFFUSE", "BACK", irfID=1);
}

} // namespace latResponse
