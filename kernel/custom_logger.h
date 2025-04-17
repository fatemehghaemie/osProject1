#ifndef CUSTOM_LOGGER_H
#define CUSTOM_LOGGER_H

// log defenition
#define LOG_INFO  1
#define LOG_WARN  2
#define LOG_ERROR 3

// log function
void log_message(int level, const char *message);

#endif 
