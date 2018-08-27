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
#include <dqm4hep/Server.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/json.h>

using namespace std::placeholders;

namespace dqm4hep {

  namespace net {

    Server::Server(NetworkEventLoop *eventLoop) :
      m_eventLoop(eventLoop),
      m_connection(eventLoop),
      m_netmanConnection(eventLoop) {
      
      ClientConnection::ConnectConfig config;
      config.m_host = Utils::networkManagerHost();
      config.m_port = Utils::networkManagerPort();
      config.m_route = "/servers";
      
      m_netmanConnection.setConnectConfig(config);
      m_netmanConnection.onConnect(std::bind(&Server::onNetmanConnect, this, _1));
      m_netmanConnection.onClose(std::bind(&Server::onNetmanClose, this, _1));
      m_netmanConnection.onMessage(std::bind(&Server::onNetmanMessage, this, _1, _2));
      
      m_connection.onHttpRequest([this](const HttpMessage &message, HttpResponse &response){
        if(message.uri() == "/list" && message.method() == "GET") {
          core::json responseJson = {
            {"name", m_name},
            {"port", m_port},
            {"services", m_services}
          };
          response.response().append(responseJson.dump(2));
        }
        else {
          if(nullptr != m_onHttpRequest) {
            m_onHttpRequest(message, response);
          }          
        }
      });
    }
    
    void Server::setName(const std::string &name) {
      m_name = name;
    }
    
    const std::string &Server::name() const {
      return m_name;
    }
    
    void Server::setUseNetworkManager(bool use) {
      if(running()) {
        dqm_error("Server::setUseNetworkManager: Already running, can't modify server mode now !");
        return;
      }
      m_useNetworkManager = use;
    }
    
    bool Server::useNetworkManager() const {
      return m_useNetworkManager;
    }

    void Server::start() {
      int port = Utils::findAvailablePort();
      if(port < 0) {
        dqm_error("Server::start: Couldn't find a free port !");
        return;
      }
      start(port);
    }
    
    void Server::start(int port) {
      if(running()) {
        return;
      }
      if(m_name.empty()) {
        dqm_error("Server::start: name not set !");
      }
      dqm_info("Server::start: starting server '{0}' on port {1}", name(), port);
      m_port = port;
      ServerConnection::BindConfig config;
      config.m_port = port;
      config.m_enableWebsockets = true;
      config.m_enableHttpRequests = true;
      m_connection.bind(config);
      if(useNetworkManager()) {
        m_netmanConnection.connect();
      }
      m_running = true;
    }
    
    void Server::stop() {
      if(not running()) {
        return;
      }
      // TODO stop server
      m_running = false;
    }
    
    bool Server::running() const {
      return m_running;
    }
    
    void Server::onHttpRequest(HttpRequestFunction func) {
      m_onHttpRequest = func;
    }
    
    void Server::createService(const std::string &route) {
      if(route.empty() or route.at(0) != '/') {
        dqm_error("Server::createService: Service name must start with '/' !");
        return;
      }
      auto iter = m_services.find(route);
      if(iter != m_services.end()) {
        dqm_error("Server::createService: Service '{0}' already registered !", route);
        return;        
      }
      m_services.insert(ServiceInfoMap::value_type(route, ServiceType::PUB_SUB));
    }
    
    bool Server::servciceCreated(const std::string &name) const {
      return (m_services.find(name) != m_services.end());
    }
    
    void Server::sendText(const std::string &service, const std::string &text) {
      if(not running()) {
        dqm_error("Server::sendText: server not running", service);
        return;
      }
      if(not servciceCreated(service)) {
        dqm_error("Server::sendText: not such service '{0}'", service);
        return;
      }
      m_connection.broadcastRoute(service, text);
    }
    
    void Server::sendBinary(const std::string &service, const char *data, size_t len) {
      if(not running()) {
        dqm_error("Server::sendBinary: server not running", service);
        return;
      }
      if(not servciceCreated(service)) {
        dqm_error("Server::sendBinary: not such service '{0}'", service);
        return;
      }
      m_connection.broadcastRoute(service, data, len);
    }
    
    void Server::onNetmanConnect(const Connection &connection) {
      dqm_info("Network manager connected !");
      // register server to manager
      core::StringMap hostInfo;
      core::fillHostInfo(hostInfo);
      core::json registration = {
        {"action" , "register"},
        {"server", name()},
        {"port", m_port},
        {"host", hostInfo["host"]},
        {"services", m_services}
      };
      std::string registrationStr = registration.dump();
      dqm_info("Sending to mgr: {0}", registrationStr);
      m_netmanConnection.sendText(registrationStr);
    }
    
    void Server::onNetmanClose(const Connection &connection) {
      dqm_info("Network manager disconnected !");
    }
    
    void Server::onNetmanMessage(const Connection &connection, const WebsocketMessage &message) {
      try {
        core::json messageJson = core::json::parse(message.message(), message.message()+message.length());
        std::string subject = messageJson.value<std::string>("subject", "");
        if(subject == "regfail") {
          std::string reason = messageJson.value<std::string>("reason", "");
          dqm_error("Failed to register server to manager: '{0}'", reason);
          dqm_warning("Stopping server '{0}'", name());
          stop();
        }
      }
      catch(...) {
        
      }
    }
    
  }
  
}
