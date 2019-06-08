/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/printk.h>
#include <lib/vsprintf.h>
#include <lib/string.h>
#include <wk/log.h>
#include <wk/clk.h>
#include <wk/cpu.h>

#ifdef CONFIG_DEBUG

static bool log_enable = true;

__printf(2,3) int pr_log(uint8_t grade, const char *fmt, ...)
{
	va_list args;
	static char buf[PR_LOG_BUG_SIZE];
	uint32_t time_sec, time_usec;
	int len;

    if (log_enable) {
        time_sec = (uint32_t)tick_to_sec_64(get_run_tick());
        time_usec = (uint32_t)((tick_to_msec_64(get_run_tick()) % 1000) *
            1000 + get_cpu_run_time_by_sys_bead());

        switch(grade) {
            case LOG_FATAL:
                sprintf(buf, "%06d.%06d[FATAL]",  time_sec, time_usec);
                len = 20;
                break;
            case LOG_ERROR:
                sprintf(buf, "%06d.%06d[ERROR]",  time_sec, time_usec);
                len = 20;
                break;
            case LOG_WARNING:
                sprintf(buf, "%06d.%06d[WARNING]",  time_sec, time_usec);
                len = 22;
                break;
            case LOG_INFO:
                sprintf(buf, "%06d.%06d[INFO]",  time_sec, time_usec);
                len = 19;
                break;
            default:
                return 0;
        }

        va_start(args, fmt);
        len += vsprintf(buf + len, fmt, args);
        va_end(args);

        len = write_log(buf, len);

        return len;
    }

    return 0;
}

void pr_log_enable(bool enable)
{
    log_enable = enable;
}

#endif
