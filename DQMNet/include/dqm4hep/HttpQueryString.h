

//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_HTTPQUERYSTRING_H
#define DQM4HEP_HTTPQUERYSTRING_H

// -- mongoose headers
#include <mongoose.h>

// -- dqm4hep headers
#include <dqm4hep/Internal.h>

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
       */
      HttpQueryString(const std::string& str) :
        m_queryString(str) {
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
    
  }

}

#endif  // DQM4HEP_HTTPQUERYSTRING_H