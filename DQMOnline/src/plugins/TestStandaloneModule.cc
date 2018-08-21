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
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Cycle.h"
#include "dqm4hep/Module.h"
#include "dqm4hep/PluginManager.h"
#include "dqm4hep/ModuleApi.h"

#include <random>
#include <cmath>

namespace dqm4hep {

  namespace online {

    class TestStandaloneModule : public StandaloneModule {
    public:      
      TestStandaloneModule() = default;
      ~TestStandaloneModule() = default;
    private:
      std::mt19937                 m_rndGen{};
      std::normal_distribution<>   m_temperatureDistribution{};
      std::normal_distribution<>   m_pressureDistribution{};
      float                        m_meanT = {24};
      float                        m_rmsT = {1};
      float                        m_meanP = {1024};
      float                        m_rmsP = {2};
      OnlineElementPtr             m_temperature = {nullptr};
      OnlineElementPtr             m_pressure = {nullptr};
    private:
      void readSettings(const core::TiXmlHandle &handle);
      void initModule();
      void startOfRun(core::Run &run);
      void startOfCycle();
      void endOfCycle(const EOCCondition &condition);
      void endOfRun(const core::Run &run);
      void endModule();
      void process();
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void TestStandaloneModule::readSettings(const core::TiXmlHandle &handle) {
      auto element = handle.Element();
      core::TiXmlPrinter printer;
      element->Accept(&printer);
      dqm_info( "Received the following XML settings:" );
      dqm_info( "{0}", printer.Str() );
      
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "TMean", m_meanT));
        
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "TRms", m_rmsT));
        
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "PMean", m_meanP));
        
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "PRms", m_rmsP));
    }
    
    //-------------------------------------------------------------------------------------------------

    void TestStandaloneModule::initModule() {
      dqm_info( "TestStandaloneModule::initModule" );

      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::mkdir(this, "Sensors"));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this, "Sensors"));
      
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookObject<core::TDynamicGraph>(this, 
        m_temperature, ".", "Temperature", "The temperature evolution"));
      m_temperature->objectTo<core::TDynamicGraph>()->SetRangeLength(60*60); // 1h range
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookObject<core::TDynamicGraph>(this, 
        m_pressure, ".", "Pressure", "The pressure evolution"));
      m_pressure->objectTo<core::TDynamicGraph>()->SetRangeLength(60*60); // 1h range
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::dump(this));
      
      m_temperatureDistribution.param(std::normal_distribution<>::param_type(m_meanT, m_rmsT));
      m_pressureDistribution.param(std::normal_distribution<>::param_type(m_meanP, m_rmsP));
    }

    //-------------------------------------------------------------------------------------------------

    void TestStandaloneModule::startOfRun(core::Run &run) {
      dqm_info( "=> TestStandaloneModule::startOfRun: {0}", core::typeToString(run) );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestStandaloneModule::startOfCycle() {
      dqm_info( "====> TestStandaloneModule::startOfCycle" );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestStandaloneModule::endOfCycle(const EOCCondition &condition) {
      dqm_info( "====> TestStandaloneModule::endOfCycle: {0}", core::typeToString(condition) );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestStandaloneModule::endOfRun(const core::Run &run) {
      dqm_info( "=> TestStandaloneModule::endOfRun: {0}", core::typeToString(run) );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestStandaloneModule::endModule() {
      dqm_info( "TestStandaloneModule::endModule");
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestStandaloneModule::process() {
      auto temperature = m_temperatureDistribution(m_rndGen);
      auto pressure = m_pressureDistribution(m_rndGen);
      auto now = time(0);
      m_temperature->objectTo<core::TDynamicGraph>()->AddPoint(now, temperature);
      m_pressure->objectTo<core::TDynamicGraph>()->AddPoint(now, pressure);
      dqm_info( "========> TestStandaloneModule::process: time is {0}, T={1}, P={2}", now, temperature, pressure);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    DQM_PLUGIN_DECL(TestStandaloneModule, "TestStandaloneModule");
  }

}
