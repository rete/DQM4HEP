//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : A.Pingault, R.Ete
//====================================================================

#ifndef DQM4HEP_INTERNAL_H
#define DQM4HEP_INTERNAL_H

// -- std headers
#include <assert.h>
#include <chrono>
#include <ctime>
#include <exception>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <queue>
#include <ratio>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <sys/stat.h>
#include <sys/utsname.h>
#include <thread>
#include <unistd.h>

// -- apple headers for stdint.h
#ifdef __APPLE__
#include <_types.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <_types/_uint8_t.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/_pthread/_pthread_types.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int64_t.h>
#else
#include <bits/pthreadtypes.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#endif

// -- dqm4hep headers
#include <dqm4hep/DQM4hepConfig.h>

//-------------------------------------------------------------------------------------------------

#if defined(__GNUC__) && defined(BACKTRACE)
#include <cstdlib>
#include <execinfo.h>
#endif

// limits definitions platform independent
#ifdef _MSC_VER
#include <float.h>
#define IS_NAN _isnan
#define IS_INF !_finite
#else
#define IS_NAN std::isnan
#define IS_INF std::isinf
#endif

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#define DEPRECATED(func) func
#endif

// Useful macro to silent any exception
#define DQM4HEP_NO_EXCEPTION( Code ) try { Code } catch(...) {}

namespace dqm4hep {

  namespace core {

    class Event;
    class EventStreamerPlugin;
    class MonitorElement;
    class QualityTest;
    class QualityTestReport;
    class Path;
    class Plugin;
    template <typename T>
    class Directory;
    template <typename T>
    class Storage;
    
    /**
     *  @brief  type traits class helper
     */
    template <typename T>
    class type {
    public:
      // stl types
      typedef std::vector<T> vector;
      typedef std::list<T> list;
      typedef std::set<T> set;
      typedef std::deque<T> deque;
      typedef std::unordered_set<T> uset;
      // pointer types
      typedef T* raw_ptr;
      typedef std::shared_ptr<T> ptr;
      typedef std::unique_ptr<T> uptr;
      typedef std::weak_ptr<T> wptr;
      // stl + pointers
      typedef std::vector<ptr> ptr_vector;
      typedef std::vector<uptr> uptr_vector;
      typedef std::vector<wptr> wptr_vector;
      typedef std::set<ptr> ptr_set;
      typedef std::set<uptr> uptr_set;
      typedef std::set<wptr> wptr_set;
      typedef std::deque<ptr> ptr_deque;
      typedef std::deque<uptr> uptr_deque;
      typedef std::deque<wptr> wptr_deque;
      typedef std::unordered_set<ptr> ptr_uset;
      typedef std::unordered_set<uptr> uptr_uset;
      typedef std::unordered_set<wptr> wptr_uset;
      // miscellanous
      typedef std::map<std::string, T> str_map;
      typedef std::map<std::string, ptr> str_ptr_map;
      typedef std::map<std::string, ptr_vector> str_ptr_vec_map;
      typedef std::unordered_map<std::string, T> str_umap;
      typedef std::unordered_map<std::string, ptr> str_ptr_umap;
      typedef std::unordered_map<std::string, ptr_vector> str_ptr_vec_umap;
    };
    
    
    /**
     *  @brief  time helper class
     */
    class time {
    public:
      // typedefs
      typedef std::chrono::system_clock clock;
      typedef clock::time_point point;
      typedef clock::duration duration;
      typedef clock::period period;

      /**
       *  @brief  Returns the current time
       */
      static inline point now() {
        return clock::now();
      }
      
      /**
       *  @brief  Sleep for a given duration
       * 
       *  @param  d the duration to sleep for 
       */
      static inline void sleep(const duration &d) {
        std::this_thread::sleep_for(d);
      }
      
      /**
       *  @brief  Sleep for n microseconds
       * 
       *  @param  usec the number of micro seconds to sleep for
       */
      static inline void usleep(unsigned int usec) {
        std::this_thread::sleep_for(std::chrono::microseconds(usec));
      }
      
      /**
       *  @brief  Sleep for n milliseconds
       * 
       *  @param  usec the number of milliseconds to sleep for
       */
      static inline void msleep(unsigned int msec) {
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
      }
      
      /**
       *  @brief  Sleep for n nanoseconds
       * 
       *  @param  usec the number of nanoseconds to sleep for
       */
      static inline void nsleep(unsigned int nsec) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(nsec));
      }
      
      /**
       *  @brief  Convert time_t to time point
       *  
       *  @param  t the time_t to convert
       */
      static inline point asPoint(std::time_t t) {
        return clock::from_time_t(t);
      }
      
