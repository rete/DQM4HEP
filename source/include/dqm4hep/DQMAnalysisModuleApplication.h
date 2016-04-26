  /// \file DQMAnalysisModuleApplication.h
/*
 *
 * DQMAnalysisModuleApplication.h header template automatically generated by a class generator
 * Creation date : dim. nov. 9 2014
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


#ifndef DQMANALYSISMODULEAPPLICATION_H
#define DQMANALYSISMODULEAPPLICATION_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMModuleApplication.h"
#include "dqm4hep/DQMCycle.h"

namespace dqm4hep
{

class DQMEventClient;
class DQMRunControlClient;
class DQMCycle;
class DQMArchiver;
class DQMEvent;
class DQMRun;
class TiXmlHandle;

/** DQMAnalysisModuleApplication class
 */ 
class DQMAnalysisModuleApplication : public DQMModuleApplication, public DQMCycleListener
{
public:
	/** Settings class
 	 */
	class Settings
	{
	public:
		/** Constructor
		 */
		Settings();

		/** Print the parameters
		 */
		void print(const std::string &moduleLogStr);

	public:
		std::string         m_settingsFileName;
		std::string         m_archiveDirectory;
		bool                m_shouldOpenArchive;
		std::string         m_runControlType;
		std::string         m_runControlName;
		std::string         m_eventClientType;
		std::string         m_monitorElementCollector;
		std::string         m_cycleType;
		float               m_cycleValue;
		unsigned int        m_cycleTimeout;
	};

public:
	/** Constructor
	 */
	DQMAnalysisModuleApplication();

	/** Destructor
	 */
	~DQMAnalysisModuleApplication();

	/** Get the application type ("AnalysisModule")
	 */
	const std::string &getType() const;

	/** Get the application name ("ModuleType/ModuleName")
	 */
	const std::string &getName() const;

	/** Read settings from a xml file.
	 *  Initialize the application. Declare all the services,
	 *  load the shared libraries, configure the active module
	 */
	StatusCode readSettings( const std::string &settingsFile );

	/** Set the parameters to replace while reading settings
	 */
	void setReplacementParameters(const DQMParameters &parametersMap);

	/** Run the application.
	 */
	StatusCode run();

	/** Get the current run if any
	 */
	DQMRun *getCurrentRun() const;

	/** Get the cycle if any
	 */
	DQMCycle *getCycle() const;

private:
	/** Configure the module
	 */
	StatusCode configureModule(const TiXmlHandle xmlHandle);

	/** Configure network interface
	 */
	StatusCode configureNetwork(const TiXmlHandle xmlHandle);

	/** Configure the workflow cycle
	 */
	StatusCode configureCycle(const TiXmlHandle xmlHandle);

	/** Configure the archiver
	 */
	StatusCode configureArchiver(const TiXmlHandle xmlHandle);

	/** Start all services. Called just at beginning of run()
	 */
	StatusCode startServices();

	/** Stop all services. Called at the end of run() before exit
	 */
	StatusCode stopServices();

	/** Get the current run number that was started (not from run control)
	 */
	int getCurrentRunNumber() const;

	// from cycle listener
	void onEventProcessed(const DQMCycle *const pCycle, const DQMEvent *const pEvent) { /* nop */ }
	void onCycleStarted(const DQMCycle *const pCycle);
	void onCycleStopped(const DQMCycle *const pCycle);

private:
	// members
	Settings                       m_settings;
	DQMParameters                  m_replacementParameters;

	DQMEventClient                *m_pEventClient;
	DQMRunControlClient           *m_pRunControlClient;
	DQMCycle                      *m_pCycle;
	DQMArchiver                   *m_pArchiver;

	std::string                   m_applicationType;
	std::string                   m_applicationName;
	std::string                   m_moduleLogStr;

	int                           m_runNumber;
}; 

} 

#endif  //  DQMANALYSISMODULEAPPLICATION_H
