/**
 * @file EventTypeMapper.h
 * @brief Singleton class to manage IRF name to event type mapping.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef irfUtil_EventTypeMapper_h
#define irfUtil_EventTypeMapper_h

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace irfUtil {

class EventTypeMapper {

public:

   static EventTypeMapper & instance();

   static void deleteInstance();

   typedef std::map<std::string, std::pair<unsigned int, std::string> > 
      EvTypeMapping_t;

   /// @return mapping of event type name to (bit position, partition name)
   /// @param irfName Name of IRFs, e.g., "P8R2_SOURCE_V6"
   const EvTypeMapping_t & mapping(const std::string & irfName) const; 

   /// @return (bit position, partition name)
   /// @param irfName Name of IRFs, e.g., "P8R2_SOURCE_V6"
   /// @param event_type Event type name, e.g., "FRONT", "PSF0", "EDISP2"
   const std::pair<unsigned int, std::string> &
   bitPos(const std::string & irfName, const std::string & event_type) const;

   const std::map<std::string, unsigned int> &
   full_bitmasks_by_partition(const std::string & irfName) const;

   void getPartitions(const std::string & irfName,
                      std::vector<std::string> & partitions) const;

protected:

   EventTypeMapper();

private:

   static EventTypeMapper * s_instance;

   mutable std::map<std::string, EvTypeMapping_t> m_mappings;

   mutable std::map<std::string, 
                    std::map<std::string, unsigned int> > m_full_bitmasks;

};

} // namespace irfUtil

#endif // irfUtil_EventTypeMapper_h