      /**
       *  @brief  Convert the time point to time_t
       * 
       *  @param  p the time point to convert
       */
      static inline std::time_t asTime(const point &p) {
        return clock::to_time_t(p);
      }

      /**
       *  @brief  Convert the integer to time point
       * 
       *  @param  integer the integer to convert
       */
      static inline point asPoint(int32_t integer) {
        return asPoint(static_cast<std::time_t>(integer));
      }
      
      /**
       *  @brief  Convert the time point to integer
       * 
       *  @param  p the time point to convert
       */
      static inline int32_t asInt(const point &p) {
        return static_cast<int32_t>(asTime(p));
      }
      
      /**
       *  @brief  Convert a time pointto string. Format is HH:MM:SS
       * 
       *  @param  p the time point to convert
       */
      static inline std::string asString(const point &p) {
        auto timeT = asTime(p);
        std::tm timeTm;
        localtime_r(&timeT, &timeTm);
        std::stringstream ss;
        ss << timeTm.tm_hour << ":" << timeTm.tm_min << ":" << timeTm.tm_sec;
        return ss.str();
      }
    };

    // event
    typedef type<Event>::ptr EventPtr;
    typedef type<Event>::ptr_deque EventQueue;
    typedef type<EventStreamerPlugin>::ptr EventStreamerPluginPtr;

    // plugin
    typedef type<Plugin>::ptr PluginPtr;

    // basic typedefs
    typedef std::vector<int> IntVector;
    typedef std::vector<unsigned int> UIntVector;
    typedef std::vector<float> FloatVector;
    typedef std::vector<double> DoubleVector;
    typedef std::vector<std::string> StringVector;
    typedef std::set<std::string> StringSet;
    typedef std::map<std::string, std::string> ParameterMap;
    typedef std::pair<std::string, std::string> StringPair;
    typedef std::map<std::string, std::string> StringMap;

    // dqm4hep typedefs
    typedef type<MonitorElement>::ptr MonitorElementPtr;
    typedef type<MonitorElement>::ptr_vector MonitorElementList;
    typedef type<MonitorElement>::str_ptr_map MonitorElementMap;
    typedef type<Directory<MonitorElement>>::ptr MonitorElementDir;
    typedef QualityTest QTest;
    typedef QualityTestReport QReport;
    typedef type<QTest>::ptr QTestPtr;
    typedef type<QReport>::str_map QReportMap;
    typedef std::map<StringPair, QReportMap> QReportContainer;
    typedef type<QTest>::str_ptr_map QTestMap;
    typedef type<Storage<MonitorElement>>::ptr MeStoragePtr;

    //-------------------------------------------------------------------------------------------------

    /** Convert std::string to a type
     */
    template <class T>
    inline bool stringToType(const std::string &s, T &t) {
      std::istringstream iss(s);
      return !(iss >> t).fail();
    }

    //-------------------------------------------------------------------------------------------------

    /** Convert type to std::string
    */
    template <class T>
    inline std::string typeToString(const T &t) {
      std::ostringstream oss;

      if ((oss << t).fail())
        throw;

      return oss.str();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <>
    inline bool stringToType(const std::string &s, std::string &t) {
      t = s;
      return true;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline std::string typeToString(const std::string &t) {
      return t;
    }

    //-------------------------------------------------------------------------------------------------

    /** Screen splash of DQM4HEP
     */
    inline void screenSplash() {
      std::cout << "#######################################################" << std::endl;
      std::cout << "#" << std::endl;
      std::cout << "#                    DQM4HEP" << std::endl;
      std::cout << "#   (Data Quality Monitoring For High Energy Physics)" << std::endl;
      std::cout << "#" << std::endl;
      std::cout << "#      Version    : " << DQM4hep_VERSION_STR << std::endl;
      std::cout << "#      Started at : " << time::asString(time::now()) << std::endl;
      std::cout << "#      Author     : R. Ete, A. Pingault" << std::endl;
      std::cout << "#      Mail       : <dqm4hep@desy.de> " << std::endl;
      std::cout << "#" << std::endl;
      std::cout << "#######################################################" << std::endl;
    }

    //-------------------------------------------------------------------------------------------------

    /** Tokenize string with delimiter.
     */
    template <typename T>
    inline void tokenize(const std::string &inputString, std::vector<T> &tokens, const std::string &delimiter = " ") {
      std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
      std::string::size_type pos = inputString.find_first_of(delimiter, lastPos);

      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        T value;
        if(not stringToType(inputString.substr(lastPos, pos - lastPos), value)) {
          return;
        }
        tokens.push_back(value);
        lastPos = inputString.find_first_not_of(delimiter, pos);
        pos = inputString.find_first_of(delimiter, lastPos);
      }
    }
    
    //-------------------------------------------------------------------------------------------------

    /** Tokenize string with delimiter.
     */
    template <>
    inline void tokenize(const std::string &inputString, StringVector &tokens, const std::string &delimiter) {
      std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
      std::string::size_type pos = inputString.find_first_of(delimiter, lastPos);

      while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
        tokens.push_back(inputString.substr(lastPos, pos - lastPos));
        lastPos = inputString.find_first_not_of(delimiter, pos);
        pos = inputString.find_first_of(delimiter, lastPos);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  Whether the test string matches the wildcard expression 
     *  
     *  @param  testString the string to test
     *  @param  wildcardString the string containing wildcards
     */
    bool wildcardMatch(const std::string &testString, const std::string &wildcardString, bool caseSensitive = true);
    
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  Replaces all occurences in the spcified string
     *  
     *  @param  subject the string on which to perform replace
     *  @param  search the portion of the input string to search
     *  @param  replace the portion of string to replace with
     */
    std::string &replaceAll(std::string &subject, const std::string& search, const std::string& replace);

    //-------------------------------------------------------------------------------------------------

    /** Get the special character list that are to be avoided in some strings
     */
    inline StringVector getSpecialCharacterList() {
      StringVector specialCharacterList;

      specialCharacterList.push_back("|");
      specialCharacterList.push_back("&");
      specialCharacterList.push_back(";");
      specialCharacterList.push_back("<");
      specialCharacterList.push_back(">");
      specialCharacterList.push_back("(");
      specialCharacterList.push_back(")");
      specialCharacterList.push_back("$");
      specialCharacterList.push_back("\\");
      specialCharacterList.push_back("\'");
      specialCharacterList.push_back("\"");
      specialCharacterList.push_back("\t");
      specialCharacterList.push_back("\n");
      specialCharacterList.push_back("*");
      specialCharacterList.push_back("?");
      specialCharacterList.push_back(",");
      specialCharacterList.push_back("[");
      specialCharacterList.push_back("]");
      specialCharacterList.push_back("#");
      specialCharacterList.push_back("~");
      specialCharacterList.push_back("=");
      specialCharacterList.push_back("%");

      return specialCharacterList;
    }

    //-------------------------------------------------------------------------------------------------

    /** Whether the string contains a special character
     */
    inline bool containsSpecialCharacters(const std::string &str) {
      StringVector specialCharacterList = dqm4hep::core::getSpecialCharacterList();

      for (auto iter = specialCharacterList.begin(), endIter = specialCharacterList.end(); endIter != iter; ++iter) {
        if (str.find(*iter) != std::string::npos)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    /** Fill host info (uname result + host name). See Key for available keys
     */
    inline void fillHostInfo(StringMap &info) {
      // uname
      struct utsname unameStruct;
      uname(&unameStruct);

      // host name
      char host[256];
      gethostname(host, 256);

      info["system"] = unameStruct.sysname;
      info["node"] = unameStruct.nodename;
      info["release"] = unameStruct.release;
      info["version"] = unameStruct.version;
      info["machine"] = unameStruct.machine;
      info["host"] = host;
    }

    //-------------------------------------------------------------------------------------------------

    /** Specialization for booleans
     */
    template <>
    inline bool stringToType(const std::string &s, bool &t) {
      if (s == "on" || s == "1" || s == "true")
        t = true;
      else if (s == "off" || s == "0" || s == "false")
        t = false;
      else
        return false;

      return true;
    }
    
    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get the current process id
     */
    inline int pid() {
#if _WIN32
      return GetCurrentProcessId();
#else
      return ::getpid();
#endif
    }
    
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  CpuStats struct
     *          Store system wide cpu usage in percentage
     */
    struct CpuStats {
      float load1m = {0.};  //!< cpu load average over 1 m
      float load5m = {0.};  //!< cpu load average over 5 m
      float load15m = {0.}; //!< cpu load average over 15 m
      float user = {0.};    //!< cpu user load in \b percentage
      float sys = {0.};     //!< cpu sys load in \b percentage
      float tot = {0.};     //!< cpu user+sys load in \b percentage
      float idle = {0.};    //!< cpu idle in \b percentage
    };

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get some memory stats
     *
     *  @param  stats the CpuStats object to receive
     *  @param  sampleTime time between two sampling to compute load, in \b seconds 
     */
    void cpuStats(CpuStats &stats, int sampleTime = 1 /*s*/);
    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  MemoryStats struct
     *          Store system wide memory usage
     *          All units in \b Mb
     */
    struct MemoryStats {
      int32_t vmTot = {0};   //!< total virtual RAM (physical RAM + swap) in \b MB
      int32_t vmUsed = {0};  //!< used virtual RAM (physical RAM + swap) in \b MB
      int32_t vmFree = {0};  //!< free virtual RAM (physical RAM + swap) in \b MB
      int32_t rssTot = {0};  //!< total physical RAM in \b MB
      int32_t rssUsed = {0}; //!< used  physical RAM in \b MB
    };

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get some memory stats
     *
     *  @param  stats the MemoryStats object to receive
     */
    void memStats(MemoryStats &stats);

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  ProcessStats struct
     *          Store cpu (time/percentage) and memory usage from current process  
     *          All memory units in \b kb !
     */
    struct ProcessStats {
      float cpuTimeUser = {0.}; //!< user time used by this process in \b seconds
      float cpuTimeSys = {0.};  //!< system time used by this process in \b seconds
      float cpuTimeTot = {0.};  //!< total time used by this process in \b seconds
      float cpuUser = {0.};     //!< cpu user load used by this process in \b percentage
      float cpuSys = {0.};      //!< cpu sys load used by this process in \b percentage
      float cpuTot = {0.};      //!< total (sys+user) cpu load used by this process in \b percentage
      long vm = {0L};           //!< virtual memory used by this process in \b KB
      long rss = {0L};          //!< resident memory used by this process in \b KB
      timeval lastPollTime {0, 0};  //!< last time process stats were polled
    };

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get stats from current process
     *
     *  @param  stats the ProcessStats object to receive
     */
    void procStats(ProcessStats &stats);
    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  INetworkStats struct
     *          Store system wide network stats
     *          Units in \b packets or \b kb 
     */
    struct INetworkStats {
      uint64_t tot_rcv_kbytes = {0L};   //!< total data received in \b kb
      uint64_t tot_rcv_packets = {0L};  //!< total packets received
      uint64_t tot_rcv_errs = {0L};     //!< total errors received
      uint64_t tot_snd_kbytes = {0L};   //!< total data sent in \b kb
      uint64_t tot_snd_packets = {0L};  //!< total packets sent
      uint64_t tot_snd_errs = {0L};     //!< total errors sent
      uint64_t rcv_rate_kbytes = {0L};  //!< rate of data received in \b kb
      uint64_t rcv_rate_packets = {0L}; //!< rate of packets received
      uint64_t rcv_rate_errs = {0L};    //!< rate of errors received
      uint64_t snd_rate_kbytes = {0L};  //!< rate of data sent in \b kb
      uint64_t snd_rate_packets = {0L}; //!< rate of packets sent
      uint64_t snd_rate_errs = {0L};    //!< rate of errors sent
    };

    typedef std::map<std::string, INetworkStats> NetworkStats;

    //-------------------------------------------------------------------------------------------------

    /**
     *  @brief  Get some network stats
     *
     *  @param  stats the NetworkStats object to receive
     *  @param  sampleTime time between two sampling to compute rates, in \b seconds 
     */
    void netStats(NetworkStats &stats, int sampleTime = 1 /*s*/);
    
    /**
     *  @brief  Os class
     */
    class Os {
    public:
      /**
       *  @brief  Get a environment variable. If it is not found, 
       *  then use the fallback value specified as second argument.
       *  The variable is converted to the specified type
       *
       *  @param  var the environment variable name
       *  @param  fallback the user fallback value if not found
       */
      template <typename T>
      static T getenv(const std::string &var, const T &fallback);
      
      /**
       *  @brief  Get a environment variable.
       *  The variable is converted to the specified type
       *
       *  @param  var the environment variable name
       */
      template <typename T>
      static T getenv(const std::string &var);
    };
    
    template <typename T>
    inline T Os::getenv(const std::string &var, const T &fallback) {
      const char *env = ::getenv(var.c_str());
      if(nullptr == env) {
        return fallback;
      }
      std::string envStr = env;
      T varType;
      if(not core::stringToType(envStr, varType)) {
        return fallback;
      }
      return varType;
    }
    

    template <typename T>
    inline T Os::getenv(const std::string &var) {
      const char *env = ::getenv(var.c_str());
      std::string envStr = (env != nullptr) ? env : "";
      T varType;
      core::stringToType(envStr, varType);
      return varType;
    }
    
    template <>
    inline std::string Os::getenv(const std::string &var, const std::string &fallback) {
      const char *env = ::getenv(var.c_str());
      if(nullptr == env) {
        return fallback;
      }
      return std::string(env);
    }
    
    template <>
    inline std::string Os::getenv(const std::string &var) {
      const char *env = ::getenv(var.c_str());
      return (env != nullptr) ? env : "";
    }
    
  } // namespace core
  
} // namespace dqm4hep

#endif //  DQM4HEP_INTERNAL_H
