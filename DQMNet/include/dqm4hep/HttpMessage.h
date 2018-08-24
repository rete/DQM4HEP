

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
        m_queryString(msg->query_string.p, msg->query_string.len) {

      }
      
      /**
       *  @brief  Get the full message buffer (request line, headers and body)
       */
      inline const char* message() const {
        return m_httpMessage->message.p;
      }
      
      /**
       *  @brief  Get the full message length (request line, headers and body)
       */
      inline size_t messageSize() const {
        return m_httpMessage->message.len;
      }
      
      /**
       *  @brief  Get the body message buffer (without request line and headers)
       */
      inline const char* body() const {
        return m_httpMessage->body.p;
      }
      
      /**
       *  @brief  Get the body message length (without request line and headers)
       */
      inline size_t bodySize() const {
        return m_httpMessage->body.len;
      }
      
      /**
       *  @brief  Get the http method (e.g "GET", "POST")
       */
      inline std::string method() const {
        return std::string(m_httpMessage->method.p, m_httpMessage->method.len);
      }
      
      /**
       *  @brief  Get the http URI (e.g "/file.html")
       */
      inline std::string uri() const {
        return std::string(m_httpMessage->uri.p, m_httpMessage->uri.len);
      }
      
      /**
       *  @brief  Get the http protocol (e.g "HTTP/1.1")
       */
      inline std::string protocol() const {
        return std::string(m_httpMessage->proto.p, m_httpMessage->proto.len);
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
      HttpQueryString               m_queryString;
    };
    
  }

}

#endif  // DQM4HEP_HTTPMESSAGE_H