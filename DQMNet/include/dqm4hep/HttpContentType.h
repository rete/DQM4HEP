

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_HTTPCONTENTTYPE_H
#define DQM4HEP_HTTPCONTENTTYPE_H

namespace dqm4hep {
  
  namespace net {
    
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
    
  }

}

#endif  // DQM4HEP_HTTPCONTENTTYPE_H