/*
 * logger.h
 *
 *  Created on: 4 cze 2022
 *      Author: marci
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_


#include <iostream>
#include <sstream>


enum struct LogLevel : uint8_t
{
    None,
    Always,
    Error,
    Warning,
    Info,
    Debug
};

//////// set current log level /////////
constexpr LogLevel currentLevel = LogLevel::Info;

void logMessage(LogLevel level, std::ostream& ostr, bool once = false);
uint16_t getChecksum(std::string str);

#define LOG_ALWAYS(message) logMessage(LogLevel::Always, std::ostringstream().flush() << message)
#define LOG_ERROR(message) logMessage(LogLevel::Error, std::ostringstream().flush() << message)
#define LOG_WARNING(message) logMessage(LogLevel::Warning, std::ostringstream().flush() << message)
#define LOG_INFO(message) logMessage(LogLevel::Info, std::ostringstream().flush() << message)
#define LOG_DEBUG(message) logMessage(LogLevel::Debug, std::ostringstream().flush() << message)

#define LOG_ERROR_ONCE(message) logMessage(LogLevel::Error, std::ostringstream().flush() << message, true)
#define LOG_WARNING_ONCE(message) logMessage(LogLevel::Warning, std::ostringstream().flush() << message, true)
#define LOG_INFO_ONCE(message) logMessage(LogLevel::Info, std::ostringstream().flush() << message, true)
#define LOG_DEBUG_ONCE(message) logMessage(LogLevel::Debug, std::ostringstream().flush() << message, true)

#endif /* INC_LOGGER_H_ */
