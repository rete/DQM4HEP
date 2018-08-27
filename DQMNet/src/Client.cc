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
#include <dqm4hep/Client.h>
#include <dqm4hep/Logging.h>

namespace dqm4hep {

  namespace net {

    Client::Client(NetworkEventLoop *eventLoop) :
      m_eventLoop(eventLoop) {
      
    }
    
  }
  
}
