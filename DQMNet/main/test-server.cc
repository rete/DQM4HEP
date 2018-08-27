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
#include <dqm4hep/Server.h>

using namespace dqm4hep::net;

int main(int argc, char ** argv) {
  
  NetworkEventLoop eventLoop;
  Server server(&eventLoop);
  
  server.setName("TEST");
  server.setUseNetworkManager(true);
  
  server.onHttpRequest([](const HttpMessage& message, HttpResponse& response){
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
  
  server.createService("/testint");
  server.createService("/testfloat");
  
  if(argc > 1) {
    server.start(atoi(argv[1]));
  }
  else {
    server.start();    
  }

  eventLoop.start(true, 200);
  
  return 0;
}
