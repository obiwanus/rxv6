#ifndef XV6_CONSOLE_H
#define XV6_CONSOLE_H

void log(const char *func, const char *type, char *fmt, ...);

#define LOG_ERROR(args...) log(__func__, "ERROR", args)
#define LOG_INFO(args...) log(__func__, "INFO", args)

void panic(const char *func, char *fmt, ...);

#define PANIC(args...) panic(__func__, args)

#endif  // XV6_CONSOLE_H
