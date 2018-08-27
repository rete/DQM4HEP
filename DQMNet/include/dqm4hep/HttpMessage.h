

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_HTTPMESSAGE_H
#define DQM4HEP_HTTPMESSAGE_H

// -- mongoose headers
#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/HttpQueryString.h>

namespace dqm4hep {
  
  namespace net {
    
    /**
     *  @brief  HttpMessage class
     *  Describe a received http message 
     */
    class HttpMessage {
    public:
      /**
       *  @brief  Constructor
       * 
       *  @param  msg the mongoose http_message ptr
       */
      inline HttpMessage(struct http_message *msg) :
        m_httpMessage(msg),
        m_queryString((msg != nullptr) ? std::string(msg->query_string.p, msg->query_string.len) : "") {

      }
      
      /**
       *  @brief  Whether the message is valid
       */
      inline bool isValid() const {
        return (m_httpMessage != nullptr);
      }
      
      /**
       *  @brief  Get the full message buffer (request line, headers and body)
       */
      inline const char* message() const {
        return isValid() ? m_httpMessage->message.p : nullptr;
      }
      
      /**
       *  @brief  Get the full message length (request line, headers and body)
       */
      inline size_t messageSize() const {
        return isValid() ? m_httpMessage->message.len : 0;
      }
      
      /**
       *  @brief  Get the body message buffer (without request line and headers)
       */
      inline const char* body() const {
        return isValid() ? m_httpMessage->body.p : nullptr;
      }
      
      /**
       *  @brief  Get the body message length (without request line and headers)
       */
      inline size_t bodySize() const {
        return isValid() ? m_httpMessage->body.len : 0;
      }
      
      /**
       *  @brief  Get the http method (e.g "GET", "POST")
       */
      inline std::string method() const {
        return isValid() ? std::string(m_httpMessage->method.p, m_httpMessage->method.len) : "";
      }
      
      /**
       *  @brief  Get the http URI (e.g "/file.html")
       */
      inline std::string uri() const {
        return isValid() ? std::string(m_httpMessage->uri.p, m_httpMessage->uri.len) : "";
      }
      
      /**
       *  @brief  Get the status message
       */
      inline std::string statusMessage() const {
        return isValid() ? std::string(m_httpMessage->resp_status_msg.p, m_httpMessage->resp_status_msg.len) : "";
      }
      
      /**
       *  @brief  Get the response code
       */
      inline int responseCode() const {
        return isValid() ? m_httpMessage->resp_code : -1;
      }
      
      /**
       *  @brief  Get the http protocol (e.g "HTTP/1.1")
       */
      inline std::string protocol() const {
        return isValid() ? std::string(m_httpMessage->proto.p, m_httpMessage->proto.len) : "";
      }
      
      /**
       *  @brief  Get the http query string
       */
      inline const HttpQueryString& queryString() const {
        return m_queryString;
      }
      
    private:
      /// The input mongoose http message
      struct http_message*          m_httpMessage = {nullptr};
      /// The http query string object
      HttpQueryString               m_queryString;
    };
    
  }

}

#endif  // DQM4HEP_HTTPMESSAGE_H