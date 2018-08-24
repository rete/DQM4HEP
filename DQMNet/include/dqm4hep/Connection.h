//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_CONNECTION_H
#define DQM4HEP_CONNECTION_H

#include <mongoose.h>

namespace dqm4hep {
  
  namespace net {
    
    class Connection {
    public:
      inline Connection(const std::string& uri, struct mg_connection *connection) :
        m_connection(connection),
        m_uri(uri) {
        
      }
      
      inline sock_t socket() const {
        return m_connection->sock;
      }
      
      inline mg_mgr *manager() const {
        return m_connection->mgr;
      }
      
      inline const std::string &uri() const {
        return m_uri;
      }
      
      inline struct mg_connection* connection() const {
        return m_connection;
      }
      
      template <typename T>
      inline T* connectionAs() const {
        return (T*) m_connection->user_data;
      }
      
    private:
      struct mg_connection*       m_connection = {nullptr};
      std::string                 m_uri = {};
    };
    
  }
  
}

#endif //  DQM4HEP_SERVERCONNECTION_H
