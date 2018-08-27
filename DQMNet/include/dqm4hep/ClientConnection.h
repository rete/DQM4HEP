//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_CLIENTCONNECTION_H
#define DQM4HEP_CLIENTCONNECTION_H

// -- mongoose headers
#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/HttpMessage.h>
#include <dqm4hep/NetworkCommon.h>
#include <dqm4hep/Connection.h>
#include <dqm4hep/WebsocketMessage.h>

namespace dqm4hep {
  
  namespace net {
    
    /**
     *  @brief  ClientConnection class
     *  Single client connection to a server
     */
    class ClientConnection {
    public:
      /**
       *  @brief  ConnectConfig struct
       */
      struct ConnectConfig {
        /// The server host name
        std::string              m_host = {};
        /// The server port
        int                      m_port = {-1}; 
        /// The url route, e.g '/list'
        std::string              m_route = {};
      };
      
    public:
      /**
       *  @brief  Constructor
       *
       *  @param  eventLoop the event loop in which the client runs
       */
      ClientConnection(NetworkEventLoop *eventLoop);
        
      /**
       *  @brief  Set the connection configuration.
       *  Throws an exception if the connection is already done
       *  
       *  @param  config the configuration to setup
       */
      void setConnectConfig(const ConnectConfig &config);
      
      /**
       *  @brief  Connect the client to the server
       *  Throws an exception if the client is already connected or not configured
       */
      void connect();
      
      /**
       *  @brief  Shortcut configure and connect
       *          
       *  @param  config the configuration to setup
       */
      void connect(const ConnectConfig &config);
      
      /**
       *  @brief  Whether the connection has already been initialized
       */
      bool connected() const;
      
      /**
       *  @brief  Whether the connection is already configured
       */
      bool configured() const;
      
      /**
       *  @brief  Get the connection configuration
       */
      const ConnectConfig &config() const;

      /**
       *  @brief  Set the callback function on websocket message
       * 
       *  @param  func the function to callback on websocket message
       */
      void onMessage(MessageFunction func);
      
      /**
       *  @brief  Set the callback function on new connection
       * 
       *  @param  func the function to callback on new connection
       */
      void onConnect(ConnectFunction func);
      
      /**
       *  @brief  Set the callback function on connection close
       * 
       *  @param  func the function to callback on connection close
       */
      void onClose(CloseFunction func);
      
      /**
       *  @brief  Send a text message to server
       * 
       *  @param  data the text message to send 
       */
      void sendText(const std::string &data);
      
      /**
       *  @brief  Send a text message to server
       * 
       *  @param  data the text message to send 
       *  @param  len the text message length 
       */
      void sendText(const char *data, size_t len);
      
      /**
       *  @brief  Send a binary message to server
       * 
       *  @param  data the binary message to send 
       */
      void sendBinary(const std::string &data);
      
      /**
       *  @brief  Send a binary message to server
       * 
       *  @param  data the binary message to send 
       *  @param  len the binary message length 
       */
      void sendBinary(const char *data, size_t len);
      
    private:
      static void eventHandler(struct mg_connection *connection, int event, void *p);
      void handleEvent(struct mg_connection *connection, int event, void *p);
      
    private:
      /// The event loop in which the connection runs
      NetworkEventLoop*            m_eventLoop = {};
      /// The connection configuration
      ConnectConfig                m_config = {};
      /// Whether the connection is configured
      bool                         m_configured = {false};
      /// Whether the connection as been initialized
      bool                         m_connected = {false};
      /// The connection implementation (mongoose)
      struct mg_connection*        m_connection = {nullptr};
      /// The user callback function on message reception
      MessageFunction              m_onMessage = {nullptr};
      /// The user callback function on connection open
      ConnectFunction              m_onConnect = {nullptr};
      /// The user callback function on connection close
      CloseFunction                m_onClose = {nullptr};
    };
    
  }
  
}

#endif //  DQM4HEP_CLIENTCONNECTION_H
