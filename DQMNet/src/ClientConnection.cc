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
#include <dqm4hep/ClientConnection.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/NetworkCommon.h>

namespace dqm4hep {

  namespace net {

    ClientConnection::ClientConnection(NetworkEventLoop *eventLoop) :
      m_eventLoop(eventLoop) {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::setConnectConfig(const ConnectConfig &config) {
      if(connected()) {
        return;
      }
      m_config = config;
      m_configured = true;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool ClientConnection::connected() const {
      return m_connected;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool ClientConnection::configured() const {
      return m_configured;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::connect() {
      if(connected()) {
        dqm_error("ClientConnection::connect: already connected !");
        throw core::StatusCodeException(core::STATUS_CODE_ALREADY_PRESENT);
      }
      if(not configured()) {
        dqm_error("ClientConnection::connect: connection not configured !");
        throw core::StatusCodeException(core::STATUS_CODE_NOT_INITIALIZED);
      }
      std::string uri = "ws://" + m_config.m_host + ":" + core::typeToString(m_config.m_port) + m_config.m_route;
      mg_connect_opts opts;
      memset(&opts, 0, sizeof(opts));
      opts.user_data = this;
      m_connection = mg_connect_ws_opt(m_eventLoop->manager(), eventHandler, opts, uri.c_str(), nullptr, nullptr);
      if(nullptr == m_connection) {
        return;
      }
      m_connected = true;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::connect(const ConnectConfig &config) {
      setConnectConfig(config);
      connect();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const ClientConnection::ConnectConfig &ClientConnection::config() const {
      return m_config;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::onMessage(MessageFunction func) {
      m_onMessage = func;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::onConnect(ConnectFunction func) {
      m_onConnect = func;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::onClose(CloseFunction func) {
      m_onClose = func;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::sendText(const std::string &data) {
      sendText(data.c_str(), data.size());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::sendText(const char *data, size_t len) {
      if(not connected()) {
        return;
      }
      mg_send_websocket_frame(m_connection, WEBSOCKET_OP_TEXT, data, len);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::sendBinary(const std::string &data) {
      sendBinary(data.c_str(), data.size());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::sendBinary(const char *data, size_t len) {
      if(not connected()) {
        return;
      }
      mg_send_websocket_frame(m_connection, WEBSOCKET_OP_BINARY, data, len);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::eventHandler(struct mg_connection *connection, int event, void *p) {
      ClientConnection* con = (ClientConnection*)connection->user_data;
      con->handleEvent(connection, event, p);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ClientConnection::handleEvent(struct mg_connection *con, int event, void *p) {
      Connection connection(m_config.m_route, con);
      switch(event) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
          if(nullptr != m_onConnect) {
            m_onConnect(connection);
          }
          break;
        }
        case MG_EV_WEBSOCKET_FRAME: {
          if(nullptr != m_onMessage) {
            struct websocket_message *ws = (struct websocket_message *)p;
            WebsocketMessage message(ws);
            std::cout << "p: " << p << std::endl;
            std::cout << "ws: " << ws << std::endl;
            m_onMessage(connection, message);
          }
          break;
        }
        case MG_EV_CLOSE: {
          if(nullptr != m_onClose) {
            m_onClose(connection);
          }
          break;
        }
        default:
          break;
      }
    }
    
  }
  
}
