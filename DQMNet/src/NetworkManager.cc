//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/NetworkManager.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/json.h>

namespace dqm4hep {

  namespace net {
    
    NetworkManager::NetworkManager() {
      setupConnection();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void NetworkManager::start(int port) {
      ServerConnection::BindConfig config;
      config.m_port = port;
      config.m_enableWebsockets = true;
      config.m_enableHttpRequests = true;
      m_connection.bind(config);
      m_eventLoop.start(true, 100);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void NetworkManager::stop() {
      m_connection.stop();
      m_eventLoop.stop();      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void NetworkManager::setupConnection() {
      m_connection.onHttpRequest([this](const HttpMessage& message, HttpResponse& response){
        const std::string uri(message.uri());
        const std::string method(message.method());
        if(uri == "/list" and method == "GET") {
          core::json serversJson({});
          serversAsJson(serversJson);
          response.response().append(serversJson.dump(2));
          response.setContentType(HttpContentType::Json);
          response.setCode(HttpStatusCode::OK);
          dqm_info("GET /list: {0}", response.response());
        }
      });
      
      m_connection.onNewConnection([this](const Connection &connection){
        std::cout << "New connection !" << std::endl;
      });
      
      m_connection.onConnectionClose([this](const Connection &connection){
        auto iter = m_serverMapping.find(connection);
        // server disconnect !
        if(iter != m_serverMapping.end()) {
          auto iter2 = m_servers.find(iter->second);
          // servers may be connected but not registered yet
          if(iter2 != m_servers.end()) {
            dqm_info("Server '{0}' removed", iter->second);
            m_servers.erase(iter2);
          }
          m_serverMapping.erase(iter);
        }
      });
      
      m_connection.onMessage([this](const Connection &connection, const WebsocketMessage &message) {
        std::cout << "Received message !" << std::endl;
        if(connection.route() == "/servers") {
          try {
            core::json messageJson = core::json::parse(message.message(), message.message() + message.length());
            std::string action = messageJson.value<std::string>("action", "");
            if(action.empty()) {
              return;
            }
            if(action == "register") {
              ServerInfo serverInfo;
              serverInfo.setInfo(
                messageJson.value<std::string>("server", ""),
                messageJson.value<std::string>("host", ""),
                messageJson.value<int>("port", -1)
              );
              ServiceInfoMap services = messageJson.value<ServiceInfoMap>("services", ServiceInfoMap());
              serverInfo.setServices(services);
              auto iter = m_servers.find(serverInfo.name());
              // server already registered !
              if(iter != m_servers.end()) {
                core::json response = {
                  {"subject", "regfail"},
                  {"reason", "Server already registered to manager !"}
                };
                m_connection.send(connection, response.dump());
                m_connection.close(connection);
                return;
              }
              dqm_info("Server '{0}' added", serverInfo.name());
              m_servers.insert(ServerInfoMap::value_type(serverInfo.name(), serverInfo));
              m_serverMapping.insert(ServerMapping::value_type(connection, serverInfo.name()));
            }
          }
          catch(...) {
            // TODO handle error here
            return;          
          }          
        }
      });
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void NetworkManager::serversAsJson(core::json &serversJson) {
      for(auto server : m_servers) {
        core::json serverJson = {
          {"server", server.second.name()},
          {"host", server.second.host()},
          {"port", server.second.port()},
          {"services", server.second.services()}
        };
        serversJson[server.first] = serverJson;
      }
    }
    
  }
  
}
