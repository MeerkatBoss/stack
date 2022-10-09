#ifndef CUSTOM_LOGGER_H
#define CUSTOM_LOGGER_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 
 * Maximum number of active loggers
 */
const size_t MAX_LOGGERS_COUNT = 16;

/**
 * @brief 
 * Log message importance
 */
enum message_level
{
    MSG_TRACE   = 0,
    MSG_INFO    = 1,
    MSG_WARNING = 2,
    MSG_ERROR   = 3,
    MSG_FATAL   = 4
};

/**
 * @brief 
 * Minimal importance of logged messages
 */
enum log_level
{
    LOG_ALL     = MSG_TRACE,
    LOG_INFO    = MSG_INFO,
    LOG_WARNING = MSG_WARNING,
    LOG_ERROR   = MSG_ERROR,
    LOG_FATAL   = MSG_FATAL,
    LOG_NONE    = 99999
};

/**
 * @brief 
 * Logger settings flags
 */
enum logger_settings
{
    LGS_USE_ESCAPE  = 0001, /*!< Use escape sequences for colors */
    LGS_USE_HTML    = 0002, /*!< Use HTML format for logs.
                                    This option is ignored if `LGS_USE_ESCAPE` is set */
    LGS_LOG_ALWAYS  = 0004, /*!< Ignore pausing logs*/
    LGS_KEEP_OPEN   = 0006  /*!< Do not close logger stream */
};

/**
 * @brief 
 * Contains info about logger
 */
struct logger
{
    const char*     name;
    FILE*           stream;
    enum log_level  logging_level;
    unsigned int    settings_mask;
};

/**
 * @brief 
 * Add new logger
 * @param[in] added Added logger instance 
 */
void add_logger(logger added);

/**
 * @brief 
 * Add dynamic logger
 * 
 * TODO:      v~~~~ added? not yet, it's just provided for addition (wording)
 * @param[in] added Added logger pointer.
 * Any modifications to logger will change its
 * behaviour.
 * 
 * TODO: wording, ownership is not assumed, it's borrowed.
 *       It's better to write that logging system "takes ownership".
 * @note Logging system does assumes ownership over
 * logger. Pointer will be freed after call to `log_stop`
 * 
 * @warning If `LGS_KEEP_OPEN` flag is not set, logging system will
 * attempt to close logger stream after call to `log_stop` even though it
 * does not own it.                                        ^~~~~~~~~~~~~
 * TODO:                                                   It has no way to know who owns it,
 *                                                         unnecessary comment (clarification about 
 *                                                         it closing stream is good though) 
 */
void add_custom_logger(logger* added);

/**
 * @brief 
 * Add logger with `logging_level` = `LOG_TRACE`
 * writing to file 'default.log'
 */
void add_default_file_logger(void); // TODO: It's not really "default" if you need to add it manually

/**
 * @brief 
 * Add logger with `logging_level` = `LOG_INFO`
 * writing to `stdout`
 */
void add_default_console_logger(void); // TODO: same thing

/**
 * @brief 
 * Write message to logs
 * @param[in] level Message importance
 * @param[in] format `printf` format string
 * @param[in] ... `printf` arguments
 */
void log_message(message_level level, const char* format, ...);

/**
 * @brief 
 * Temporarily suspend writing log messages to all loggers.
 * Loggers with `LGS_LOG_ALWAYS` ignore this call.
 * 
 * This operation is idempotent.
 */
void log_pause(void);

/**
 * @brief 
 * Undo the effects of `log_pause` call.
 * 
 * This operation is idempotent.
 */
void log_resume(void);

/**
 * @brief 
 * Stop writing log messages to all loggers and close
 * associated streams. Streams associated with logs with
 * `LGS_KEEP_OPEN` flag set will not be closed.
 */
void log_stop(void);

#ifndef NTRACE
/**
 * @brief 
 * Print verbose information about performing given action
 * 
 * @param[in] operation Traced action
 * @param[in] state_format `printf` format string
 * @param[in] ... `printf` arguments
 */
#define LOG_PRINT_TRACE(operation, state_format, ...) do                    \
{                                                                           \
    log_message(MSG_TRACE, "Performing operation %s in %s:%zu, file: %s",   \
        #operation, __PRETTY_FUNCTION__, __LINE__, __FILE__);               \
    if (state_format && *(const char*)(state_format))                       \
    {                                                                       \
        log_message(MSG_TRACE, "State before execution:");                  \
        log_message(MSG_TRACE, state_format, __VA_ARGS__);                  \
    }                                                                       \
    operation;                                                              \
    if (state_format && *(const char*)(state_format)) /* TODO: repetition */\
    {                                                                       \
        log_message(MSG_TRACE, "State after execution:");                   \
        log_message(MSG_TRACE, state_format, __VA_ARGS__);                  \
    }                                                                       \
} while (0)
#else
/**
 * @brief 
 * DISABLED // TODO: Who disabled and why. I know it's brief, but this is too short)
 * 
 * Print verbose information about performing given action
 * 
 * @param[in] operation Traced action
 * @param[in] state_format `printf` format string
 * @param[in] ... `printf` arguments
 */
#define LOG_PRINT_TRACE(operation, state_format, ...) do {operation;} while (0)
#endif

/**
 * @brief 
 * Assert condition is met. If not, print message to log and perform action
 * 
 * @param[in] level Message importance
 * @param[in] condition Condition to be tested against
 * @param[in] on_fail Action performed if `condition` is not met
 */
#define LOG_ASSERT(level, condition, on_fail) do                            \
{                                                                           \
    if (!(condition))                                                       \
    {                                                                       \
        log_message(level, "Condition %s not met in %s:%zu, file: %s",      \
            #condition, __PRETTY_FUNCTION__, __LINE__, __FILE__);           \
                       /* TODO: Extract this stuff in seprate macro */      \
        on_fail;                                                            \
    }                                                                       \
} while (0)

/**
 * @brief 
 * Trace action execution. If condition is not met after execution,
 * perform `on_fail` action
 * 
 * @param[in] operation Traced operation
 * @param[in] condition Condition needed to be met after `operation` execution
 * @param[in] on_fail   Action to be performed if `condtition` is not met
 */
#define LOG_CATCH_ERROR(operation, condition, on_fail) do                   \
{                                                                           \
    LOG_PRINT_TRACE(operation, NULL, NULL);                                 \
    LOG_ASSERT(MSG_ERROR, condition, on_fail);                              \
} while (0)

/**
 * @brief 
 * Trace action execution. If condition is not met after execution,
 * abort program execution
 * 
 * @param[in] operation Traced operation
 * @param[in] condition Condition needed to be met after `operation` execution
 */
#define LOG_ERROR_FATAL(operation, condition) do                                        \
{                                                                                       \
    LOG_PRINT_TRACE(operation, NULL);                                                   \
    LOG_ASSERT(MSG_FATAL, condition, {log_stop(); abort();});                           \
} while (0)

#endif
