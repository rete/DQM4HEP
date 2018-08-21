//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dim.h"
#include "dqm4hep/Client.h"
#include "dqm4hep/DQMNet.h"
#include "dqm4hep/ws/WsServer.h"

#include "json/json.h"

using WsServer = dqm4hep::net::SocketServer<dqm4hep::net::WS>;
using Endpoint = WsServer::Endpoint;
using Client = dqm4hep::net::Client;
using Buffer = dqm4hep::net::Buffer;

static const size_t maxNumberStrLen = 16;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

void trim(std::string &str) {
  // count leading spaces
  int i = 0;

  while (i < (int)str.length() && isspace(str[i]))
    ++i;

  // delete leading spaces
  if (i > 0)
    str.erase(0, i);

  // last char position
  i = str.length();
  int j = i;
  while (i > 0 && isspace(str[i - 1]))
    --i;

  // delete trailing spaces
  if (i < j)
    str.erase(i, j);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

class ServiceForwarding {
public:
  ServiceForwarding(const std::string &serviceName, WsServer &server);
  std::unordered_set<std::shared_ptr<WsServer::Connection>> &connections();
  void forward(const Buffer &contents);

private:
  std::string m_serviceName;
  WsServer &m_server;
  std::unordered_set<std::shared_ptr<WsServer::Connection>> m_connections;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

inline ServiceForwarding::ServiceForwarding(const std::string &serviceName, WsServer &server)
    : m_serviceName(serviceName), m_server(server) {
  /* nop */
}

//----------------------------------------------------------------------------------

std::unordered_set<std::shared_ptr<WsServer::Connection>> &ServiceForwarding::connections() {
  return m_connections;
}

//----------------------------------------------------------------------------------

void ServiceForwarding::forward(const Buffer &contents) {
  auto connections = this->connections();

  auto message_stream = std::make_shared<WsServer::SendStream>();
  *message_stream << m_serviceName;
  *message_stream << std::string(MAX_NAME - m_serviceName.size(), ' ');
  message_stream->write(contents.begin(), contents.size());

  // std::cout << "Sending service data. Service : " << m_serviceName << " , data : " << contents << std::endl;
  m_server.forward(connections, message_stream);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

class ServiceManager {
public:
  ServiceManager(Client &client, WsServer &server, Endpoint &serviceEndpoint);

  void addConnection(const std::string &serviceName, std::shared_ptr<WsServer::Connection> connection);
  void removeConnection(const std::string &serviceName, std::shared_ptr<WsServer::Connection> connection);
  void removeConnection(std::shared_ptr<WsServer::Connection> connection);

private:
  Client &m_client;
  WsServer &m_server;
  Endpoint &m_serviceEndpoint;

  typedef std::map<std::string, ServiceForwarding *> ServiceForwardingMap;
  ServiceForwardingMap m_serviceConnections;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

inline ServiceManager::ServiceManager(Client &client, WsServer &server, Endpoint &serviceEndpoint)
    : m_client(client), m_server(server), m_serviceEndpoint(serviceEndpoint) {
  ServiceManager &me = *this;
  m_serviceEndpoint.on_open = [](std::shared_ptr<WsServer::Connection> connection) {
    std::cout << "New web connection" << std::endl;
  };

  m_serviceEndpoint.on_close = [this](std::shared_ptr<WsServer::Connection> connection, int status,
                                      const std::string &reason) {
    std::cout << "Removing connection. status : " << status << ", reason : " << reason << std::endl;

    this->removeConnection(connection);
  };

  m_serviceEndpoint.on_message = [this](std::shared_ptr<WsServer::Connection> connection,
                                        std::shared_ptr<WsServer::Message> message) {
    if (message->size() < MAX_NAME) {
      std::cout << "Wrong message size (" << message->size() << "), expecting > " << MAX_NAME << std::endl;
      return;
    }

    // Extract command name and content
    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string serviceName(messageStr, 0, MAX_NAME);
    trim(serviceName);

    std::string action(messageStr, MAX_NAME);

    if (action == "subscribe") {
      this->addConnection(serviceName, connection);
      std::cout << "Got new subscription for service : " << serviceName << std::endl;
    } else if (action == "unsubscribe") {
      this->removeConnection(serviceName, connection);
    } else {
      std::cout << "ServiceManager (on_message) : Unknown action '" << action << "'" << std::endl;
    }
  };
}

//----------------------------------------------------------------------------------

inline void ServiceManager::addConnection(const std::string &serviceName,
                                          std::shared_ptr<WsServer::Connection> connection) {
  auto iter = m_serviceConnections.find(serviceName);

  // no subscription for this service yet
  // subscribe to service !
  if (m_serviceConnections.end() == iter) {
    ServiceForwarding *srvFwd = new ServiceForwarding(serviceName, m_server);
    iter = m_serviceConnections.insert(ServiceForwardingMap::value_type(serviceName, srvFwd)).first;

    m_client.subscribe(serviceName, srvFwd, &ServiceForwarding::forward);
  }

  // add this connection to service
  iter->second->connections().insert(connection);
}

//----------------------------------------------------------------------------------

inline void ServiceManager::removeConnection(const std::string &serviceName,
                                             std::shared_ptr<WsServer::Connection> connection) {
  auto iter = m_serviceConnections.find(serviceName);

  if (m_serviceConnections.end() != iter) {
    iter->second->connections().erase(connection);

    if (iter->second->connections().empty()) {
      m_client.unsubscribe(iter->first, iter->second, &ServiceForwarding::forward);
      delete iter->second;
      m_serviceConnections.erase(iter);
    }
  }
}

//----------------------------------------------------------------------------------

inline void ServiceManager::removeConnection(std::shared_ptr<WsServer::Connection> connection) {
  std::set<std::string> servicesRemoval;

  for (auto iter = m_serviceConnections.begin(); iter != m_serviceConnections.end(); ++iter) {
    iter->second->connections().erase(connection);

    // if no connection remaining, unsubscribe from service
    if (iter->second->connections().empty())
      servicesRemoval.insert(iter->first);
  }

  for (auto serviceName : servicesRemoval) {
    auto iter = m_serviceConnections.find(serviceName);

    if (m_serviceConnections.end() == iter)
      continue;

    m_client.unsubscribe(iter->first, iter->second, &ServiceForwarding::forward);
    delete iter->second;
    m_serviceConnections.erase(iter);
  }
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

Endpoint &addEndpoint(WsServer &server, const std::string &endpoint) {
  std::cout << "Adding endpoint : " << endpoint << std::endl;
  return server.endpoint[endpoint];
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

int main(int argc, char **argv) {
  // websocket port
  const int port = argc > 1 ? atoi(argv[1]) : 2506;

  WsServer webServer;
  webServer.config.port = port;

  Client client;

  /* Ping endpoint */
  auto &ping = addEndpoint(webServer, "^/dqmnet/ping/?$");

  ping.on_message = [&webServer](std::shared_ptr<WsServer::Connection> connection,
                                 std::shared_ptr<WsServer::Message> message) {
    std::cout << "Received ping, sending pong ..." << std::endl;
    auto sendStream = std::make_shared<WsServer::SendStream>();
    webServer.send(connection, sendStream, [](const boost::system::error_code &) {});
  };

  auto &browserGetServices = addEndpoint(webServer, "^/dqmnet/browser/getServices?$");

  browserGetServices.on_message = [&webServer](std::shared_ptr<WsServer::Connection> connection,
                                               std::shared_ptr<WsServer::Message> message) {

    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string uid(messageStr, 0, maxNumberStrLen);
    std::string serviceRegex(messageStr, maxNumberStrLen);

    DimBrowser browser;
    int nServices = browser.getServices(serviceRegex.c_str());

    char *service, *format;
    int type;
    Json::Value jsonStream, servicesValue(Json::arrayValue);
    unsigned int index = 0;

    while ((type = browser.getNextService(service, format))) {
      Json::Value serviceValue;
      serviceValue["service"] = service;
      serviceValue["format"] = format;
      serviceValue["type"] = type;

      servicesValue[index] = serviceValue;
      index++;
    }

    jsonStream["services"] = servicesValue;
    Json::FastWriter writer;
    std::string ret = writer.write(jsonStream);

    std::cout << "GetServices: Sending back -> " << ret << std::endl;

    auto send_stream = std::make_shared<WsServer::SendStream>();
    *send_stream << uid;
    *send_stream << ret;

    webServer.send(connection, send_stream, nullptr);
  };

  auto &browserGetServers = addEndpoint(webServer, "^/dqmnet/browser/getServers?$");

  browserGetServers.on_message = [&webServer](std::shared_ptr<WsServer::Connection> connection,
                                              std::shared_ptr<WsServer::Message> message) {

    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string uid(messageStr, 0, maxNumberStrLen);

    DimBrowser browser;
    browser.getServers();

    char *srv, *node;
    int pid;
    Json::Value jsonStream, serversValue(Json::arrayValue);
    unsigned int index = 0;

    while (browser.getNextServer(srv, node, pid)) {
      Json::Value serverValue;
      serverValue["server"] = srv;
      serverValue["node"] = node;
      serverValue["pid"] = pid;

      serversValue[index] = serverValue;
      index++;
    }

    jsonStream["servers"] = serversValue;
    Json::FastWriter writer;
    std::string ret = writer.write(jsonStream);

    auto send_stream = std::make_shared<WsServer::SendStream>();
    *send_stream << uid;
    *send_stream << ret;

    webServer.send(connection, send_stream, nullptr);
  };

  auto &browserGetServerServices = addEndpoint(webServer, "^/dqmnet/browser/getServerServices?$");

  browserGetServerServices.on_message = [&webServer](std::shared_ptr<WsServer::Connection> connection,
                                                     std::shared_ptr<WsServer::Message> message) {

    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string uid(messageStr, 0, maxNumberStrLen);
    std::string serviceRegex(messageStr, maxNumberStrLen);

    DimBrowser browser;
    browser.getServerServices(serviceRegex.c_str());

    char *service, *format;
    int type;
    Json::Value jsonStream, servicesValue(Json::arrayValue);
    unsigned int index = 0;

    while ((type = browser.getNextServerService(service, format))) {
      Json::Value serviceValue;
      serviceValue["service"] = service;
      serviceValue["format"] = format;
      serviceValue["type"] = type;

      servicesValue[index] = serviceValue;
      index++;
    }

    jsonStream["serverServices"] = servicesValue;
    Json::FastWriter writer;
    std::string ret = writer.write(jsonStream);

    auto send_stream = std::make_shared<WsServer::SendStream>();
    *send_stream << uid;
    *send_stream << ret;

    webServer.send(connection, send_stream, nullptr);
  };

  auto &browserGetServerClients = addEndpoint(webServer, "^/dqmnet/browser/getServerClients?$");

  browserGetServerClients.on_message = [&webServer](std::shared_ptr<WsServer::Connection> connection,
                                                    std::shared_ptr<WsServer::Message> message) {

    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string uid(messageStr, 0, maxNumberStrLen);
    std::string serviceRegex(messageStr, maxNumberStrLen);

    DimBrowser browser;
    browser.getServerClients(serviceRegex.c_str());

    char *client, *node;
    Json::Value jsonStream, serversValue(Json::arrayValue);
    unsigned int index = 0;

    while (browser.getNextServerClient(client, node)) {
      Json::Value serverValue;
      serverValue["client"] = client;
      serverValue["node"] = node;

      serversValue[index] = serverValue;
      index++;
    }

    jsonStream["serverClients"] = serversValue;
    Json::FastWriter writer;
    std::string ret = writer.write(jsonStream);

    auto send_stream = std::make_shared<WsServer::SendStream>();
    *send_stream << uid;
    *send_stream << ret;

    webServer.send(connection, send_stream, nullptr);
  };

  /* Endpoint for commands */
  auto &command = addEndpoint(webServer, "^/dqmnet/command/?$");

  command.on_message = [&webServer, &client](std::shared_ptr<WsServer::Connection> connection,
                                             std::shared_ptr<WsServer::Message> message) {

    if (message->size() < MAX_NAME) {
      std::cout << "Wrong message size (" << message->size() << "), expecting > " << MAX_NAME << std::endl;
      return;
    }

    // Extract command name and content
    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string commandName(messageStr, 0, MAX_NAME);
    trim(commandName);
    std::string buffer(messageStr, MAX_NAME);

    // Send command
    client.sendCommand(commandName, buffer, true);
  };

  /* Endpoint for rpc */
  auto &rpc = addEndpoint(webServer, "^/dqmnet/rpc/?$");

  rpc.on_message = [&webServer, &client](std::shared_ptr<WsServer::Connection> connection,
                                         std::shared_ptr<WsServer::Message> message) {

    if (message->size() < MAX_NAME + maxNumberStrLen) {
      std::cout << "Wrong message size (" << message->size() << "), expecting > " << MAX_NAME + maxNumberStrLen
                << std::endl;
      return;
    }

    // Extract command name and content
    std::string messageStr(message->buffer()->begin_iptr(), message->size());
    std::string rpcName(messageStr, 0, MAX_NAME);
    std::string rpcUid(messageStr, MAX_NAME, maxNumberStrLen);
    trim(rpcName);
    std::string buffer(messageStr, MAX_NAME + maxNumberStrLen);
    std::string response;

    std::cout << "Rpc name : " << rpcName << std::endl;
    std::cout << "Rpc uid" << rpcUid << std::endl;
    std::cout << "Rpc buffer : " << buffer << std::endl;

    // Send command
    try {
      client.sendRequest(rpcName, buffer, response);
    } catch (...) {
      std::cout << "Error occured on sending request" << std::endl;
    }

    auto sendStream = std::make_shared<WsServer::SendStream>();
    *sendStream << rpcUid;
    *sendStream << response;

    webServer.send(connection, sendStream);
  };

  /* Service manager handling the endpoint for services */
  auto &service = addEndpoint(webServer, "^/dqmnet/service/?$");
  ServiceManager serviceManager(client, webServer, service);

  /* Start web server */
  std::thread serverThread([&webServer]() { webServer.start(); });

  std::this_thread::sleep_for(std::chrono::seconds(2));
  serverThread.join();

  return 0;
}
