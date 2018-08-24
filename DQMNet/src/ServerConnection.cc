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
#include <dqm4hep/ServerConnection.h>
#include <dqm4hep/Logging.h>

namespace dqm4hep {

  namespace net {

    ServerConnection::ServerConnection(NetworkEventLoop *eventLoop) :
      m_eventLoop(eventLoop) {
      
    }
    
    ServerConnection::~ServerConnection() {
      if(nullptr != m_connection) {
        // TODO not sure if we should close the connection there
        // or if the event loop close it for us
        m_connection = nullptr;
      }
    }
    
    void ServerConnection::bind(const BindConfig& config) {
      if(m_connected) {
        dqm_error("ServerConnection::bind: Already connected, can't connect twice !");
        throw core::StatusCodeException(core::STATUS_CODE_ALREADY_INITIALIZED);
      }
      if(config.m_port <= 0) {
        dqm_error("ServerConnection::bind: Invalid port number !");
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
      }
      m_bindConfig = config;
      NetworkEventLoop::Manager* mgr = m_eventLoop->manager();
      std::string portString = core::typeToString(m_bindConfig.m_port);
      struct mg_bind_opts options;
      options.user_data = this;
      m_connection = mg_bind_opt(mgr, portString.c_str(), &ServerConnection::eventHandler, options);
      mg_set_protocol_http_websocket(m_connection);
      m_connected = true;
    }
    
    void ServerConnection::eventHandler(struct mg_connection *connection, int event, void *p) {
      ServerConnection* srvCon = (ServerConnection*) connection->user_data;
      srvCon->handleEvent(connection, event, p);
    }
    
    void ServerConnection::onHttpRequest(HttpRequestFunction func) {
      m_httpRequestFunction = func;
    }
    
    void ServerConnection::onNewConnection(NewConnectionFunction func) {
      m_newConnectionFunction = func;
    }
    
    void ServerConnection::onConnectionClose(ConnectionCloseFunction func) {
      m_connectionCloseFunction = func;
    }
    
    void ServerConnection::onMessage(MessageFunction func) {
      m_messageFunction = func;
    }
    
    void ServerConnection::handleEvent(struct mg_connection *connection, int event, void *p) {
      switch(event) {
        case MG_EV_HTTP_REQUEST: {
          if(m_bindConfig.m_enableHttpRequests) {
            if(m_httpRequestFunction != nullptr) {
              http_message* msg = (http_message*) p;
              HttpMessage message(msg);
              HttpResponse response;
              m_httpRequestFunction(message, response);
              std::string contentType = "Content-Type: " + response.contentType();
              mg_send_head(connection, static_cast<int>(response.code()), response.response().size(), contentType.c_str());
              mg_send(connection, response.response().c_str(), response.response().size());
            }
            else {
              std::string response = "No callback on server side !";
              mg_send_head(connection, 501, response.size(), "Content-Type: text/plain");
              mg_send(connection, response.c_str(), response.size());
            }
          }
          else {
            std::string response = "Http requests have been disabled for this server!";
            mg_send_head(connection, 403, response.size(), "Content-Type: text/plain");
            mg_send(connection, response.c_str(), response.size());
          }
          break;
        }
        case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
          if(m_bindConfig.m_enableWebsockets) {
            http_message* msg = (http_message*) p;
            Connection con(std::string(msg->uri.p, msg->uri.len), connection);
            m_websocketConnections.insert(Connections::value_type(connection, con));
          }
          break;
        }
        // new websocket connection
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
          if(m_bindConfig.m_enableWebsockets) {
            auto iter = m_websocketConnections.find(connection);
            if(m_newConnectionFunction != nullptr and m_websocketConnections.end() != iter) {
              m_newConnectionFunction(iter->second);
            }
          }
          break;
        }
        // websocket connection closing
        case MG_EV_CLOSE: {
          if(m_bindConfig.m_enableWebsockets) {
            if((connection->flags & MG_F_IS_WEBSOCKET) and m_connectionCloseFunction != nullptr) {
              auto iter = m_websocketConnections.find(connection);
              if(m_websocketConnections.end() != iter) {
                m_connectionCloseFunction(iter->second);
                m_websocketConnections.erase(iter);
              }              
            }
          }
          break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
          if(m_bindConfig.m_enableWebsockets) {
            if(m_messageFunction != nullptr) {
              auto iter = m_websocketConnections.find(connection);
              if(m_websocketConnections.end() != iter) {
                struct websocket_message *wm = (struct websocket_message *) p;
                WebsocketMessage message(wm);
                m_messageFunction(iter->second, message);                
              }
            }
          }
          break;
        }
        default:
          break;
      }
    }
    
    void ServerConnection::stop() {
      if(not m_connected) {
        return;
      }
      // close all client connections
      for(auto con : m_websocketConnections) {
        con.first->flags |= MG_F_CLOSE_IMMEDIATELY;
      }
      m_websocketConnections.clear();
      // close server connection
      m_connection->flags |= MG_F_CLOSE_IMMEDIATELY;
      m_connected = false;
    }
    
    void ServerConnection::close(const Connection &connection) {
      connection.connection()->flags |= MG_F_CLOSE_IMMEDIATELY;
    }
    
    void ServerConnection::send(const Connection &connection, const std::string& data) {
      send(connection, data.c_str(), data.size());
    }
    
    void ServerConnection::send(const Connection &connection, const char *data, size_t length) {
      mg_send(connection.connection(), data, length);
    }
    
    void ServerConnection::broadcastUri(const std::string &uri, const std::string& data) {
      broadcastUri(uri, data.c_str(), data.size());
    }
    
    void ServerConnection::broadcastUri(const std::string &uri, const char *data, size_t length) {
      for(auto con : m_websocketConnections) {
        if(con.second.uri() == uri) {
          send(con.second, data, length);          
        }
      }
    }
    
    void ServerConnection::broadcast(const std::string& data) {
      broadcast(data.c_str(), data.size());
    }
    
    void ServerConnection::broadcast(const char *data, size_t length) {
      for(auto con : m_websocketConnections) {
        send(con.second, data, length);          
      }
    }
    
  }
  
}
