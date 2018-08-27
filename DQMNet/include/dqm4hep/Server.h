//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_SERVER_H
#define DQM4HEP_SERVER_H

#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/ServerConnection.h>
#include <dqm4hep/ClientConnection.h>

namespace dqm4hep {
  
  namespace net {
    
    class Server {
    public:
      Server(NetworkEventLoop *eventLoop);
      
      void setName(const std::string &name);
      const std::string &name() const;
      
      void setUseNetworkManager(bool use);
      bool useNetworkManager() const;
      
      // start on first available port
      void start();
      // start on specified port
      void start(int port);
      
      void stop();
      
      bool running() const;
      
      void onHttpRequest(HttpRequestFunction func);
      
      void createService(const std::string &name);
      
      bool servciceCreated(const std::string &name) const;
      
      template <typename T>
      void sendValue(const std::string &service, const T &data);
      
      template <typename T>
      void sendArray(const std::string &service, const T *const data, size_t len);
      
      void sendText(const std::string &service, const std::string &text);
      void sendBinary(const std::string &service, const char *data, size_t len);
      
    private:
      void onNetmanConnect(const Connection &connection);
      void onNetmanClose(const Connection &connection);
      void onNetmanMessage(const Connection &connection, const WebsocketMessage &message);
      void formatRoute(std::string &str);
      
    private:
      NetworkEventLoop*            m_eventLoop = {};
      std::string                  m_name = {};
      ServerConnection             m_connection;
      ClientConnection             m_netmanConnection;
      bool                         m_running = {false};
      bool                         m_useNetworkManager = {true};
      int                          m_port = {-1};
      ServiceInfoMap               m_services = {};
      HttpRequestFunction          m_onHttpRequest = {nullptr};
    };
    
    template <typename T>
    inline void Server::sendValue(const std::string &service, const T &value) {
      sendText(service, core::typeToString(value));
    }
    
    template <typename T>
    inline void Server::sendArray(const std::string &service, const T *const data, size_t len) {
      std::string value, separator = "";
      for(size_t i=0 ; i<len ; i++) {
        value += separator + core::typeToString(data[i]);
        separator = " ";
      }
      sendText(service, value);
    }
    
  }
  
}

#endif //  DQM4HEP_SERVER_H
