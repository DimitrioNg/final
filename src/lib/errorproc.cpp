#include <stdarg.h> //va_list
#include <stdio.h>  //vsnprintf, fputs
#include <string.h> //strlen, strcat
#include <stdlib.h> // exit 
#include <syslog.h> //syslog
#include <errno.h>

#define MAXLINE 4096
 
 
 /*
 * Выводит сообщение и возвращает управление в вызывающую функцию.
 * Вызывающая функция определяет значение флага "errnoflag".
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap) {
    char    buf[MAXLINE];
    vsnprintf(buf, MAXLINE-1, fmt, ap); 
    if (errnoflag)
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s", strerror(error)); 
    strcat(buf, "\n");
    fflush(stdout); /* в случае, когда stdout и stderr - одно и то же устройство */
    fputs(buf, stderr); 
    fflush(NULL);   /* сбрасывает все выходные потоки */
    }
 
 
 /*
 * Обрабатывает фатальные ошибки, не связанные с системными вызовами.
 * Выводит сообщение и завершает работу процесса.
 */
void err_quit(const char *fmt, ...) {
    va_list     ap;
    va_start(ap, fmt);
    err_doit(0, 0, fmt, ap);
    va_end(ap);
    exit(1);
    }

/*
 * Процедуры обработки ошибок для демона.
 */  

/*
 * В вызывающем процессе должна быть определена и установлена эта переменная:
 * ненулевое значение - для интерактивных программ, нулевое – для демонов
*/
extern int log_to_stderr;

/*
 * Выводит сообщение и возвращает управление в вызывающую функцию.
 * Вызывающая функция должна определить значения аргументов
 * "errnoflag" и "priority".
 */
static void log_doit(int errnoflag, int error, int priority, const char *fmt, va_list ap){
    char buf[MAXLINE];
    vsnprintf(buf, MAXLINE-1, fmt, ap);
    if (errnoflag) snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s", strerror(error));
    strcat(buf, "\n");
    if (log_to_stderr) {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
        } 
    else {
        syslog(priority, "%s", buf);
        }
    }

/*
 * Инициализировать syslog(), если процесс работает в режиме демона.
 */
void log_open(const char *ident, int option, int facility) {
    if (log_to_stderr == 0)
        openlog(ident, option, facility); 
    }

/*
 * Обрабатывает нефатальные ошибки, связанные с системными вызовами. 
 * Выводит сообщение, соответствующее содержимому переменной errno, 
 * и возвращает управление.
 */
void log_ret(const char *fmt, ...) {
    va_list     ap;
    va_start(ap, fmt);
    log_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
    }
/*
 * Обрабатывает фатальные ошибки, связанные с системными вызовами.
 * Выводит сообщение и завершает работу процесса.
 */
void log_sys(const char *fmt, ...) {
    va_list     ap;
    va_start(ap, fmt);
    log_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
    }
/*
 * Обрабатывает нефатальные ошибки, не связанные с системными вызовами.
 * Выводит сообщение и возвращает управление.
 */
void log_msg(const char *fmt, ...) {
    va_list     ap;
    va_start(ap, fmt);
    log_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
    }
/*
 * Обрабатывает фатальные ошибки, не связанные с системными вызовами.
 * Выводит сообщение и завершает работу процесса.
 */
void log_quit(const char *fmt, ...){
    va_list     ap;
    va_start(ap, fmt);
    log_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
    }
/*
 * Обрабатывает фатальные ошибки, связанные с системными вызовами.
 * Номер ошибки передается в параметре.
 * Выводит сообщение и завершает работу процесса.
 */
void log_exit(int error, const char *fmt, ...) {
    va_list     ap;
    va_start(ap, fmt);
    log_doit(1, error, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
    }
