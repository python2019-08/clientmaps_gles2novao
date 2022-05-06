  
#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include <map> 

#ifdef OsID_WINDOWS  
#include <windows.h>
#endif
#include <md/util/chrono.hpp>
#include <md/util/logtool.hpp>


#define VA2MSG \
    va_list args; \
    va_start(args, format); \
    char msg[4096]; \
    vsnprintf(msg, sizeof(msg), format, args); \
    va_end(args);  

namespace clientmap {
 
namespace {
 
std::atomic<bool> s_useThread(true);
std::mutex s_mutex;

} // namespace
 
std::atomic<int>   Log::m_eventLevel = (int)EventSeverity::Debug;
std::atomic<bool>  Log::m_eventLevelAsMin = true;

Log::Log()  
{

}

Log::~Log() = default;

Log* Log::get() noexcept 
{
    static Log instance;
    return &instance;
}

void Log::setEventLevel(int aLevel)
{
    m_eventLevel = aLevel;
}

void Log::setEventLevelAsMin(bool aLevelAsMin)
{

}

void Log::Debug(const char* format , ...)
{
    VA2MSG;
    _record(EventSeverity::Debug, std::string{ msg });
}
  

void Log::Info(const char* format, ...)
{
    VA2MSG;
    _recordInfo(EventSeverity::Info, std::string{ msg });
}

void Log::Warning(const char* format  , ...)
{
    VA2MSG;
    _record(EventSeverity::Warning, std::string{ msg });
}

void Log::Error(const char* format , ...)
{
    VA2MSG;
    _record(EventSeverity::Error, std::string{ msg });
}


void Log::Record(EventSeverity severity, const char* format, ... ) 
{
    va_list args;
    va_start(args, format);
    char msg[4096];
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    _record(severity,std::string{msg});
}
 

void Log::_record(EventSeverity aES, const std::string& msg ) 
{
    std::lock_guard<std::mutex> lock(s_mutex);
 
    int evtLevel = m_eventLevel;
    bool evtLevelAsMin = m_eventLevelAsMin;
    if ( (!evtLevelAsMin && (int)aES != evtLevel)||
        (evtLevelAsMin && (int)aES < evtLevel)
        )
    {
        return;
    }

    std::stringstream logStream;  
    logStream << currentMillisec(); // now
    if (!msg.empty()) {
        logStream << ": " << msg;
    }

    platformRecord(aES, logStream.str());
}


void Log::_recordInfo(EventSeverity aES, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    int evtLevel = m_eventLevel;
    bool evtLevelAsMin = m_eventLevelAsMin;
    if ((!evtLevelAsMin && (int)aES != evtLevel) ||
        (evtLevelAsMin && (int)aES < evtLevel)
        )
    {
        return;
    }
     

    platformRecord(aES, msg);
}


void Log::platformRecord(EventSeverity aSeverity,
    const std::string &msg)
{
    //EventSeverity_strArr[] = {
    //{ EventSeverity::Debug, "DEBUG" },
    //{ EventSeverity::Info, "INFO" },
    //{ EventSeverity::Warning, "WARNING" },
    //{ EventSeverity::Error, "ERROR" },
    //{ EventSeverity(-1), "UNKNOWN" },
    //};
    static  std::map<EventSeverity,std::string>   eventSeverity2str= 
        {
        { EventSeverity::Debug, "DEBUG" },
        { EventSeverity::Info, "INFO" },
        { EventSeverity::Warning, "WARNING" },
        { EventSeverity::Error, "ERROR" },
        { EventSeverity(-1), "UNKNOWN" },
        } ;
#ifdef OsID_WINDOWS  
    if (aSeverity != EventSeverity::Info)
    {
        OutputDebugStringA("[");
        OutputDebugStringA(eventSeverity2str[aSeverity].c_str());
        OutputDebugStringA("] ");
    }
    OutputDebugStringA( msg.c_str() );
    //OutputDebugStringA("\n");
#else
    if (aSeverity != EventSeverity::Info)
    {
        std::cerr << "[" << eventSeverity2str[aSeverity] << "] "
    }
    std::cerr << msg;// << std::endl;
#endif
}

} // MdU//namespace mbgl
