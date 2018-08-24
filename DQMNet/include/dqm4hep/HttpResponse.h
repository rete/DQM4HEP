

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_HTTPRESPONSE_H
#define DQM4HEP_HTTPRESPONSE_H

// -- std headers
#include <string>

// -- dqm4hep headers
#include <dqm4hep/HttpStatusCodes.h>
#include <dqm4hep/HttpContentType.h>

namespace dqm4hep {
  
  namespace net {
    
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
    
  }

}

#endif  // DQM4HEP_HTTPRESPONSE_H
