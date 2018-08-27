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
#include <dqm4hep/NetworkCommon.h>
#include <dqm4hep/HttpMessage.h>

// -- mongoose headers
#include <mongoose.h>

// -- std headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>

namespace dqm4hep {

  namespace net {
    
    void handleHttpGetResponse(struct mg_connection *connection, int event, void *p) {
      auto &callback = *(std::function<void(struct mg_connection *connection, int event, void *p)> *)connection->user_data;
      callback(connection, event, p);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int Utils::networkManagerDefaultPort() {
      return 5555;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string Utils::networkManagerDefaultHost() {
      return "localhost";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int Utils::networkManagerPort() {
      return core::Os::getenv<int>("DQM4hep_NETMGR_PORT", networkManagerDefaultPort());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string Utils::networkManagerHost() {
      return core::Os::getenv<std::string>("DQM4hep_NETMGR_HOST", networkManagerDefaultHost());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Utils::portRange(int &startRange, int &endRange) {
      startRange = 5556;
      endRange = 6556;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int Utils::findAvailablePort(int startRange, int endRange) {
      if(startRange <= 0 or endRange <= 0) {
        return -1;
      }
      if(startRange > endRange) {
        std::swap(startRange, endRange);
      }
      std::string domain("localhost");
      int handle = socket(AF_INET, SOCK_STREAM, 0);
      if(handle == -1) {
        return -1;
      }
      struct sockaddr_in sockname;
      for(int port = startRange; port <= endRange ; port++) {
        sockname.sin_family = AF_INET;
        sockname.sin_addr.s_addr = INADDR_ANY;
        sockname.sin_port = htons((ushort) port);
        if( (bind(handle, (struct sockaddr*)&sockname, sizeof(sockname))) == -1) {
          continue;
        }
        close(handle);
        return port;
      }
      close(handle);
      return -1;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int Utils::findAvailablePort() {
      int begin, end;
      portRange(begin, end);
      return findAvailablePort(begin, end);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string Utils::buildUriString(const Uri &uri) {
      std::string uriout;
      if(not uri.m_protocol.empty()) {
        uriout = uri.m_protocol + "://";
      }
      uriout += uri.m_host;
      uriout += "" + core::typeToString(uri.m_port);
      if(not uri.m_route.empty()) {
        if(uri.m_route[0] != '/') {
          uriout += '/';
        }
        uriout += uri.m_route;
      }
      if(not uri.m_data.empty()) {
        uriout += '?';
        std::string separator;
        for(auto data : uri.m_data) {
          uriout += separator + data.first + "=" + data.second;
          separator = "&";
        }
      }
      if(not uri.m_fragment.empty()) {
        uriout += "#" + uri.m_fragment;
      }
      return uriout;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Utils::httpGet(const std::string &url, HttpResponseFunction func) {
      struct mg_mgr mgr;
      mg_mgr_init(&mgr, nullptr);
      int exitFlag = 0;

      std::function<void(struct mg_connection *connection, int event, void *p)> lambdaFunc =
      [&exitFlag,&func](struct mg_connection *connection, int event, void *p){
        if(exitFlag == 1) {
          return;
        }
        switch(event) {
          case MG_EV_CONNECT: {
            if (*(int *) p != 0) {
              HttpMessage message(nullptr);
              func(message);
              exitFlag = 1;
            }
            break;
          }
          case MG_EV_HTTP_REPLY: {
            connection->flags |= MG_F_CLOSE_IMMEDIATELY;
            HttpMessage message((struct http_message*)p);
            func(message);
            exitFlag = 1;
            break;
          }
          case MG_EV_CLOSE: {
            exitFlag = 1;
          break;
          }
          default:
            break;
        }
      };
      struct mg_connect_opts opts;
      memset(&opts, 0, sizeof(opts));
      opts.user_data = &lambdaFunc;
      mg_connect_http_opt(&mgr, handleHttpGetResponse, opts, url.c_str(), nullptr, nullptr);
      while (exitFlag == 0) {
        mg_mgr_poll(&mgr, 50);
      }
      mg_mgr_free(&mgr);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Utils::httpPost(const std::string &uri, const std::string &contentType, const core::StringMap &data, HttpResponseFunction func) {
      struct mg_mgr mgr;
      mg_mgr_init(&mgr, nullptr);
      int exitFlag = 0;
      std::function<void(struct mg_connection *connection, int event, void *p)> lambdaFunc =
      [&exitFlag,&func](struct mg_connection *connection, int event, void *p){
        if(exitFlag == 1) {
          return;
        }
        switch(event) {
          case MG_EV_CONNECT: {
            if (*(int *) p != 0) {
              HttpMessage message(nullptr);
              func(message);
              exitFlag = 1;
            }
            break;
          }
          case MG_EV_HTTP_REPLY: {
            connection->flags |= MG_F_CLOSE_IMMEDIATELY;
            HttpMessage message((struct http_message*)p);
            func(message);
            exitFlag = 1;
            break;
          }
          case MG_EV_CLOSE: {
            exitFlag = 1;
          break;
          }
          default:
            break;
        }
      };
      struct mg_connect_opts opts;
      memset(&opts, 0, sizeof(opts));
      opts.user_data = &lambdaFunc;
      std::string fullContentType = "Content-Type: " + contentType + "\r\n";
      std::string dataStr, separator;
      for(auto d : data) {
        dataStr += separator + d.first + "=" + d.second;
        separator = "&";
      }
      mg_connect_http_opt(&mgr, handleHttpGetResponse, opts, uri.c_str(), fullContentType.c_str(), dataStr.c_str());
      while (exitFlag == 0) {
        mg_mgr_poll(&mgr, 50);
      }
      mg_mgr_free(&mgr);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ServerInfo::setServices(const ServiceInfoMap& svcs) {
      m_services = svcs;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ServerInfo::addServices(const ServiceInfoMap& svcs) {
      m_services.insert(svcs.begin(), svcs.end());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ServerInfo::removeServices(const ServiceInfoMap& svcs) {
      for(auto svc : svcs) {
        m_services.erase(svc.first);        
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const ServiceInfoMap& ServerInfo::services() const {
      return m_services;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string& ServerInfo::name() const {
      return m_name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string& ServerInfo::host() const {
      return m_host;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int ServerInfo::port() const {
      return m_port;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ServerInfo::setInfo(const std::string& n, const std::string& h, int p) {
      m_name = n;
      m_host = h;
      m_port = p;
    }
    
  }
  
}
