//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_CLIENT_H
#define DQM4HEP_CLIENT_H

#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/ClientConnection.h>

namespace dqm4hep {
  
  namespace net {
    
    /**
     *  @brief  Client class
     *  Main client interface for users
     */
    class Client {
    public:
      /**
       *  @brief  Constructor
       *
       *  @param  eventLoop the event loop in which the client runs
       */
      Client(NetworkEventLoop *eventLoop);
                  
    private:
      /// The event loop in which the client runs
      NetworkEventLoop*            m_eventLoop = {};
    };
    
  }
  
}

#endif //  DQM4HEP_CLIENT_H
