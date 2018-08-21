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
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/QualityTest.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XmlHelper.h>

// -- root headers
#include <TH1.h>
#include <TF1.h>
#include <TF2.h>
#include <TF3.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMath.h>
#include <TFormula.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>

namespace dqm4hep {

  namespace core {

    /** 
     *  @brief  FitParamInRangeTest class
     *  
     *  Fit a user function on 
     *     - TGraph
     *     - TGraph2D
     *     - TH1
     *  and look if a chosen fit parameter is within a certain allowed range.
     */
    class FitParamInRangeTest : public QualityTest {
    public:
      /**
       *  @brief  Factory class to create quality test
       */
      class Factory : public QualityTestFactory {
      public:
        /**
         *  @brief  Create a quality test
         *   
         *  @param  qname The quality test name
         */
        QTestPtr createQualityTest(const std::string &qname) const override;
      };
      
      /**
       *  @brief  Constructor
       *
       *  @param  qname the quality test name
       */
      FitParamInRangeTest(const std::string &qname);
      
      /**
       *  @brief  Destructor
       */
      ~FitParamInRangeTest() override = default;
      
      /**
       *  @brief  Read settings from the xml handle
       *  
       *  @param  xmlHandle the xml handle to parse
       */
      StatusCode readSettings(const dqm4hep::core::TiXmlHandle xmlHandle) override;
      
      /**
       *  @brief  Run the quality test and get a quality test report from it
       *  
       *  @param  monitorElement the monitor element to test
       *  @param  report the quality test report to receive
       */
      void userRun(MonitorElement* monitorElement, QualityTestReport &report) override;
      
    private:
      /**
       *  @brief  Create the function to fit
       */
      TF1* createFunction() const;
      
      /**
       *  @brief  Check the input monitor element
       *  
       *  @param  monitorElement the monitor element to check
       */
      bool checkElement(MonitorElement* monitorElement) const;
      
      /**
       *  @brief  Check consistency between the monitor element and the fit function
       *  
       *  @param  monitorElement the input monitor element
       *  @param  function the fit function
       *  @param  report the quality test report to fill if error occurs 
       */
      bool checkElement(MonitorElement* monitorElement, TF1 *function, QReport &report) const;
      
      /**
       *  @brief  Get the fit options
       *  
       *  @param  isHistogram whether the fit object is an histogram
       *  @param  withSumW2 whether the histogram as the sumw2 set
       */
      std::string getFitOptions(bool isHistogram, bool withSumW2) const;
      
      /**
       *  @brief  Perform the fit
       *   
       *  @param  monitorElement the monitor element to fit
       *  @param  fitFunction the function to fit
       *  @param  report the quality test report to fill
       */
      TFitResultPtr performFit(MonitorElement* monitorElement, TF1 *fitFunction, QReport &report) const;
      
      /**
       *  @brief  Perform the range test
       *  
       *  @param  fitResult the actual fit result
       *  @param  report the quality test report to fill
       */
      void performRangeTest(TFitResultPtr fitResult, QReport &report) const;
      
      /**
       *  @brief  Test whether a value is within a certain range
       *
       *  @param  value the value to test
       *  @param  lower the lower range bound
       *  @param  upper the upper range bound
       */
      template <typename T>
      bool withinRange(T value, T lower, T upper) const;
      
    private:
      /// The TF1 formula to fit (mandatory)
      std::string                     m_fitFormula = {""};
      /// The list of function parameter to guess before fitting (optional)
      FloatVector                     m_guessParameters = {};
      /// The parameter id to test (mandatory)
      unsigned int                    m_testParameter = {0};
      /// The lower deviation bound allow for the test parameter (mandatory)
      double                          m_deviationLower = {0.};
      /// The upper deviation bound allow for the test parameter (mandatory)
      double                          m_deviationUpper = {0.};
      /// The function range for fitting (optional)
      FloatVector                     m_functionRange = {};
      /// Whether to use a log likelihood for fitting (optional)
      bool                            m_useLoglikelihood = {false};
      /// Whether to use a pearson chi2 for fitting
      bool                            m_usePearsonChi2 = {false};
      /// Whether to use the IMPROVE command in TMinuit fitter
      bool                            m_improveFitResult = {false};      
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    typedef FitParamInRangeTest::Factory FitParamInRangeTestFactory;

    //-------------------------------------------------------------------------------------------------

