//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_NETWORKMANAGER_H
#define DQM4HEP_NETWORKMANAGER_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/NetworkCommon.h>
#include <dqm4hep/ServerConnection.h>
#include <dqm4hep/json.h>

namespace dqm4hep {
  
  namespace net {

    /**
     *  @brief  NetworkManager class
     */
    class NetworkManager {
    public:
      /**
       *  @brief  Constructor
       */
      NetworkManager();

      /**
       *  @brief  Start the network manager on a given port
       * 
       *  @param  port the port number
       */
      void start(int port);
      
      /**
       *  @brief  Stop the network manager
       */
      void stop();
      
    private:
      void setupConnection();
      void serversAsJson(core::json &serversJson);      
      typedef std::map<Connection, std::string> ServerMapping;
      
    private:
      /// The network event loop
      NetworkEventLoop            m_eventLoop = {};
      /// The server connection
      ServerConnection            m_connection = {&m_eventLoop};
      /// The mapping of connection <-> server name
      ServerMapping               m_serverMapping = {};
      /// The map of server <-> server info
      ServerInfoMap               m_servers = {};
    };
    
  }
  
}

#endif //  DQM4HEP_NETWORKMANAGER_H
