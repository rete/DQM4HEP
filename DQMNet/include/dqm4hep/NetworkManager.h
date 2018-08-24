//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_NETWORKMANAGER_H
#define DQM4HEP_NETWORKMANAGER_H

#include <mongoose.h>

#include <mutex>
#include <atomic>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>

namespace dqm4hep {
  
  namespace net {
  
    class NetworkManager {
    public:
      NetworkManager() = default;
      
      
    private:
      NetworkEventLoop            m_eventLoop = {};
    };
    
  }
  
}

#endif //  DQM4HEP_NETWORKMANAGER_H
