#include <math.h>

#include "logger.h"

const char *const MESSAGE_CLASS[] =
{
    "TRACE",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
};

static size_t loggers_count_ = 0;
static logger* loggers_[MAX_LOGGERS_COUNT];
static size_t nonstop_loggers_count_ = 0;
static logger* nonstop_loggers_[MAX_LOGGERS_COUNT];
static int paused = 0;

static int trash_ = atexit(log_stop);

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

    if(added.settings_mask & LGS_LOG_ALWAYS)
        nonstop_loggers_[nonstop_loggers_count_++] = log_ptr;

    if(added.settings_mask & LGS_USE_HTML)
    {
        fputs("<!DOCTYPE html>",added.stream);
        fprintf(added.stream, "<head><title>%s</title></head>", added.name);
        fputs("<body><pre>", added.stream);
    }
}
