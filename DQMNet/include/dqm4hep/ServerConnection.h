//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_SERVERCONNECTION_H
#define DQM4HEP_SERVERCONNECTION_H

#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/HttpMessage.h>
#include <dqm4hep/HttpResponse.h>
#include <dqm4hep/Connection.h>
#include <dqm4hep/WebsocketMessage.h>

namespace dqm4hep {
  
  namespace net {
    
    class ServerConnection {
    public:
      typedef std::function<void(const HttpMessage&, HttpResponse&)> HttpRequestFunction;
      typedef std::function<void(const Connection&)> NewConnectionFunction;
      typedef std::function<void(const Connection&)> ConnectionCloseFunction;
      typedef std::function<void(const Connection&, const WebsocketMessage&)> MessageFunction;
      typedef std::map<struct mg_connection*, Connection> Connections;
      
      struct BindConfig {
        int            m_port = {-1};
        bool           m_enableWebsockets = {true};
        bool           m_enableHttpRequests = {true};
      };
      
    public:
      ServerConnection(NetworkEventLoop *eventLoop);
      ~ServerConnection();
      
      // bind
      void bind(const BindConfig& config);
      
      // callbacks
      void onHttpRequest(HttpRequestFunction func);
      void onNewConnection(NewConnectionFunction func);
      void onConnectionClose(ConnectionCloseFunction func);
      void onMessage(MessageFunction func);
      
      void stop();
      void close(const Connection &connection);
      
      void send(const Connection &connection, const std::string& data);
      void send(const Connection &connection, const char *data, size_t length);
      
      void broadcastUri(const std::string &uri, const std::string& data);
      void broadcastUri(const std::string &uri, const char *data, size_t length);
      
      void broadcast(const std::string& data);
      void broadcast(const char *data, size_t length);      
      
    private:
      static void eventHandler(struct mg_connection *connection, int event, void *p);
      void handleEvent(struct mg_connection *connection, int event, void *p);
      
    private:
      NetworkEventLoop*            m_eventLoop = {};
      bool                         m_connected = {false};
      struct mg_connection*        m_connection = {nullptr};
      BindConfig                   m_bindConfig = {};
      // callback functions
      HttpRequestFunction          m_httpRequestFunction = {nullptr};
      NewConnectionFunction        m_newConnectionFunction = {nullptr};
      ConnectionCloseFunction      m_connectionCloseFunction = {nullptr};
      MessageFunction              m_messageFunction = {nullptr};
      Connections                  m_websocketConnections = {};
    };
    
  }
  
}

#endif //  DQM4HEP_SERVERCONNECTION_H
