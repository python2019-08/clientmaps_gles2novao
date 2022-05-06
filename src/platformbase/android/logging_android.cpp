//from <test_runner_common.cpp>
//#include "test_runner_common.hpp"
#include <android/log.h>

namespace mbgl {
namespace android {
int severityToPriority(EventSeverity severity) 
{
    switch (severity) 
	{
        case EventSeverity::Debug:
            return ANDROID_LOG_DEBUG;
        case EventSeverity::Info:
            return ANDROID_LOG_INFO;
        case EventSeverity::Warning:
            return ANDROID_LOG_WARN;
        case EventSeverity::Error:
            return ANDROID_LOG_ERROR;
        default:
            return ANDROID_LOG_VERBOSE;
    }
} 
} // namespace android

void Log::platformRecord(EventSeverity severity, 
						const std::string& msg) 
{
    __android_log_print(android::severityToPriority(severity), 
		"mbgl", "%s", msg.c_str());
}


} // namespace mbgl
