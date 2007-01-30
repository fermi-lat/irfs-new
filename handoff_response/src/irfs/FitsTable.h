/**
 * @file FitsTable.h
 * @brief Abstraction for reading IRF FITS binary tables.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_FitsTable_h
#define handoff_response_FitsTable_h

namespace tip {
   class Table;
}

class TH2F;

namespace handoff_response {

/**
 * @class FitsTable
 * @brief Abstraction for reading IRF FITS binary tables.
 *
 */

class FitsTable {

public:
   
   FitsTable(const std::string & filename,
             const std::string & extname);

   ~FitsTable();

   void getVectorData(const std::string & fieldName,
                      std::vector<double> & values) const;

   void getTableData(const std::string & fieldName,
                     std::vector< std::vector<double> > & values) const;

   TH2F * tableData(const std::string & fieldName) const;

   void getTableDims(const std::string & fieldName, 
                     size_t & xdim, size_t & ydim) const;

   const std::vector<std::string> & fieldNames() const {
      return m_fieldNames;
   }

private:

   const tip::Table * m_table;

   std::string m_filename;

   std::vector<std::string> m_fieldNames;

   size_t fieldNum(const std::string & fieldName) const;

};

} // namespace handoff_response

#endif // handoff_response_FitsTable_h