    inline QTestPtr FitParamInRangeTest::Factory::createQualityTest(const std::string &qname) const {
      return std::shared_ptr<QTest>(new FitParamInRangeTest(qname));
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    FitParamInRangeTest::FitParamInRangeTest(const std::string &qname)
        : QualityTest("FitParameters", qname) {
      m_description = std::string("Fit a user function on\n") + 
      "  - TGraph\n" +
      "  - TGraph2D\n" +
      "  - TH1\n" +
      "and look if a chosen fit parameter is within a certain allowed range.";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline bool FitParamInRangeTest::withinRange(T value, T lower, T upper) const {
      return (value > lower and value < upper);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode FitParamInRangeTest::readSettings(const TiXmlHandle xmlHandle) {   
      UInt_t nParameters = 0;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, 
        "FitFormula", m_fitFormula, [&nParameters,this](const std::string &value){
          if(value.empty()) {
            return false;
          }
          std::stringstream formulaName;
          formulaName << "Formula_RS_" << (void*)this;
          TFormula formula(formulaName.str().c_str(), value.c_str(), true);
          nParameters = formula.GetNpar();
          if(0 == nParameters) {
            return false;
          }
          return true;
        }));
      
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameters(xmlHandle, 
        "GuessParameters", m_guessParameters, [nParameters](const FloatVector &values){
          return (values.size() <= nParameters);
        }));
      
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, 
        "TestParameter", m_testParameter, [nParameters](unsigned int value){
          return (nParameters > value);
        }));
        
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, 
        "DeviationLower", m_deviationLower));
      
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, 
        "DeviationUpper", m_deviationUpper, [this](double value){
          return (this->m_deviationLower < value);
        }));

      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameters(xmlHandle, 
        "FunctionRange", m_functionRange, [](const FloatVector &values){
          return (values.size() <= 6);
        }));
        
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, 
        "UseLoglikelihood", m_useLoglikelihood));
          
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, 
        "UsePearsonChi2", m_usePearsonChi2));
            
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, 
        "ImproveFitResult", m_improveFitResult));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void FitParamInRangeTest::userRun(MonitorElement* monitorElement, QualityTestReport &report) {
      report.m_extraInfos["formula"] = m_fitFormula;
      // check monitor element first
      if(not checkElement(monitorElement)) {
        report.m_message = "Object attached to monitor element is not a TH1, TGraph or TGraph2D !";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }
      // create the function
      TF1 *fitFunction = createFunction();
      std::unique_ptr<TF1> up(fitFunction);
      // check monitor element <-> function matching
      if(not checkElement(monitorElement, fitFunction, report)) {
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      // perform fit
      TFitResultPtr fitResult = performFit(monitorElement, fitFunction, report);
      up.release();
      // perform range check
      performRangeTest(fitResult, report);
    }

    //-------------------------------------------------------------------------------------------------
    
    TF1* FitParamInRangeTest::createFunction() const {
      // Create our function
      std::stringstream formulaName;
      formulaName << "FitFormula_" << (void*)this;
      TFormula formula(formulaName.str().c_str(), m_fitFormula.c_str(), true);
      TF1 *fitFunction = nullptr;
      if(formula.GetNdim() == 1) {
        dqm_debug( "Created function 1D" );
        fitFunction = new TF1(formulaName.str().c_str(), m_fitFormula.c_str(), 0, 1);
      }
      else if(formula.GetNdim() == 2) {
        dqm_debug( "Created function 2D" );
        fitFunction = new TF2(formulaName.str().c_str(), m_fitFormula.c_str(), 0, 1, 0, 1);
      }
      else if(formula.GetNdim() == 3) {
        dqm_debug( "Created function 3D" );
        fitFunction = new TF3(formulaName.str().c_str(), m_fitFormula.c_str(), 0, 1, 0, 1, 0, 1);
      }
      else {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
      // set guess parameters 
      dqm_debug( "Settings guess parameters ..." );
      for(unsigned int i=0 ; i<m_guessParameters.size() ; ++i) {
        fitFunction->SetParameter(i, m_guessParameters.at(i));
      }
      dqm_debug( "Settings guess parameters ... OK" );
      // set function range
      dqm_debug( "Settings function range ..." );
      Double_t functionRange[6] = {0};
      for(unsigned int i=0 ; i<6 ; ++i) {
        if(i < m_functionRange.size()) {
          functionRange[i] = m_functionRange.at(i);
        }
        else {
          functionRange[i] = 0.;
        }
      }
      if(not m_functionRange.empty()) {
        dqm_debug( "Calling SetRange(...)" );
        fitFunction->SetRange(
          functionRange[0], functionRange[2], functionRange[4], 
          functionRange[1], functionRange[3], functionRange[5]
        );        
      }
      dqm_debug( "Settings function range ... OK" );
      return fitFunction;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool FitParamInRangeTest::checkElement(MonitorElement* monitorElement) const {
      const bool isHistogram = (nullptr == monitorElement->objectTo<TH1>());
      const bool isGraph = (nullptr == monitorElement->objectTo<TGraph>());
      const bool isGraph2D = (nullptr == monitorElement->objectTo<TGraph2D>());
      return (isHistogram or isGraph or isGraph2D);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool FitParamInRangeTest::checkElement(MonitorElement* monitorElement, TF1 *fitFunction, QReport &report) const {
      TH1 *histogram = monitorElement->objectTo<TH1>();
      TGraph *graph = monitorElement->objectTo<TGraph>();
      TGraph2D *graph2D = monitorElement->objectTo<TGraph2D>();
      // Check formula dimensions
      if(nullptr != histogram and fitFunction->GetNdim() != histogram->GetDimension()) {
        report.m_message = "TH1 attached to monitor element and fit formula have different dimensions !";
        report.m_quality = 0.f;
        return false;
      }
      if(nullptr != graph and fitFunction->GetNdim() != 1) {
        report.m_message = "TGraph attached to monitor element and fit formula have different dimensions !";
        report.m_quality = 0.f;
        return false;
      }
      if(nullptr != graph2D and fitFunction->GetNdim() != 2) {
        report.m_message = "TGraph2D attached to monitor element and fit formula have different dimensions !";
        report.m_quality = 0.f;
        return false;
      }
      return true;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string FitParamInRangeTest::getFitOptions(bool isHistogram, bool withSumW2) const {
      // setup fit option
      std::string fitOptions = "QN0+S";      
      if(m_usePearsonChi2 and isHistogram) {
        fitOptions += "P";
      }
      if(m_useLoglikelihood and isHistogram) {
        if(withSumW2) {
          fitOptions += "W";
        }
        fitOptions += "L";
      }
      if(m_improveFitResult) {
        fitOptions += "M";
      }
      if(not m_functionRange.empty()) {
        fitOptions += "R";
      }
      return fitOptions;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    TFitResultPtr FitParamInRangeTest::performFit(MonitorElement* monitorElement, TF1 *fitFunction, QReport &report) const {
      TH1 *histogram = monitorElement->objectTo<TH1>();
      TGraph *graph = monitorElement->objectTo<TGraph>();
      TGraph2D *graph2D = monitorElement->objectTo<TGraph2D>();
      // get the fit options
      bool isHistogram = (nullptr != monitorElement->objectTo<TH1>());
      bool withSumW2 = false;
      if(isHistogram)
        withSumW2 = (0 != monitorElement->objectTo<TH1>()->GetSumw2N());
      std::string fitOptions = getFitOptions(isHistogram, withSumW2);
      report.m_extraInfos["fit.opt"] = fitOptions;
      // perform fit
      TFitResultPtr fitResult;
      if(nullptr != histogram) {
        dqm_debug( "Fitting histogram" );
        fitResult = histogram->Fit(fitFunction, fitOptions.c_str(), "");
      }
      else if(nullptr != graph) {
        dqm_debug( "Fitting graph" );
        fitResult = graph->Fit(fitFunction, fitOptions.c_str(), "");
      }
      else if(nullptr != graph2D) {
        dqm_debug( "Fitting graph2D" );
        TF2 *fitFunction2D = (TF2*)fitFunction;
        fitResult = graph2D->Fit(fitFunction2D, fitOptions.c_str(), "");
      }
      dqm_debug( "Fitting done ..." );
      if(not fitResult->IsValid() or fitResult->IsEmpty()) {
        report.m_message = "Fit operation returned error !";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      return fitResult;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void FitParamInRangeTest::performRangeTest(TFitResultPtr fitResult, QReport &report) const {
      const double testParameter = fitResult->GetParams()[m_testParameter];
      const double testParameterError = fitResult->GetErrors()[m_testParameter];
      const double testParameterMinus = testParameter - testParameterError;
      const double testParameterPlus = testParameter + testParameterError;
      
      if(withinRange(testParameter, m_deviationLower, m_deviationUpper)) {
        report.m_message = "Fit parameter within range";
        report.m_quality = 1.f; // TODO scale me correctly !
      }
      else if(testParameterPlus > m_deviationLower or testParameterMinus < m_deviationUpper) {
        report.m_message = "Fit parameter not within range, but by using the fit parameter error, the value is within the expected range";
        report.m_quality = (this->errorLimit() + this->warningLimit())/2.f; // TODO scale me correctly !
      }
      else {
        report.m_message = "Fit parameter not within range !";
        report.m_quality = this->errorLimit() / 2.f; // TODO scale me correctly !
      }
      report.m_extraInfos["fit.val"] = testParameter;
      report.m_extraInfos["fit.err"] = testParameterError;
      report.m_extraInfos["fit.prob"] = fitResult->Prob();
      report.m_extraInfos["fit.status"] = fitResult->Status();
      report.m_extraInfos["fit.ncalls"] = fitResult->NCalls();
      report.m_extraInfos["fit.mintype"] = fitResult->MinimizerType();
      report.m_extraInfos["fit.minfcn"] = fitResult->MinFcnValue();
      report.m_extraInfos["fit.edm"] = fitResult->Edm();
      report.m_extraInfos["fit.chi2"] = fitResult->Chi2();
    }
    
    //-------------------------------------------------------------------------------------------------

    DQM_PLUGIN_DECL(FitParamInRangeTestFactory, "FitParamInRangeTest");
  }
}
