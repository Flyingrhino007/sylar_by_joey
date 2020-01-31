#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "singleton.h"
#include "util.h"

#define SYLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(), \
                        sylar::GetFiberId(), time(0)))).getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(), \
                        sylar::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

namespace sylar {

class Logger;

// Log level
class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
};


// Log event
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            , const char* file, int32_t m_line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time);
//    ~LogEvent();

    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    std::string getContent() const { return m_ss.str();}
    std::shared_ptr<Logger> getLogger() { return m_logger;}
    LogLevel::Level getLevel() { return m_level;}

    std::stringstream& getSS() { return m_ss;}          // 返回的m_ss，放在 << 左边，接收字符串
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
        
private:
    const char* m_file = nullptr;   // file name
    int32_t m_line = 0;             // line number
    uint32_t m_elapse = 0;          // milliseconds since program start
    uint32_t m_threadId = 0;        // thread id
    uint32_t m_fiberId = 0;         // coroutine id
    uint64_t m_time = 0;            // timestamp
    std::string m_content;
    std::stringstream m_ss;

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap {
    // 就拿来存储LogEvent的智能指针
    
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    LogEvent::ptr getEvent() { return m_event;}
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

// Log formatter
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);       // 实例化的时候输入pattern

    // %t   %thread_id %m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    // 日志解析子模块
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;    // 智能指针
//        FormatItem(const std::string& fmt = "") {};
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();                                // 解析pattern
private:
    // 日志格式模板
    std::string m_pattern;                 
    // 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;       // 定义很多不同的子类，每个子类负责输出对应的内容
    // 是否有错误
    bool m_error = false;

private:

};


// Log output path
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    void setLevel(LogLevel::Level val) { m_level = val;}

    LogFormatter::ptr getFormatter() const { return m_formatter;}
protected:
    // protected 子类可用
    LogLevel::Level m_level = LogLevel::DEBUG;      // 初始化level，不然会随机赋值，造成无法正常输出
    LogFormatter::ptr m_formatter;                  // 记录的格式
};

// Logger
class Logger : public std::enable_shared_from_this<Logger>{
    // 只有定义了enable_from_this，该类才能在自己的成员函数中获得指针
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val) {m_level = val;}

    const std::string& getName() const { return m_name;}
private:
    std::string m_name;         // Logger name
    LogLevel::Level m_level;    // log level
    std::list<LogAppender::ptr> m_appenders;    // Appender set
    LogFormatter::ptr m_formatter;              // 初始化时使用
};

// Appender to console
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;   // override描述从虚基类继承出来的
};

// Appender to file
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& flename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    // 重新打开文件，文件打开成功，返回true
    bool reopen();
private:
    /// 文件路径及文件名
    std::string m_filename;
    /// 文件流
    std::ofstream m_filestream;
    /// 上次重新打开时间
    uint64_t m_latTime = 0;
};

class LoggerManager {
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();                    // 方便从配置文件直接读入

    Logger::ptr getRoot() const { return m_root;} 
private:
    std::map<std::string, Logger::ptr> m_logger;
    Logger::ptr m_root;
};

typedef sylar::Singleton<LoggerManager> LoggerMgr;

}
#endif
