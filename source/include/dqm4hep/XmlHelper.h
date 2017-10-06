/// \file DQMXmlHelper.h
/*
 *
 * DQMXmlHelper.h header template automatically generated by a class generator
 * Creation date : mar. juil. 1 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef DQM4HEP_XMLHELPER_H
#define DQM4HEP_XMLHELPER_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/tinyxml.h"
#include "dqm4hep/Logging.h"

namespace dqm4hep {

  namespace core {

    class Module;

    /** XmlHelper class
     */
    class XmlHelper
    {
    public:
      /**
       */
      static StatusCode readXmlFile(const std::string &fileName, TiXmlDocument &document, StringMap &constants, bool processIncludes = true);

      /** Read a value from an xml element
       */
      template <typename T>
      static StatusCode readValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t);

      /** Read a vector of values from a (space separated) list in an xml element
       */
      template <typename T>
      static StatusCode readVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector);

      /** Read a two-dimensional array of values into a vector of vectors. Each row of values must be contained
       *          within <rowname></rowname> xml tags, whilst the values in the row must be space separated
       */
      template <typename T>
      static StatusCode read2DVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, const std::string &rowName,
          std::vector< std::vector<T> > &vector);

      /** Get the attribute of the xml element
       */
      template <typename T>
      static StatusCode getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName, T &attributeValue);

      /** Get the attribute of the xml element and use a validator to validate the value
       */
      template <typename T, typename Validator>
      static StatusCode getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName, T &attributeValue, Validator validator);

      /** Read a parameter value from an xml element
       */
      template <typename T>
      static StatusCode readParameterValue(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t);

      /** Read a parameter value from an xml element and use a validator to validate the value
       */
      template <typename T, typename Validator>
      static StatusCode readParameterValue(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t, Validator validator);

      /** Read a vector of values for a parameter from a (space separated) list in an xml element
       */
      template <typename T>
      static StatusCode readParameterValues(const TiXmlHandle &xmlHandle, const std::string &parameterName, std::vector<T> &vector);

      /** Read a vector of values for a parameter from a (space separated) list in an xml element
       */
      template <typename T, typename Validator>
      static StatusCode readParameterValues(const TiXmlHandle &xmlHandle, const std::string &parameterName, std::vector<T> &vector, Validator validator);

      // /** Create a quality test. Works if the quality test factory has been registered first
      //  */
      // static StatusCode createQualityTest(const Module *const pModule, const TiXmlHandle &xmlHandle, const std::string &qualityTestName);
      //
      // /** Create a monitor element from a xml handle
      //  */
      // static StatusCode bookMonitorElement(const Module *const pModule, const TiXmlHandle &xmlHandle, const std::string &meStringId,
      //     MonitorElementPtr &monitorElement);
      //
      // /** Create a monitor element from a xml handle
      //  */
      // static StatusCode bookMonitorElement(const Module *const pModule, const TiXmlHandle &xmlHandle, const std::string &meStringId,
      //     const std::string &strSuffix, MonitorElementPtr &monitorElement);
      //
      // /** Create a monitor element from a xml handle
      //  */
      // static StatusCode bookMonitorElement(const Module *const pModule, const TiXmlHandle &xmlHandle, const std::string &meStringId,
      //     unsigned int suffix, MonitorElementPtr &monitorElement);
      //
      // /** Create a monitor element from a xml handle and configure name, path and title from parameters (see DQM4HEP::replace())
      //  */
      // static StatusCode bookMonitorElement(const Module *const pModule, const TiXmlHandle &xmlHandle, const std::string &meStringId,
      //     MonitorElementPtr &monitorElement, const ParameterMap &parameters);

      /** Replace all xml attribute recursively from the parameters map
       */
      static StatusCode replaceAllXmlAttributes(TiXmlElement *pXmlElement, const ParameterMap &parameters);

    private:
      static StatusCode processConstants(const std::string &fileName, TiXmlElement *parent, StringMap &constants, bool parseIncludes = true);
      static StatusCode processConstant(TiXmlElement *parent, StringMap &constants);
      static StatusCode performConstantReplacement(std::string &value, const StringMap &constants);
      static StatusCode processIncludeElements(const std::string &fileName, TiXmlElement* element , const StringMap& constants);
      static StatusCode processIncludeElement(const std::string &fileName, TiXmlElement* element , const StringMap& constants , TiXmlDocument &document);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readValue(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, T &t)
    {
      const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      if (!DQM4HEP::stringToType(pXmlElement->GetText(), t))
        return STATUS_CODE_FAILURE;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline StatusCode XmlHelper::readValue<bool>(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, bool &t)
    {
      const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      const std::string xmlElementString = pXmlElement->GetText();

      if ((xmlElementString == "1") || (xmlElementString == "true"))
      {
        t = true;
      }
      else if ((xmlElementString == "0") || (xmlElementString == "false"))
      {
        t = false;
      }
      else
      {
        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, std::vector<T> &vector)
    {
      const TiXmlElement *const pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      StringVector tokens;
      DQM4HEP::tokenize(pXmlElement->GetText(), tokens);

      for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter)
      {
        T t;

        if (!DQM4HEP::stringToType(*iter, t))
          return STATUS_CODE_FAILURE;

        vector.push_back(t);
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::read2DVectorOfValues(const TiXmlHandle &xmlHandle, const std::string &xmlElementName, const std::string &rowName,
        std::vector< std::vector<T> > &vector)
    {
      TiXmlElement *pXmlElement = xmlHandle.FirstChild(xmlElementName).Element();

      if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

      TiXmlElement *pXmlRowElement = TiXmlHandle(pXmlElement).FirstChild(rowName).Element();

      if (NULL == pXmlRowElement)
        return STATUS_CODE_NOT_FOUND;

      for ( ; NULL != pXmlRowElement; pXmlRowElement = pXmlRowElement->NextSiblingElement(rowName))
      {
        std::vector<T> rowVector;

        StringVector tokens;
        DQM4HEP::tokenize(pXmlRowElement->GetText(), tokens);

        for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter)
        {
          T t;

          if (!DQM4HEP::stringToType(*iter, t))
            return STATUS_CODE_FAILURE;

          rowVector.push_back(t);
        }

        vector.push_back(rowVector);
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName, T &attributeValue)
    {
      if(NULL == pXmlElement)
        return STATUS_CODE_INVALID_PTR;

      const char *pAttributePtr(pXmlElement->Attribute(attributeName.c_str()));

      if(NULL == pAttributePtr)
        return STATUS_CODE_NOT_FOUND;

      std::string attributeStr(pAttributePtr);

      if(!DQM4HEP::stringToType(attributeStr, attributeValue))
        return STATUS_CODE_FAILURE;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T, typename Validator>
    inline StatusCode XmlHelper::getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName, T &attributeValue, Validator validator)
    {
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute<T>(pXmlElement, attributeName, attributeValue));

      if(!validator(attributeValue))
        return STATUS_CODE_INVALID_PARAMETER;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline StatusCode XmlHelper::getAttribute(const TiXmlElement *const pXmlElement, const std::string &attributeName, std::string &attributeValue)
    {
      if(NULL == pXmlElement)
        return STATUS_CODE_INVALID_PTR;

      const char *pAttributePtr(pXmlElement->Attribute(attributeName.c_str()));

      if(NULL == pAttributePtr)
        return STATUS_CODE_NOT_FOUND;

      attributeValue = pAttributePtr;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readParameterValue(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t)
    {
      for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("parameter").Element(); NULL != pXmlElement;
          pXmlElement = pXmlElement->NextSiblingElement("parameter"))
      {
        std::string name;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));

        if(name != parameterName)
          continue;

        if (!DQM4HEP::stringToType(pXmlElement->GetText(), t))
          return STATUS_CODE_FAILURE;

        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T, typename Validator>
    inline StatusCode XmlHelper::readParameterValue(const TiXmlHandle &xmlHandle, const std::string &parameterName, T &t, Validator validator)
    {
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameterValue(xmlHandle, parameterName, t));

      if( ! validator(t) )
        return STATUS_CODE_INVALID_PARAMETER;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode XmlHelper::readParameterValues(const TiXmlHandle &xmlHandle, const std::string &parameterName, std::vector<T> &vector)
    {
      for (TiXmlElement *pXmlElement = xmlHandle.FirstChild("parameter").Element(); NULL != pXmlElement;
          pXmlElement = pXmlElement->NextSiblingElement("parameter"))
      {
        std::string name;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));

        if(name != parameterName)
          continue;

        StringVector tokens;
        DQM4HEP::tokenize(pXmlElement->GetText(), tokens);

        for (StringVector::const_iterator iter = tokens.begin(), iterEnd = tokens.end(); iter != iterEnd; ++iter)
        {
          T t;

          if (!DQM4HEP::stringToType(*iter, t))
            return STATUS_CODE_FAILURE;

          vector.push_back(t);
        }

        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T, typename Validator>
    inline StatusCode XmlHelper::readParameterValues(const TiXmlHandle &xmlHandle, const std::string &parameterName, std::vector<T> &vector, Validator validator)
    {
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameterValues(xmlHandle, parameterName, vector));

      if( ! validator(vector) )
        return STATUS_CODE_INVALID_PARAMETER;

      return STATUS_CODE_SUCCESS;
    }

  }

}

#endif //  DQM4HEP_XMLHELPER_H
