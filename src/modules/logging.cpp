#include "logging.h"

#ifdef USE_LOGGING
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
#endif

void Logger::logMessage(Logger::custom_severity_level severity, const char* msg) {
    #ifdef USE_LOGGING
    logging::trivial::severity_level boostSeverity;
    
    switch (severity) {
        case info:
            boostSeverity = logging::trivial::info;
            break;
        case warning:
            boostSeverity = logging::trivial::warning;
            break;
        case error:
            boostSeverity = logging::trivial::error;
            break;
        default:
            boostSeverity = logging::trivial::info;
    }

    BOOST_LOG_TRIVIAL(info) << msg;
    #else
    std::cout << msg << std::endl;
    #endif
}
