#ifndef __HELLBENDER_KERNEL_LOG_H__
#define __HELLBENDER_KERNEL_LOG_H__

void log_debug(const char *component, const char *function, const char *message);

void log_info(const char *component, const char *function, const char *message);

void log_warn(const char *component, const char *function, const char *message);

__attribute__((__noreturn__))
void log_error(const char *component, const char *function, const char *message);

#endif
