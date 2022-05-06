#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <algorithm>
#include <cassert>
#include <string>
#include <atomic>
 
#include <md/util/cpp_misc.hpp>
  
namespace clientmap {
//----------------------------------------------------
    enum class EventSeverity : int8_t {
        Unknown = -1,
        Debug,
        Info,
        Warning,
        Error,
    };

    
//----------------------------------------------------
class Log {
public:
   
private:
    template <typename T, size_t N>
    constexpr static bool includes(const T e, 
                                const T (&l)[N], 
                                const size_t i = 0
    ) 
    {
        return i < N && (l[i] == e || includes(e, l, i + 1));
    }

public:
    Log();
    ~Log();

    static void setEventLevel(int aLevel);
    static void setEventLevelAsMin(bool aLevelAsMin);
      
    static void Debug(const char* format = "", ...); 
    static void Info(const char* format = "", ...);
    static void Warning(const char* format = "", ...);     
    static void Error(const char* format = "", ...);

    static void Record(EventSeverity severity, const char* format = "", ...);

private:
    static Log* get() noexcept;    
    static void _record(EventSeverity aES,const std::string &msg); 
    static void _recordInfo(EventSeverity aES, const std::string& msg);
    // This method is the data sink that must be implemented by each platform we
    // support. It should ideally output the error message in a human readable
    // format to the developer.
    static void platformRecord(EventSeverity aES, const std::string &msg); 

    static std::atomic<int>   m_eventLevel  ;
    static std::atomic<bool>  m_eventLevelAsMin ;
  
};

} // MdU//namespace md
