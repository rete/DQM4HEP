//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/ServerConnection.h>

using namespace dqm4hep::net;

int main(int argc, char ** argv) {
  
  NetworkEventLoop eventLoop;
  ServerConnection connection(&eventLoop);
  
  connection.onHttpRequest([](const HttpMessage& message, HttpResponse& response){
    dqm_info( "---------------------------" );
    dqm_info( "-- Received HTTP request --" );
    dqm_info( "---------------------------" );    
    dqm_info( "-- Method: {0}", message.method() );
    dqm_info( "-- Protocol: {0}", message.protocol() );
    dqm_info( "-- URI: {0}", message.uri() );
    dqm_info( "-- Query string: {0}", message.queryString().str() );
    dqm_info( "-- Body: " );
    dqm_info( std::string(message.body(), message.bodySize()) );
    dqm_info( "---------------------------" );
    response.response().append("Hello world !"); 
    dqm_info( "-- Sending back '{0}'", response.response() );
    dqm_info( "---------------------------" );
  });
  
  connection.onNewConnection([](const Connection& con) {
    dqm_info( "-- New connection, uri: {0}", con.uri() );
  });
  
  connection.onConnectionClose([](const Connection& con) {
    dqm_info( "-- Closing connection, uri: {0}", con.uri() );
  });
  
  connection.onMessage([&eventLoop,&connection](const Connection& con, const WebsocketMessage& message) {
    dqm_info( "-- Received message, uri: {0}", con.uri() );
    std::string inmsg(message.message(), message.length());
    dqm_info( "-- Message: {0}", inmsg );
    if(inmsg == "stop") {
      connection.stop();
      eventLoop.stop();
    }
    else {
      connection.send(con, "Boubouuuuuuuu !");
      connection.close(con);      
    }
  });
  
  ServerConnection::BindConfig config;
  config.m_port = argc > 1 ? atoi(argv[1]) : 8000;
  config.m_enableWebsockets = true;
  config.m_enableHttpRequests = true;
  
  connection.bind(config);
  eventLoop.start(true, 200);
  
  return 0;
}
