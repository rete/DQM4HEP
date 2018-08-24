//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_NETWORKEVENTLOOP_H
#define DQM4HEP_NETWORKEVENTLOOP_H

#include <mongoose.h>

#include <atomic>

#include <dqm4hep/Internal.h>

namespace dqm4hep {
  
  namespace net {
  
    class NetworkEventLoop {
    public:
      typedef mg_mgr Manager; 
    public:
      NetworkEventLoop();
      ~NetworkEventLoop();
      
      void start(bool blocking = false, int milliseconds = 200);
      void stop();
      Manager *manager();
      bool running() const;
      
    private:
      void poll();
      static void *event_loop(void *param);
      
    private:
      struct mg_mgr                m_mgr = {};
      struct mg_connection*        m_connection = {nullptr};
      std::atomic_bool             m_running = {false};
      std::atomic_bool             m_stopFlag = {false};
      int                          m_pollTime = {100};
    };
    
  }
  
}

#endif //  DQM4HEP_NETWORKEVENTLOOP_H
