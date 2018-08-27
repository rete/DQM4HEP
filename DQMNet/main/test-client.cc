//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include <dqm4hep/Logging.h>
#include <dqm4hep/HttpStatusCodes.h>
#include <dqm4hep/NetworkCommon.h>
#include <dqm4hep/HttpMessage.h>

using namespace dqm4hep::net;

int main(int argc, char ** argv) {
  if(argc < 2) {
    std::cout << "Usage: test-client uri" << std::endl;
    return 1;
  }
  
  std::string uri(argv[1]);

  Utils::httpGet(uri, [](const HttpMessage& message){
    if(message.isValid()) {
      dqm_info( "---------------------------" );
      dqm_info( "-- Received HTTP response --" );
      dqm_info( "---------------------------" );    
      dqm_info( "-- Method: {0}", message.method() );
      dqm_info( "-- Protocol: {0}", message.protocol() );
      dqm_info( "-- URI: {0}", message.uri() );
      dqm_info( "-- Code: {0}", reasonPhrase(message.responseCode()) );
      dqm_info( "-- Query string: {0}", message.queryString().str() );
      dqm_info( "-- Body: " );
      dqm_info( std::string(message.body(), message.bodySize()) );
      dqm_info( "---------------------------" );
    }
    else {
      dqm_info("Invalid http message");
    }
  });
  
  return 0;
}
