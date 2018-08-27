

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_NETWORKCOMMON_H
#define DQM4HEP_NETWORKCOMMON_H

#include <dqm4hep/Internal.h>

namespace dqm4hep {
  
  namespace net {
    
    class Connection;
    class WebsocketMessage;
    class HttpResponse;
    class HttpMessage;
    
    typedef std::function<void(HttpMessage &)> HttpResponseFunction;
    typedef std::function<void(const HttpMessage&, HttpResponse&)> HttpRequestFunction;
    typedef std::function<void(const Connection&, const WebsocketMessage&)> MessageFunction;
    typedef std::function<void(const Connection&)> ConnectFunction;
    typedef std::function<void(const Connection&)> CloseFunction;
    
    /**
     *  @brief  Uri struct.
     *  Uris are described as [protocol]://host[:port][route][?data][#fragment]
     */
    struct Uri {
      /// The uri protocol
      std::string            m_protocol = {"http"};
      /// The uri host
      std::string            m_host = {"localhost"};
      /// The uri port number
      int                    m_port = {80};
      /// The uri route
      std::string            m_route = "";
      /// The list of data (key=value)
      core::StringMap        m_data = {};
      /// The uri fragment
      std::string            m_fragment = {""};
    };
    
    /**
     *  @brief  Utils class
     *  A collection of networking utility functions
     */
    class Utils {
    public:
      /**
       *  @brief  Get the default network manager port
       * @return [description]
       */
      static int networkManagerDefaultPort();
      
      /**
       *  @brief  Get the default network manager host
       */
      static std::string networkManagerDefaultHost();
      
      /**
       *  @brief  Get the network manager port
       *  Look in environment variable DQM4hep_NETMGR_PORT or use the default
       */
      static int networkManagerPort();
      
      /**
       *  @brief  Get the network manager host
       *  Look in environment variable DQM4hep_NETMGR_HOST or use the default
       */
      static std::string networkManagerHost();
      
      /**
       *  @brief  Get the DQM4hep port range for networking
       *  
       *  @param  startRange the start range to receive
       *  @param  endRange the end range to receive
       */
      static void portRange(int &startRange, int &endRange);
      
      /**
       *  @brief  Find an available port on the running host to open a new connection
       *  
       *  @param  startRange the start range to look
       *  @param  endRange the end range to look (included)
       */
      static int findAvailablePort(int startRange, int endRange);
      
      /**
       *  @brief  Find an available port on the running host to open a new connection
       *  using the default DQM4hep port range
       */
      static int findAvailablePort();
      
      /**
       *  @brief  Convert the uri object to string
       * 
       *  @param  uri the uri object to convert
       */
      static std::string buildUriString(const Uri &uri);
      
      /**
       *  @brief  Perform an http GET and callback the user function on response
       *  
       *  @param  uri the full URI to send the GET to
       *  @param  func the user callback function to process the GET response 
       */
      static void httpGet(const std::string &uri, HttpResponseFunction func);
      
      /**
       *  @brief  Send an http POST and callback the user function on response
       * 
       *  @param uri The full URI to send the POST to
       *  @param contentType the content type of the post
       *  @param data the data to post
       *  @param func the user callback function on response
       */
      static void httpPost(const std::string &uri, const std::string &contentType, const core::StringMap &data, HttpResponseFunction func);
    };
    
    /**
     *  @brief  ServiceType struct 
     */
    struct ServiceType {
      /// unknown: default type
      static constexpr int UNKNOWN = 0;
      /// request to server
      static constexpr int REQUEST = 1;
      /// publish - subscribe pattern
      static constexpr int PUB_SUB = 2;
      /// push data to server
      static constexpr int PUSH = 3;
    };
    
    typedef std::map<std::string, int> ServiceInfoMap;
    
    /**
     *  @brief  ServerInfo class
     */
    class ServerInfo {
    public:
      ServerInfo() = default;
      
      /**
       *  @brief  Set the list of services handled by the server
       *
       *  @param  svcs the services list
       */
      void setServices(const ServiceInfoMap& svcs);
      
      /**
       *  @brief  Add the list of services to current list
       *
       *  @param  svcs the services list
       */
      void addServices(const ServiceInfoMap& svcs);
      
      /**
       *  @brief  Remove the list of services from the current list
       *
       *  @param  svcs the services list
       */
      void removeServices(const ServiceInfoMap& svcs);
      
      /**
       *  @brief  Get the list of services handled by the server
       */
      const ServiceInfoMap& services() const;
      
      /**
       *  @brief  Get the sever name
       */
      const std::string& name() const;
      
      /**
       *  @brief  Get the server host name
       */
      const std::string& host() const;
      
      /**
       *  @brief  Get the server port number
       */
      int port() const;
      
      /**
       *  @brief  Set name, port and host of the server
       * 
       *  @param  name the server name
       *  @param  host the host name
       *  @param  port the port number
       */
      void setInfo(const std::string& name, const std::string& host, int port);
      
    private:
      /// Th server port number
      int                m_port = {-1};
      /// The server name
      std::string        m_name = {};
      /// The server host name
      std::string        m_host = {};
      /// The list of services handled by the server
      ServiceInfoMap     m_services = {};
    };
    
    typedef std::map<std::string, ServerInfo> ServerInfoMap;

  }

}

#endif  // DQM4HEP_NETWORKCOMMON_H