

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_WEBSOCKETMESSAGE_H
#define DQM4HEP_WEBSOCKETMESSAGE_H

// -- mongoose headers
#include <mongoose.h>

namespace dqm4hep {
  
  namespace net {
    
    /**
     *  @brief  WebsocketMessage class
     */
    class WebsocketMessage {
    public:
      /**
       *  @brief  Constructor
       * 
       *  @param  wm the mongoose websocket ptr
       */
      inline WebsocketMessage(struct websocket_message* wm) :
        m_message(wm) {
        
      }
      
      /**
       *  @brief  Get the websocket message buffer
       */
      inline const char* message() const {
        return (const char*)m_message->data;
      }
      
      /**
       *  @brief  Get the websocket message buffer length
       */
      inline size_t length() const {
        return m_message->size;
      }
      
    private:
      /// The input mongoose websocket message
      struct websocket_message* m_message = {nullptr};
    };
    
  }

}

#endif  // DQM4HEP_WEBSOCKETMESSAGE_H