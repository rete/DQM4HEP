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

// -- mongoose headers
#include <mongoose.h>

namespace dqm4hep {
  
  namespace net {
    
    /**
     *  @brief  Connection class
     *  Wraps a mongoose connection and provides custom accessors
     */
    class Connection {
    public:
      /**
       *  @brief  Constructor
       *  
       *  @param  route the connection route, e.g '/list'
       *  @param  connection the connection implementation (mongoose)
       */
      inline Connection(const std::string& route, struct mg_connection *connection) :
        m_connection(connection),
        m_route(route) {
        
      }
      
      /**
       *  @brief  Get the socket fd of the connection
       */
      inline sock_t socket() const {
        return m_connection->sock;
      }
      
      /**
       *  @brief  Get the manager that owns the connection
       */
      inline mg_mgr *manager() const {
        return m_connection->mgr;
      }
      
      /**
       *  @brief  Get the connection route
       */
      inline const std::string &route() const {
        return m_route;
      }
        
      /**
       *  @brief  Get the connection implementation
       */
      inline struct mg_connection* connection() const {
        return m_connection;
      }
      
      /**
       *  @brief  Cast the connection user data to the specified type
       */
      template <typename T>
      inline T* connectionAs() const {
        return (T*) m_connection->user_data;
      }
      
    private:
      /// The connection implementation (mongoose)
      struct mg_connection*       m_connection = {nullptr};
      /// The connection route
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
