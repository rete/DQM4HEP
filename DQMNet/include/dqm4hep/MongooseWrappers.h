

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_MONGOOSEWRAPPER_H
#define DQM4HEP_MONGOOSEWRAPPER_H

#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/HttpStatusCodes.h>

namespace dqm4hep {
  
  namespace net {
    
    /**
     *  @brief  HttpQueryString class.
     *  Wraps an http query message and provides a simple API to get parameters
     */
    class HttpQueryString {
    public:
      /**
       *  @brief  Constructor
       *  
       *  @param str the input string describing the http query string
       *  @param len the length of the query string
       */
      HttpQueryString(const char *str, size_t len) :
      m_queryString(str, len) {
        core::StringVector tokens;
        core::tokenize(m_queryString, tokens, "&");
        for(auto token : tokens) {
          core::StringVector argVal;
          core::tokenize(token, argVal, "=");
          if(argVal.empty() or argVal.size() > 2) {
            continue;
          }
          m_parameters[argVal[0]] = (argVal.size() == 2) ? argVal[1] : "";
        }
      }
      
      /**
       *  @brief  Get the query string as a std::string
       */
      inline std::string str() const {
        return m_queryString;
      }
      
      /**
       *  @brief  Get a map containing all parameters in the query string.
       *  Parameters without value are also present in the map
       */
      inline const core::StringMap& parameters() const {
        return m_parameters;
      }
      
      /**
       *  @brief  Extract a specific parameter value from the query string
       * 
       *  @param  key the parameter key
       */
      inline std::string parameter(const std::string &key) const {
        auto iter = m_parameters.find(key);
        return (iter != m_parameters.end()) ? iter->second : "";
      }
      
      /**
       *  @brief  Extract a specific parameter value from the query string and convert it to requested type
       * 
       *  @param  key the parameter key
       */
      template <typename T>
      inline T parameterAs(const std::string &key) const {
        T param;
        core::stringToType(parameter(key), param);
        return param;
      }
      
    private:
      /// The input query string as supplied in the constructor
      std::string                   m_queryString = {};
      /// The parameter map parsed in constructor
      core::StringMap               m_parameters = {};
    };
    
    
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
    
    /**
     *  @brief  HttpContentType class.
     *  List a few relevant http content type to be used in http message and response
     */
    class HttpContentType {
    public:
      /// Plain text message
      static constexpr const char* PlainText = "text/plain";
      /// Message containing HTML data
      static constexpr const char *HtmlText = "text/html";
      /// JSON message
      static constexpr const char *Json = "application/json";
      /// XML message
      static constexpr const char *Xml = "application/xml";
      /// Binary data
      static constexpr const char *Binary = "application/octet-stream";
    };
    
    /**
     *  @brief  HttpResponse class
     */
    class HttpResponse {
    public:
      HttpResponse() = default;
      
      /**
       *  @brief  Get the response string
       */
      inline const std::string& response() const {
        return m_response;
      }
      
      /**
       *  @brief  Get the response string
       */
      inline std::string& response() {
        return m_response;
      }
      
      /**
       *  @brief  Get the http content type (see HttpContentType class).
       */
      inline const std::string& contentType() const {
        return m_contentType;
      }
      
      /**
       *  @brief  Set the http content type (see HttpContentType class).
       *
       *  @param  type the http content type
       */
      inline void setContentType(const std::string& type) {
        m_contentType = type;
      }
      
      /**
       *  @brief  Get the http status code (see HttpStatusCode enum).
       */
      inline HttpStatusCode code() const {
        return m_httpCode;
      }
      
      /**
       *  @brief  Set the http status code (see HttpStatusCode enum).
       *
       *  @param  code the http status code
       */
      inline void setCode(HttpStatusCode code) {
        m_httpCode = code;
      }

    private:
      /// The response string content
      std::string             m_response = {};
      /// The http content type
      std::string             m_contentType = {HttpContentType::PlainText};
      /// The http status code
      HttpStatusCode          m_httpCode = {HttpStatusCode::OK};
    };
    
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
    
    class Connection {
    public:
      inline Connection(const std::string& uri, struct mg_connection *connection) :
        m_connection(connection),
        m_uri(uri) {
        
      }
      
      inline sock_t socket() const {
        return m_connection->sock;
      }
      
      inline mg_mgr *manager() const {
        return m_connection->mgr;
      }
      
      inline const std::string &uri() const {
        return m_uri;
      }
      
      inline struct mg_connection* connection() const {
        return m_connection;
      }
      
      template <typename T>
      inline T* connectionAs() const {
        return (T*) m_connection->user_data;
      }
      
    private:
      struct mg_connection*       m_connection = {nullptr};
      std::string                 m_uri = {};
      bool                        m_handshakeDone = {false};
    };
    
  }

}

#endif  // DQM4HEP_MONGOOSEWRAPPER_H