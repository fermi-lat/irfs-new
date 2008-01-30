/**
 * @brief Implementation for post-handoff review IRFs
 * @author J. Chiang
 *
 * $Header$
 */

//namespace latResponse {

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());

/// @todo Replace all these hardwired IRF names with code that reads
/// caldb.indx for this information.

   const char * class_names[] = {"standard/front", "standard/back"};
   std::vector<std::string> classNames(class_names, class_names + 2);

   classNames.at(0) = m_className + "/front";
   classNames.at(1) = m_className + "/back";
   registry.registerEventClasses("HANDOFF", classNames);
   registry.registerEventClass("HANDOFF_front", classNames.at(0));
   registry.registerEventClass("HANDOFF_back", classNames.at(1));

   registry.registerEventClasses("Pass4_v1", classNames);
   registry.registerEventClass("Pass4_v1_front", classNames.at(0));
   registry.registerEventClass("Pass4_v1_back", classNames.at(1));

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
   registry.registerEventClass("P5_v0_source_front", classNames.at(0));
   registry.registerEventClass("P5_v0_source_back", classNames.at(1));

   classNames.at(0) = "P5_v0_diffuse/front";
   classNames.at(1) = "P5_v0_diffuse/back";
   registry.registerEventClasses("P5_v0_diffuse", classNames);
   registry.registerEventClass("P5_v0_diffuse_front", classNames.at(0));
   registry.registerEventClass("P5_v0_diffuse_back", classNames.at(1));
}

void IrfLoader::loadIrfs() const {
   char * irf_name(::getenv("HANDOFF_IRF_NAME"));
   if (!irf_name) {
      m_className = "standard";
   } else {
      m_className = irf_name;
   }
   std::vector<std::string> irfNames;
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   myFactory->getIrfsName(irfNames);
   
   if (!std::count(irfsNames.begin(), irfsNames.end(),
                   m_className + "/front")) {
      irfUtil::Util::getCaldbFile("FRONT
   }
}

} // namespace latResponse
