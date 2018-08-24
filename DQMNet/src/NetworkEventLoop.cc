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
#include "dqm4hep/NetworkEventLoop.h"

namespace dqm4hep {

  namespace net {
    
    NetworkEventLoop::NetworkEventLoop() {
      mg_mgr_init(&m_mgr, this);
    }
    
    NetworkEventLoop::~NetworkEventLoop() {
      mg_mgr_free(&m_mgr);
    }
    
    void NetworkEventLoop::start(bool blocking, int milliseconds) {
      if(m_running.load()) {
        return;
      }
      m_pollTime = milliseconds;
      m_stopFlag = false;
      if(blocking) {
        poll();
      }
      else {
        mg_start_thread(&NetworkEventLoop::event_loop, this);
      }
    }
    
    void NetworkEventLoop::stop() {
      m_stopFlag = true;
    }
    
    NetworkEventLoop::Manager *NetworkEventLoop::manager() {
      return &m_mgr;
    }
    
    bool NetworkEventLoop::running() const {
      return m_running;
    }
    
    void NetworkEventLoop::poll() {
      m_running = true;
      while(!m_stopFlag.load()) {
        mg_mgr_poll(&m_mgr, m_pollTime);
      }
      m_running = false;
    }
    
    void *NetworkEventLoop::event_loop(void *param) {
      NetworkEventLoop *loop = (NetworkEventLoop *)param;
      loop->poll();
      return nullptr;
    }
    
  }
  
}
