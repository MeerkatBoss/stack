#include <math.h>
#include <stdarg.h>
#include <time.h>

#include "logger.h"
#include "text_styles.h"

// TODO: Why use defines here? static const will do
#define MSG_TRACE_TEXT  "~trace~"
#define MSG_INFO_TEXT   "Info"
#define MSG_WARNING_TEXT "WARNING"
#define MSG_ERROR_TEXT  "ERROR"
#define MSG_FATAL_TEXT  "!!FATAL!!"

static size_t loggers_count_ = 0;
static logger* loggers_[MAX_LOGGERS_COUNT];
static int paused = 0;

static int trash_ = atexit(log_stop);
//                  ^~~~~~ TODO: definitely possible, but it's very easy for user
//                               to accidently override your atexit by calling it again
//                               with a different function provided.
//
//                               If you want you could make a system that will allow
//                               multiple atexit functions (by calling atexit on some
//                               dispatching function).
//
//                               It won't solve problem, but will, at least, provide an
//                               alternative for atexit that doesn't break your logging
//                               system.

void add_logger(logger added)
{
    LOG_ASSERT(MSG_ERROR,
        loggers_count_ < MAX_LOGGERS_COUNT,
        {return;});

    logger* log_ptr = (logger*)calloc(1, sizeof(*log_ptr));

    LOG_ASSERT(MSG_ERROR,
        log_ptr != NULL,
        {return;});

    *log_ptr = added;
    loggers_[loggers_count_++] = log_ptr;

    setbuf(added.stream, NULL);

    if(added.settings_mask & LGS_USE_HTML)
    {
        fputs("<!DOCTYPE html>",added.stream);
        fprintf(added.stream, "<head><title>%s</title></head>", added.name);
        fputs("<body><pre>", added.stream);
    }

}

void add_custom_logger(logger* added) // TODO: very similar function to the one above, extract!
{
    LOG_ASSERT(MSG_ERROR,
        loggers_count_ < MAX_LOGGERS_COUNT,
        {return;});
    //  ^~~~~~~~ TODO: please, do something with this style

    loggers_[loggers_count_++] = added;   
    setbuf(added->stream, NULL); // TODO: If you want to disable buffering, use setvbuf (with IONBF)

    if(added->settings_mask & LGS_USE_HTML)
    {
        fputs("<!DOCTYPE html>",added->stream);
        fprintf(added->stream, "<head><title>%s</title></head>", added->name);
        fputs("<body><pre>", added->stream);
    }
}

void add_default_file_logger(void)
{
    add_logger({
        .name           = "Default file logger",
        .stream         = fopen("default.log", "a"),
        .logging_level  = LOG_ALL,
        .settings_mask  = 0
    });
}

void add_default_console_logger(void) // TODO: shouldn't default logger be added automatically?
{
    add_logger({
        .name           = "Default console logger",
        .stream         = stdout,
        .logging_level  = LOG_INFO,
        .settings_mask  = LGS_KEEP_OPEN | LGS_USE_ESCAPE
    });
}

// TODO: use __attribute__((format (printf 1, 2))), is gonna
//       get you many of the compiler enforced goods for your
//       function! You are gonna love it!
void log_message(message_level level, const char* format, ...)
{
    // TODO: please, align this macro on one line, so it's readable 
    //                                       v~~~~~~~~~~~~~
    #define COLORED_CASE(var, type, setting) case MSG_##type:\
            var = TEXT_##setting##_##type (MSG_##type##_TEXT); break;

    // TODO: consider aligning "\", makes it look cleaner 
    #define ALL_CASES(lvl, var, setting) switch(lvl)\
    {\
        COLORED_CASE(var, TRACE,   setting)\
        COLORED_CASE(var, INFO,    setting)\
        COLORED_CASE(var, WARNING, setting)\
        COLORED_CASE(var, ERROR,   setting)\
        COLORED_CASE(var, FATAL,   setting)\
        default:\
            msg_type = "??UNKNOWN??";\
            break;\
    } // TODO: can you move this macro closer to its usage?

    // TODO: I think this could be extracted in function
    const int MAX_DATE_SIZE = 32;
    char time_str[MAX_DATE_SIZE] = "";
    time_t cur_time = time(NULL);
    struct tm time_struct = {};
    localtime_r(&cur_time, &time_struct);
    strftime(time_str, MAX_DATE_SIZE, "%F %T%z", &time_struct);

    va_list vlist = {};
    va_start(vlist, format);

    for (size_t i = 0; i < loggers_count_; i++)
    {
        logger* current_logger = loggers_[i];
        if (paused && !(current_logger->settings_mask & LGS_LOG_ALWAYS))
            continue;
        
        if (current_logger->settings_mask & LGS_USE_ESCAPE)
        {
            const char* msg_type = "";
            ALL_CASES(level, msg_type, ESCAPED)
            fprintf(current_logger->stream,
                    TEXT_ESCAPED_NOTE("%s") "\t[%s]:\t",
                    time_str, msg_type);
        }
        else if (current_logger->settings_mask & LGS_USE_HTML)
        {
            const char* msg_type = ""; // TODO: Still boilerplate, you can do better)
            ALL_CASES(level, msg_type, HTML)
            fprintf(current_logger->stream,
                    "<p>" TEXT_HTML_NOTE("%s") "\t[%s]:\t",
                    time_str, msg_type);
        }
        else
        {
            const char* msg_type = "";
            switch (level) // TODO: You've made a macro, why don't you use it here?
            {
                case MSG_TRACE:     msg_type = MSG_TRACE_TEXT;  break;
                case MSG_INFO:      msg_type = MSG_INFO_TEXT;   break;
                case MSG_WARNING:   msg_type = MSG_WARNING_TEXT;break;
                case MSG_ERROR:     msg_type = MSG_ERROR_TEXT;  break;
                case MSG_FATAL:     msg_type = MSG_FATAL_TEXT;  break;
                default:            msg_type = "??UNKNOWN??";   break;
                // TODO: Maybe default is assert worthy?
            }
            fprintf(current_logger->stream, "<%s>\t[%s]\t", time_str, msg_type);
        }

        va_list tmp_vlist = {};
        va_copy(tmp_vlist, vlist);
        vfprintf(current_logger->stream, format, tmp_vlist);

        if (current_logger->settings_mask & LGS_USE_HTML)
            fputs("</p>\n", current_logger->stream);
        else
            fputc('\n', current_logger->stream);
    }

    va_end(vlist);

    #undef COLORED_CASE
    #undef ALL_CASES
}

void log_pause(void)    {paused = 1;}
// TODO:            ^~~~ ^~~~~~~~~~~ too little space, weird style :/
//                  |
//                  what are you aligning? ghost?

void log_resume(void)   {paused = 0;}
// TODO:                ^           ^ same thing!

void log_stop(void)
{
    for (size_t i = 0; i < loggers_count_; i++)
    {
        logger* current_logger = loggers_[i];
        if (current_logger->settings_mask & LGS_USE_HTML)
            fputs("</pre></body>\n", current_logger->stream);
        if (!(current_logger->settings_mask & LGS_KEEP_OPEN))
            fclose(current_logger->stream);
        free(current_logger); // TODO: consider zeroing pointer after free
        // Also, shouldn't this delete logger id est reduce logger_count?
    }
}
