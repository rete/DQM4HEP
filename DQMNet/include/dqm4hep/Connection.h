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
      inline Connection(const std::string& route, struct mg_connection *connection) :
        m_connection(connection),
        m_route(route) {
        
      }
      
      inline sock_t socket() const {
        return m_connection->sock;
      }
      
      inline mg_mgr *manager() const {
        return m_connection->mgr;
      }
      
      inline const std::string &route() const {
        return m_route;
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
      std::string                 m_route = {};
    };
    
    inline bool operator==(const Connection &lhs, const Connection &rhs) {
      return (lhs.connection() == rhs.connection());
    }
    
    inline bool operator!=(const Connection &lhs, const Connection &rhs) {
      return (lhs.connection() != rhs.connection());
    }
    
    inline bool operator>(const Connection &lhs, const Connection &rhs) {
      return (lhs.connection() > rhs.connection());
    }
    
    inline bool operator<(const Connection &lhs, const Connection &rhs) {
      return (lhs.connection() < rhs.connection());
    }
    
  }
  
}

#endif //  DQM4HEP_SERVERCONNECTION_H
